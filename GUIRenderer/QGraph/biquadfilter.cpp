#include "biquadfilter.h"
#include <math.h>

BiQuadFilter::BiQuadFilter()
{

}

void BiQuadFilter::CalcFilterCoefficients(BiQuadFilterType type, double sr, double f, double p1, double gain)
{
    //double b0 = 1.0;
    double w0 = (TWOPI * f) / sr;
    double sin_  = sin(w0);
    double cos_  = cos(w0);

    if (type== BiQuadFilterType_lp) {
        /* p1 indicates at what gain level the graph will be at desired freq
        ** so f=1000 and p1=-3dB=0.707 will have the filter cross -3dB at 1000Hz
        ** values >0dB are not recomended */
        //double Q = Math.pow(10.0, (gain / 20));
        double Q = p1;
        double alpha = sin_ / (2.0 * Q);
        a0 = (1.0 - cos_) / 2.0;
        a1 = (1.0 - cos_);
        a2 = (1.0 - cos_) / 2.0;
        b0 =  1.0 + alpha;
        b1 = -2.0 * cos_;
        b2 =  1.0 - alpha;
    }
    if (type==BiQuadFilterType_hp) {
        /* See lp */
        //double Q = Math.pow(10.0, (gain / 20));
        double Q = p1;
        double alpha = sin_ / (2.0 * Q);
        a0 =  (1.0 + cos_) / 2.0;
        a1 = -(1.0 + cos_);
        a2 =  (1.0 + cos_) / 2.0;
        b0 =  (1.0 + alpha);
        b1 = -(2.0 * cos_);
        b2 =  (1.0 - alpha);
    }
    if (type==BiQuadFilterType_bp_peak) {
        /* The gain parameter determines where the peak of the BP will be */
        double A = pow(10, gain/20);
        double alpha = sin_ / (2.0 * A);
        a0 =   A*alpha;
        a1 =   0.0;
        a2 =  -A*alpha;
        b0 =   1.0 + alpha;
        b1 =  -2.0 * cos_;
        b2 =   1.0 - alpha;
    }
    if (type==BiQuadFilterType_bp_0db) {
        /* p1 indicates the bandwith in octaves at the -3 dB level */
        double BW = p1;
        double alpha = sin_* sinh( log(2)/2 * BW * w0/sin_ );
        a0 =   alpha;
        a1 =   0.0;
        a2 =  -alpha;
        b0 =   1.0 + alpha;
        b1 =  -2.0*cos_;
        b2 =   1.0 - alpha;
    }
    if (type==BiQuadFilterType_peak) {
        /* p1 contains the bandwidth in octaves at 1/2 gain */
        double BW = p1;
        double A = pow (10.0, (gain / 40.0));
        double alpha = sin_* sinh( log(2)/2 * BW * w0/sin_ );
        a0 =  1.0 + (alpha * A);
        a1 = -2.0 * cos_;
        a2 =  1.0 - (alpha * A);
        b0 =  1.0 + (alpha / A);
        b1 = -2.0 * cos_;
        b2 =  1.0 - (alpha / A);
    }
    if (type==BiQuadFilterType_notch) {
        /* p1 indicates the bandwidth in octaves at the -3 dB level */
        double BW = p1;
        double alpha = sin_* sinh( log(2)/2 * BW * w0/sin_ );
        a0 =   1.0;
        a1 =  -2.0 * cos_;
        a2 =   1.0;
        b0 =   1.0 + alpha;
        b1 =  -2.0 * cos_;
        b2 =   1.0 - alpha;
    }

    if (type==BiQuadFilterType_lowshelf) {
        /* p1 indicates the shelf slope parameter S. This parameter detemines the steepness
         * of the slope. With this biquad filter, given a gain of 2 dB and S=1 the slope will
         * be 1 dB per octave. A value of S=0.5 will divide the steepness in half, resulting
         * in 1/2 dB per octave. S=1 is the maximum value at which the filter will work without
         * causing a ripple (or overshoot).
         */
        double S = p1;
        double A = pow (10.0, (gain / 40.0));
        double alpha = sin_ /2.0 * sqrt( (A + 1.0/A)*(1.0/S - 1.0) + 2.0 );
        double beta = 2.0 * sqrt(A) * alpha;

        a0 =         A * ((A + 1) - (A - 1) * cos_ + beta);
        a1 = 2.0 * A * ((A - 1) - (A + 1) * cos_);
        a2 =         A * ((A + 1) - (A - 1) * cos_ - beta);
        b0 =              (A + 1) + (A - 1) * cos_ + beta;
        b1 =      -2.0 * ((A - 1) + (A + 1) * cos_);
        b2 =              (A + 1) + (A - 1) * cos_ - beta;
    }
    if (type==BiQuadFilterType_highshelf) {
        /* See lowshelf */
        double S = p1;
        double A = pow (10.0, (gain / 40.0));
        double alpha = sin_ /2.0 * sqrt( (A + 1.0/A)*(1.0/S - 1.0) + 2.0 );
        double beta = 2.0 * sqrt(A) * alpha;

        a0 =          A * ((A + 1) + (A - 1) * cos_ + beta);
        a1 = -(2.0 * A) * ((A - 1) + (A + 1) * cos_);
        a2 =          A * ((A + 1) + (A - 1) * cos_ - beta);
        b0 =               (A + 1) - (A - 1) * cos_ + beta;
        b1 =        2.0 * ((A - 1) - (A + 1) * cos_);
        b2 =               (A + 1) - (A - 1) * cos_ - beta;
    }
    if (type==BiQuadFilterType_allpass1) {
        double Q = p1;
        double alpha = sin_ / (2.0 * Q);
        a0 =  -(1.0 - alpha);
        a1 =   (2.0 * cos_);
        a2 =  -(1.0 + alpha);
        b0 =  1.0 + alpha;
        b1 = -2.0 * cos_;
        b2 =  1.0 - alpha;
    }
    if (type==BiQuadFilterType_allpass2) {
        double Q = p1;
        double alpha = sin_ / (2.0 * Q);
        a0 =   (1.0 - alpha);
        a1 =  -(2.0 * cos_);
        a2 =   (1.0 + alpha);
        b0 =  1.0 + alpha;
        b1 = -2.0 * cos_;
        b2 =  1.0 - alpha;
    }

    a0 /= b0;
    a1 /= b0;
    a2 /= b0;
    b1 /= b0;
    b2 /= b0;

    ResetSamples();
}

