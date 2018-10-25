#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include "jaudio.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
     QScopedPointer<QAudioInput> m_audioInput;


     QScopedPointer<JAudio> jaudiodevice;
};

#endif // MAINWINDOW_H
