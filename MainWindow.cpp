#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
   QWidget *centralWidget = new QWidget(this);
   setCentralWidget(centralWidget);

   m_slider = new QSlider(Qt::Horizontal);
   m_slider->setRange(200, 800); // частота от 200 до 800 Гц
   m_slider->setValue(440);

   QPushButton *btn = new QPushButton("Play Tone (440 Hz)");

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addWidget(m_slider);
   layout->addWidget(btn);
   centralWidget->setLayout(layout);

   connect(m_slider, &QSlider::valueChanged, this, &MainWindow::onFrequencyChanged);
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
      out[i] = 0.3f * std::sin(2.0 * M_PI * self->m_currentFreq * phase / sampleRate);
      phase += 1.0;
      if (phase >= sampleRate)
         phase -= sampleRate;
   }

   return paContinue;
}

// Добавьте слот, который запускает воспроизведение
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
   if (m_stream)
   {
      Pa_StopStream(m_stream);
      Pa_CloseStream(m_stream);
      Pa_Terminate();
   }
}