void BiQuadFilter::SetFilterCoefficients(double a0, double a1, double a2, double b0, double b1, double b2)
{
    this->a0 = a0;
    this->a1 = a1;
    this->a2 = a2;
    this->b0 = b0;
    this->b1 = b1;
    this->b2 = b2;

    ResetSamples();
}

void BiQuadFilter::ResetSamples()
{
    x1=0.0;
    x2=0.0;
    y1=0.0;
    y2=0.0;
    xy1=0.0;
    xy2=0.0;
}



double BiQuadFilter::GetSample(double x, double sampleRate)
{
    double y;
//
// Direct form I (good for fixed point processing)
//
//        y  = (a0 * x) + (a1 * x1) + (a2 * x2) - (b1 * y1) - (b2 * y2);
//        x2 = x1;
//        x1 = x;
//        y2 = y1;
//        y1 = y;

//
// Direct form II (good for floating point processing)
//
//        y = (a1 * xy1) + (a2 * xy2);
//        double tmp = x - (b1 * xy1) - (b2 * xy2);
//        y += (a0 * tmp);
//        xy2 = xy1;
//        xy1 = tmp;

//
// Transposed direct form II (even better for floating point processing)
//
//    y = (a0 * x) + xy1;
//    xy1 = (a1 * x) + xy2 - (b1 * y);
//    xy2 = (a2 * x) - (b2 * y);



    //WOW THIS ACTUALLY WORKS!
    //w = frequency (0 < w < PI)
    //square(x) = x*x
    double w = (x /sampleRate * 2 * M_PI) ;


    y = 20*log10((sqrt(square(a0*square(cos(w))-a0*square(sin(w))+a1*cos(w)+a2)+square(2*a0*cos(w)*sin(w)+a1*(sin(w))))/
                      sqrt(square(b0*square(cos(w))-b0* square(sin(w))+b1*cos(w)+b2)+square(2* b0* cos(w)*sin(w)+b1*(sin(w))))));
    if(b0 != 1)
        y = -y;

    return y;
}


double BiQuadFilter::square(double x)
{
    return x*x;
}


