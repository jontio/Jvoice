#include "jaudio.h"
#include <QDebug>
#include <qendian.h>
#include <QMessageBox>

using namespace std;
using namespace arma;


JAudio::JAudio(const QAudioFormat &format)
    : m_format(format)
{

    circbuffer.setLength(256);

    utterances.setLength(4);

    //W1,W2;
    //B1,B2;
    //x_vec,y_vec,h_vec;

    //%setup sizes
    int m=256/2+1;
    int k=400;
    int n=4;

    //set input size
    x_vec=zeros<mat>(m,1);

    //load weighting and offset vectors
    if(\
            !W1.load("W1.txt", raw_ascii)||\
            !W2.load("W2.txt", raw_ascii)||\
            !b1_vec.load("B1.txt", raw_ascii)||\
            !b2_vec.load("B2.txt", raw_ascii)\
      )
    {
        QMessageBox msgBox;
        msgBox.setText("Can't fine the matricies. Please put them in the program's directory.\nThis program wont work without them.");
        msgBox.exec();
        //%fill rand numbers
        W1=randn<mat>(k,m);
        W2=randn<mat>(n,k);
        b1_vec=randn<vec>(k,1);
        b2_vec=randn<vec>(n,1);
    }

    //check loaded matricies are good sizes
    if(\
            (W1.n_rows!=(uint)k)||\
            (W1.n_cols!=(uint)m)||\
            (W2.n_rows!=(uint)n)||\
            (W2.n_cols!=(uint)k)||\
            (b1_vec.n_rows!=(uint)k)||\
            (b1_vec.n_cols!=(uint)1)||\
            (b2_vec.n_rows!=(uint)n)||\
            (b2_vec.n_cols!=(uint)1)\
       )
    {
        QMessageBox msgBox;
        msgBox.setText("Matricies are incorrect size. Please correct this.\nThis program wont work without the correct sizes.");
        msgBox.exec();
        //%fill rand numbers
        W1=randn<mat>(k,m);
        W2=randn<mat>(n,k);
        b1_vec=randn<vec>(k,1);
        b2_vec=randn<vec>(n,1);
    }

    //sig.load("delme.txt", raw_ascii);

    //LPF fir output vector
    for(int i=0;i<n;i++)
    {
        JFastFir fir;
        fir.SetKernel(JFilterDesign::LowPassHanning(7,1000,300));
        firs.push_back(fir);
    }

//    file.setFileName("e://out.txt");
//    if(!file.open(QIODevice::WriteOnly| QIODevice::Text))return;
//    dataout.setDevice(&file);
}


JAudio::~JAudio()
{

}

void JAudio::start()
{
    open(QIODevice::WriteOnly);
}

void JAudio::stop()
{
    close();
}

qint64 JAudio::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 JAudio::writeData(const char *data, qint64 len)
{

   // qDebug()<<circbuffer.return_buffer.size();

    const qint16 *ptr = reinterpret_cast<const qint16 *>(data);
    int nsamples=len/sizeof(qint16);
    for(int i=0;i<nsamples;i++)
    {
        double dval=((double)(*ptr))/32768.0;
        ptr++;

//        int static hhh=0;
//        if(hhh<sig.n_rows)dval=sig(hhh);
//        hhh++;
//        if(hhh>=sig.n_rows)
//        {
//            hhh=sig.n_rows;
//            dval=0;
//        }

        //if time to process a vector
        circbuffer.update(dval);
        static int overlapcounter=0;overlapcounter++;overlapcounter%=8;
        if(!overlapcounter)
        {            
            hannwindow.window_data(circbuffer.calc_return_buffer());
            fft.fft_real(circbuffer.return_buffer,fft_output);
            for(int k=0;k<fft_output.size()/2+1;k++)
            {
                double spectrograph_val=log(abs(fft_output[k]));
                x_vec(k)=spectrograph_val;
                //dataout<<spectrograph_val<<"\t";
            }
            //dataout<<"\n";

            //NN start
            h_vec=sigmoid(W1*x_vec+b1_vec);
            y_vec=sigmoid(W2*h_vec+b2_vec);
            //NN end

            //low pass filter the output
            for(uint k=0;k<y_vec.n_elem;k++)
            {
                y_vec(k)=firs[k].update_real_slow(y_vec(k));
            }

            //static int slowprnt=0;slowprnt++;slowprnt%=2;
            //if(!slowprnt)
            {
                //qDebug()<<y_vec(0)<<y_vec(1)<<y_vec(2)<<y_vec(3);

            }

            //dataout<<y_vec(0)<<"\t"<<y_vec(1)<<"\t"<<y_vec(2)<<"\t"<<y_vec(3)<<"\n";

            double utterance_prob=max(y_vec);
            int utterance=index_max(y_vec);
            static int counter=0;
            static int todostate=-1;
            if((utterance_prob>0.73)&&(last_utterance!=utterance))//&&(y_vec(last_utterance)<0.4))
            {
                utterances.update(utterance);
                if(utterance==3)
                {

                    //check the word
                    utterances.calc_return_buffer();
                    if(
                            (utterances.return_buffer[0]==3)&&
                            (utterances.return_buffer[1]==0)&&
                            (utterances.return_buffer[2]==1)&&
                            (utterances.return_buffer[3]==3)
                            )
                    {
                        //qDebug()<<"OFF";
                        todostate=0;
                        //emit power_signal(false);
                    }
                    if(
                            (utterances.return_buffer[0]==3)&&
                            (utterances.return_buffer[1]==0)&&
                            (utterances.return_buffer[2]==2)&&
                            (utterances.return_buffer[3]==3)
                            )
                    {
                        //qDebug()<<"ON";
                        todostate=1;
                        //emit power_signal(true);
                    }


                    qDebug()<<utterances.return_buffer;


                } //else qDebug()<<utterance;

                last_utterance=utterance;
                counter=0;

            }
             else
             {
                if(counter>1000)//wait a second to make sure we are not half way through some speech
                {
                    if(todostate==0)
                    {
                        qDebug()<<"OFF";
                        emit power_signal(false);
                    }
                    if(todostate==1)
                    {
                        qDebug()<<"ON";
                        emit power_signal(true);
                    }
                    todostate=-1;
                }
                 else counter++;

             }

        }



        //dataout<<dval<<"\n";

    }
    return len;
}
