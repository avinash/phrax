#include "metadata.h"

const int DefaultType = 0;
const double DefaultMinX = -2.0;
const double DefaultMaxX = 2.0;
const double DefaultMinY = -1.3;
const double DefaultMaxY = 1.3;
const int DefaultHeight = 600;
const int DefaultWidth = 1020;


MetaData::MetaData()
{
	setMetaData( DefaultType , DefaultHeight , DefaultWidth , DefaultMinX , DefaultMaxX , DefaultMinY , DefaultMaxY );
//    setDefaultData();
}

MetaData::~MetaData()
{

}

void MetaData::setDefaultData( int t)
{
    type = t;
    width = DefaultWidth;
    height = DefaultHeight;
    minX = DefaultMinX;
    maxX = DefaultMaxX;
    minY = DefaultMinY;
    maxY = DefaultMaxY;
    
} // setDefaultData

void MetaData::setMetaData(int t , int h , int w , double a , double b , double c , double d )
{
    type = t;
    height = h;
    width = w;
    minX = a;
    maxX = b;
    minY = c;
    maxY = d;
	
}

void MetaData::setHeight( int h )
{
    height = h;
}

void MetaData::setWidth( int w )
{
    width = w;
}

//get the default values 
double MetaData::getDefaultMinX()
{
    return DefaultMinX;
}

double MetaData::getDefaultMaxX()
{
    return DefaultMaxX;
}

double MetaData::getDefaultMinY()
{
    return DefaultMinY;
}

double MetaData::getDefaultMaxY()
{
    return DefaultMaxY;
}

int MetaData::getType()
{
    return type;
}

int MetaData::getWidth()
{
    return width;
}

int MetaData::getHeight()
{
    return height;
}

double MetaData::getMinX()
{
    return minX;
}

double MetaData::getMaxX()
{
    return maxX;

}

double MetaData::getMinY()
{
    return minY;

}

double MetaData::getMaxY()
{
    return maxY;

}
