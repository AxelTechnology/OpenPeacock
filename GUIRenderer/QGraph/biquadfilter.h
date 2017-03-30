#ifndef BIQUADFILTER_H
#define BIQUADFILTER_H

#include <QObject>
#include <math.h>


enum BiQuadFilterType{
    BiQuadFilterType_lp = 0,
    BiQuadFilterType_hp = 1,
    BiQuadFilterType_lowshelf = 2,
    BiQuadFilterType_highshelf = 3,
    BiQuadFilterType_peak = 4,
    BiQuadFilterType_allpass1 = 5,
    BiQuadFilterType_allpass2 = 6,
    BiQuadFilterType_notch = 7,
    BiQuadFilterType_bp_peak = 8,
    BiQuadFilterType_bp_0db = 9

};


class BiQuadFilter
{
public:

    BiQuadFilter();
    void CalcFilterCoefficients(BiQuadFilterType type, double sr, double f, double p1, double gain);
    void SetFilterCoefficients(double a0, double a1, double a2, double b0, double b1, double b2);
    double GetSample(double x, double sampleRate);

    //Temporarly public for debug purpose
//    double a0; // multiply factor on x (input)
//    double a1; // multiply factor on x1 (input-1)
//    double a2; // multiply factor on x2 (input-2)
//    double b1; // multiply factor on y1 (output-1)
//    double b2; // multiply factor on y1 (output-2)



private:

        void ResetSamples();
        double square(double x);

        double TWOPI = 2.0 * M_PI;

        double x1; // x (input) minus 1
        double x2; // x (input) minus 2
        double y1; // y (output) minus 1
        double y2; // y (output) minus 2

        double a0; // multiply factor on x (input)
        double a1; // multiply factor on x1 (input-1)
        double a2; // multiply factor on x2 (input-2)
        double b0; // multiply factor on y (onput)
        double b1; // multiply factor on y1 (output-1)
        double b2; // multiply factor on y1 (output-2)

        double xy1;
        double xy2;
};

#endif // BIQUADFILTER_H
