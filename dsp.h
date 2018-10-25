#ifndef DSP_H
#define DSP_H


#include <QVector>
#include <assert.h>
#include <math.h>
#include <armadillo>
#include "jfft/jfft.h"

using namespace arma;


inline double sigmoid(double x);
inline double sigmoiddiv(double x);
vec sigmoid(const vec &v);

void normalize(QVector<double> &x);

template <class T>
class HannWindow
{
public:
    HannWindow()
    {
        setLength(12);
    }
    void setLength(int length)
    {
        // sin(pi*(0:0.1:99)/(100-1)).^2
        assert(length>0);
        window.resize(length);
        assert(length==window.size());
        for(int idx=0;idx<length;idx++)
        {
            double val=sin(M_PI*((double)idx)/((double)(length-1))  );
            val*=val;
            window[idx]=val;
        }
    }
    void window_data(QVector<T> &data)
    {
        if(data.size()!=window.size())
        {
            setLength(data.size());
        }
        int length=data.size();
        for(int idx=0;idx<length;idx++)
        {
            data[idx]*=window[idx];
        }
        return;
    }
private:
    QVector<double> window;
};

template <class T>
class CircBuffer
{
public:
    CircBuffer()
    {
        setLength(12);
    }
    void setLength(int length)
    {
        assert(length>0);
        buffer.resize(length);
        buffer_ptr=0;
        buffer_sz=buffer.size();
    }
    void update(T &data)
    {
        buffer[buffer_ptr]=data;
        buffer_ptr++;buffer_ptr%=buffer_sz;
        return;
    }
    QVector<T> &calc_return_buffer()
    {
        return_buffer.resize(buffer.size());
        int tmp=buffer_ptr;
        for(int i=0;i<buffer.size();i++)
        {
            return_buffer[i]=buffer[buffer_ptr];
            buffer_ptr++;buffer_ptr%=buffer_sz;
        }
        assert(buffer_ptr==tmp);
        return return_buffer;
    }
    QVector<T> return_buffer;
private:
    QVector<T> buffer;
    int buffer_ptr;
    int buffer_sz;
};


#endif // DSP_H
