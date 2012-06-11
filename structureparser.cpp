#include "structureparser.h"

#include <stdio.h>
#include <qstring.h>

bool StructureParser::startDocument()
{
    count = 0;
    indent = "";
    return TRUE;
}

bool StructureParser::characters(const QString & ch)
{   
    if( count == 1)
    {
	// the second read by the parser is the type
	theType =  atoi( ch );
    }
    else
    {
	counter = count - 1;
	theValues[ counter ] = atof( ch );
    } // else
    
    count++;
    
    return TRUE;
}

void StructureParser::setValues()
{
    type = theType;
    minX = theValues[ 3 ];
    maxX = theValues[ 5 ];
    minY = theValues[ 9 ];
    maxY = theValues[ 11 ];
}

int StructureParser::getType()
{
    return type;
}

double StructureParser::getMinX()
{
    return minX;
}

double StructureParser::getMaxX()
{
    return maxX;
}

double StructureParser::getMinY()
{
    return minY;
}

double StructureParser::getMaxY()
{
    return maxY;
}

void StructureParser::printValues()
{
    int i;
    
    printf( "\nin structure parser, type = %d " , theType );
    printf( "\ntheValues[ %d ] = %f " , 3 , theValues[ 3 ] );
    printf( "\ntheValues[ %d ] = %f " , 5 , theValues[ 5 ] );
    printf( "\ntheValues[ %d ] = %f " , 9 , theValues[ 9 ] );
    printf( "\ntheValues[ %d ] = %f " , 11 , theValues[ 11 ] );
//    for( i = 1 ; i < 5 ; i++)
//	printf( "\ntheValues[ %d ] = %f " , i , theValues[i] );
//    
//    printf( "\n" );    
    
//    printf( "\n\n     mx = %f " , mx);
//    
//    printf( "\n" );    
    
//    printf( "\n minX = %f " , getMinX());
//     
//    printf( "\n" );
}
