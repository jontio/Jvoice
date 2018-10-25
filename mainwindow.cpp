#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //populate devices
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))ui->comboBox->addItem(deviceInfo.deviceName());





}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    qDebug()<<arg1;

    if(!m_audioInput.isNull())m_audioInput->stop();

    QAudioFormat format;
    // Set up the desired format, for example:
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        if(arg1==deviceInfo.deviceName())
        {
            info=deviceInfo;
        }
    }

    if (!info.isFormatSupported(format))
    {
        qWarning() << "Format not supported!!!";
        return;
    }

    jaudiodevice.reset(new JAudio(format));
    m_audioInput.reset(new QAudioInput(info, format));

    connect(jaudiodevice.data(),SIGNAL(power_signal(bool)),ui->led,SLOT(setPower(bool)));

    jaudiodevice->start();
    m_audioInput->setBufferSize(16000);
    m_audioInput->start(jaudiodevice.data());

}
