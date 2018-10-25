#include "dsp.h"

using namespace arma;

inline double sigmoid(double x)
{
    return (1.0/(1.0+std::exp(-x)));
}

inline double sigmoiddiv(double x)
{
    double val=(1.0+exp(-x));
    val*=val;
    return (std::exp(-x)/(val));
}

vec sigmoid(const vec &v)
{
    vec ret_vec=v;
    for(uint i=0;i<v.n_rows;i++)
    {
        ret_vec(i)=sigmoid(v(i));
    }
    return ret_vec;
}

void normalize(QVector<double> &x)
{
    double maxval=-1;
    for(int i=0;i<x.size();i++)
    {
        double val=fabs(x[i]);
        if(val>maxval)maxval=val;
    }
    for(int i=0;i<x.size();i++)
    {
        x[i]/=maxval;
    }
}
