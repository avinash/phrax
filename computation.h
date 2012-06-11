#ifndef COMPUTATION_H
#define COMPUTATION_H

#include "fractal.h"
#include "mpi.h"
#include "metadata.h"

class Computation
{
private:
/*    double conx;
    double cony;*/
public:	
    Computation();
    ~Computation();
    void render( double , double , double , double );
    int mandel(int,int,double,double,double,double , int );
    int julia( int , int , double , double , double , double , float , float );
    uint rgbFromWaveLength(double wave);

};

#endif
