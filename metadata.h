#ifndef METADATA_H
#define METADATA_H

class MetaData
{
private:
    int type;
    int height;
    int width;
    double minX;
    double maxX;
    double minY;
    double maxY;	

public:	
    MetaData();
    ~MetaData();
    
    void setDefaultData( int );
    void setMetaData( int , int , int , double , double , double , double );
    void setHeight( int );
    void setWidth( int );
    
    double getDefaultMinX();
    double getDefaultMaxX();
    double getDefaultMinY();
    double getDefaultMaxY();
    
    int getType();
    int getWidth();
    int getHeight();
    double getMinX();
    double getMaxX();
    double getMinY();
    double getMaxY();
			
};

#endif
