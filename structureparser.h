#ifndef STRUCTUREPARSER_H
#define STRUCTUREPARSER_H

#include <qxml.h>

#include "metadata.h"

class QString;

class StructureParser : public QXmlDefaultHandler
{
public:
    bool startDocument();
    bool characters( const QString & );
    void setValues();
    
    void printValues();
	
    int getType();
    double getMinX();
    double getMaxX();
    double getMinY();
    double getMaxY();
	
private:
    int counter;
    int count;
    double value[ 4 ];
    double theValues[ 15 ];
    int type;
    int theType;
    QString indent;
    QString text;
    QString textValue_str;
    double textValue;
    double minX,maxX,minY,maxY;
};                   

#endif
