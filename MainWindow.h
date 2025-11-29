#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include "portaudio.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
   class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

private slots:
   void onFrequencyChanged(int value);
   void onTogglePlayback();

private:
   QSlider *m_slider = nullptr;
   QPushButton *m_toggleButton = nullptr;

   PaStream *m_stream = nullptr;
   double m_currentFreq = 440.0;
   double m_phase = 0.0;
   bool m_isPlaying = false;
   static int audioCallback(const void *input, void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo *timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData);
};

#endif // MAINWINDOW_H