#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define _USE_MATH_DEFINES

#include <cmath>
#include <portaudio.h>
#include <QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

private slots:
   void onFrequencyChanged(int value);

private:
   QSlider *m_slider;
   PaStream *m_stream = nullptr;
   double m_currentFreq = 440.0;
   static int audioCallback(const void *input, void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo *timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData);
};

#endif // MAINWINDOW_H