#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_stream(nullptr), m_currentFreq(440.0), m_isPlaying(false)
{
   QWidget *centralWidget = new QWidget(this);
   setCentralWidget(centralWidget);

   m_slider = new QSlider(Qt::Horizontal);
   m_slider->setRange(200, 800); // частота от 200 до 800 Гц
   m_slider->setValue(440);

   m_toggleButton = new QPushButton("Play");

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addWidget(m_slider);
   layout->addWidget(m_toggleButton);
   centralWidget->setLayout(layout);

   connect(m_slider, &QSlider::valueChanged, this, &MainWindow::onFrequencyChanged);
   connect(m_toggleButton, &QPushButton::clicked, this, &MainWindow::onTogglePlayback);
}

int MainWindow::audioCallback(const void *input, void *output,
                              unsigned long frameCount,
                              const PaStreamCallbackTimeInfo *timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData)
{
   MainWindow *self = static_cast<MainWindow *>(userData);
   float *out = static_cast<float *>(output);
   static double phase = 0.0;
   const double sampleRate = 44100.0;

   for (unsigned long i = 0; i < frameCount; ++i)
   {
      out[i] = 0.3f * std::sin(2.0 * M_PI * self->m_currentFreq * self->m_phase / sampleRate);
      self->m_phase += 1.0;
      if (self->m_phase >= sampleRate)
         self->m_phase -= sampleRate;
   }

   return paContinue;
}

void MainWindow::onTogglePlayback()
{
   if (m_isPlaying)
   {
      Pa_StopStream(m_stream);
      Pa_CloseStream(m_stream);
      m_stream = nullptr;
      m_isPlaying = false;
      m_toggleButton->setText("Play");
      m_phase = 0.0;
   }
   else
   {
      PaError err = Pa_Initialize();
      if (err != paNoError)
         return;

      PaStreamParameters outputParams;
      outputParams.device = Pa_GetDefaultOutputDevice();
      if (outputParams.device == paNoDevice)
      {
         Pa_Terminate();
         return;
      }

      outputParams.channelCount = 1;
      outputParams.sampleFormat = paFloat32;
      outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
      outputParams.hostApiSpecificStreamInfo = nullptr;

      err = Pa_OpenStream(&m_stream, nullptr, &outputParams, 44100, 256, paNoFlag,
                          MainWindow::audioCallback, this);
      if (err != paNoError)
      {
         Pa_Terminate();
         return;
      }

      err = Pa_StartStream(m_stream);
      if (err != paNoError)
      {
         Pa_CloseStream(m_stream);
         m_stream = nullptr;
         Pa_Terminate();
         return;
      }

      m_isPlaying = true;
      m_toggleButton->setText("Stop");
   }
}

void MainWindow::onFrequencyChanged(int value)
{
   m_currentFreq = static_cast<double>(value);
   if (!m_stream)
   {
      Pa_Initialize();
      PaStreamParameters outputParams;
      outputParams.device = Pa_GetDefaultOutputDevice();
      outputParams.channelCount = 1;
      outputParams.sampleFormat = paFloat32;
      outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
      outputParams.hostApiSpecificStreamInfo = nullptr;

      Pa_OpenStream(&m_stream, nullptr, &outputParams, 44100, 256, paNoFlag,
                    MainWindow::audioCallback, this);
      Pa_StartStream(m_stream);
   }
}

MainWindow::~MainWindow()
{
   if (!m_isPlaying)
      return;

   Pa_StopStream(m_stream);
   Pa_CloseStream(m_stream);
   m_stream = nullptr;
   m_isPlaying = false;
   m_phase = 0.0;
}