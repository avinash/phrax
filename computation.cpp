#include "computation.h"
#include "math.h"
#include "metadata.h"

//number of times to repeat the function
//the combination of 500 iterations and a ColorMapSize of 512
//gives a nice display
const int iterations = 500;

const int MANDEL_TYPE1 = 0;
const int MANDEL_TYPE2 = 1;
const int MANDEL_TYPE3 = 2;
const int MANDEL_TYPE4 = 3;
const int MANDEL_TYPE5 = 4;
		    
Computation::Computation()
{
//    conx = -1.25;
//    cony = 0;
}

Computation::~Computation()
{

}

int Computation::mandel(int xpt,int ypt,double xmin,double xmax,double ymin,double ymax , int _type)
{
    MetaData m;
	
    long double x = 0;
    long double y = 0;
    long double xnew = 0;
    long double ynew = 0;
    double ax;
    double ay;
    int color;
    int h;
    int xp = xpt;
    int yp = ypt;
    double minx = xmin;
    double maxx = xmax;
    double miny = ymin;
    double maxy = ymax;
    double pixCorx = (maxx - minx) / m.getWidth();
    double pixCory = (maxy - miny) / m.getHeight();
    
    ax = (xp * pixCorx) + (minx);
    ay = maxy - (yp * pixCory);
    
    for(h = 1 ; h <= iterations + 1 ; h++)
    {
	if( _type == MANDEL_TYPE1 )
	{
	    xnew = ((x * x) - (y * y)) + ax;
	    ynew = (2 * x * y) + ay;
	} // if
	
	if( _type == MANDEL_TYPE2 )
	{
	    xnew = ( ( x * x * x ) - ( 3 * x * y * y ) ) + ax;
	    ynew = ( ( 3 * x * x * y ) - ( y * y * y ) ) + ay;
	} // if 
	
	if( _type == MANDEL_TYPE3 )
	{
	    xnew = ( ( x * x * x * x ) - ( 6 * x * x * y * y ) + ( y * y * y * y ) ) + ax;
	    ynew = ( ( 4 * x * x * x * y ) - ( 4 * x * y * y * y ) ) + ay;
	} // if 
	
	if( _type == MANDEL_TYPE4 )
	{
	    xnew = ( ( x * x * x * x * x ) - ( 10 * x * x * x * y * y ) + ( 5 * x * y * y * y * y ) ) + ax;
	    ynew = ( ( 5 * x * x * x * x * y ) - ( 10 * x * x * y * y * y ) + ( y * y * y * y * y ) ) + ay;
	} // if
	
	if( _type == MANDEL_TYPE5 )
	{
	    xnew = ( ( x * x * x * x * x * x ) - ( 15 * x * x * x * x * y * y ) + (15 * x * x * y * y * y * y ) - ( y * y * y * y * y * y ) ) + ax;
	    ynew = ( ( 6 * x * x * x * x * x * y ) - ( 20 * x * x * x * y * y * y ) + ( 6 * x * y * y * y * y * y ) ) + ay;
	} // if
	
	x = xnew;
	y = ynew;
	
	if(((x * x) + (y * y)) > 4)
	    break;
    }  // for
    
    
    if(h >= iterations + 1 )
    {
	color =0 ;
    } // if
    
    else
    {
	color = h;
    } // else
    
	return color;	

}

int Computation::julia(int xpt , int ypt , double xmin , double xmax , double ymin , double ymax , float conx , float cony)
{  
    MetaData m;
    
    int color;
    
    int h;
    
    double minx = xmin;
    double maxx = xmax;
    double miny = ymin;
    double maxy = ymax;
    
    double pixCorx = (maxx - minx) / m.getWidth();
    double pixCory = (maxy - miny) / m.getHeight();
    
    long double x = xpt * pixCorx + minx;
    long double y = maxy - ypt * pixCory;		//converting from pixels to points
    long double xnew = 0;
    long double ynew=  0;
    
    for(h = 1 ; h <= iterations + 1; h++)
    {
	
	xnew = ((x * x) - (y * y)) + conx;
	ynew = (2 * x * y) + cony;
	x = xnew;
	y = ynew;
	
	if(((x * x) + (y * y)) > 4)
	    break;
    }  // for
      
    
    if(h >= iterations + 1)
    {
	color = 0;
    } // if
    
    else
    {
	color = h;
    } // else

    return color;
    
} // julia()

uint Computation::rgbFromWaveLength(double wave)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    
    if (wave >= 380.0 && wave <= 440.0) 
    {
	r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
	b = 1.0;
    } 
    
    else if (wave >= 440.0 && wave <= 490.0) 
    {
	g = (wave - 440.0) / (490.0 - 440.0);
	b = 1.0;
    } 
    
    else if (wave >= 490.0 && wave <= 510.0) 
    {
	g = 1.0;
	b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
    } 
    
    else if (wave >= 510.0 && wave <= 580.0) 
    {
	r = (wave - 510.0) / (580.0 - 510.0);
	g = 1.0;
    } 
    
    else if (wave >= 580.0 && wave <= 645.0) 
    {
	r = 1.0;
	g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
    } 
    
    else if (wave >= 645.0 && wave <= 780.0) 
    {
	r = 1.0;
    }
    
    double s = 1.0;
    
    if (wave > 700.0)
	s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
    
    else if (wave <  420.0)
	s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);
    
    r = pow(r * s, 0.8);
    g = pow(g * s, 0.8);
    b = pow(b * s, 0.8);
    
    return qRgb(int(r * 255), int(g * 255), int(b * 255));
    
} // rgbFromWaveLength()

