#ifndef JAUDIO_H
#define JAUDIO_H

#include <QObject>
#include <QIODevice>
#include "jfft/jfft.h"
#include "dsp.h"
#include <QFile>
#include <QDataStream>
#include <QAudioFormat>
#include <complex>
#include <armadillo>

class JAudio : public QIODevice
{
    Q_OBJECT
public:
    explicit JAudio(const QAudioFormat &format);
    ~JAudio();


    void start();
    void stop();


    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

    JFFT fft;
    CircBuffer<double> circbuffer;
    HannWindow<double> hannwindow;


    QVector<JFFT::cpx_type> fft_output;

    QFile file;
    QTextStream dataout;

    const QAudioFormat m_format;

    arma::mat W1,W2;
    arma::vec b1_vec,b2_vec,sig;
    arma::vec x_vec,y_vec,h_vec;


    QList<JFastFir> firs;

    CircBuffer<int> utterances;
    int last_utterance=0;

signals:
    void power_signal(bool on_off);




};

#endif // JAUDIO_H
