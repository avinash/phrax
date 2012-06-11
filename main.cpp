#include <qapplication.h>
#include <syslog.h>

#include "fractal.h"
#include "mpi.h"
#include "computation.h"
#include "metadata.h"
#include "mainform.h"

// The values to be taken by the type of fractal selected

const int MANDEL_TYPE1 = 0;
const int MANDEL_TYPE2 = 1;
const int MANDEL_TYPE3 = 2;
const int MANDEL_TYPE4 = 3;
const int MANDEL_TYPE5 = 4;

const int JULIA_TYPE1 = 5;
const int JULIA_TYPE2 = 6;
const int JULIA_TYPE3 = 7;
const int JULIA_TYPE4 = 8;
const int JULIA_TYPE5 = 9;

/*const double JULIA1_CONS_X = -0.74;
const double JULIA1_CONS_Y = 0.1;*/

const double JULIA1_CONS_X = -0.835;
const double JULIA1_CONS_Y = -0.2321;

const double JULIA2_CONS_X = -1.25;
const double JULIA2_CONS_Y = 0;

const double JULIA3_CONS_X = -0.7543;
const double JULIA3_CONS_Y = 0.113;

/*const double JULIA4_CONS_X = -0.122561;
const double JULIA4_CONS_Y =  0.744862;*/

const double JULIA4_CONS_X = -0.70176;
const double JULIA4_CONS_Y =  -0.3842;

/*const double JULIA5_CONS_X = 0.11;
const double JULIA5_CONS_Y = 0.66;*/

const double JULIA5_CONS_X = 0.285;
const double JULIA5_CONS_Y = 0.013;

int main(int argc, char *argv[])
{ 
    int root = 0;
    int my_rank;
    int my_size;
    int oneProcess;
    int startProcess;
    int endProcess;
    int count;
    int i , j;
    int onePixel;
    int theColor;
    int A[ 816000 ];
    int B[ 816000 ];
    int counter = 0;
    int theCounter = 0;
    int TypeDimension[ 4 ];
    int indexSize;
    
    float cons_x;
    float cons_y;
    
    MetaData meta;
    Computation c;
     
    double Coordinates[ 4 ];
    
    enum { ColormapSize = 512 };
    uint colormap[ColormapSize];
    
    int theH = meta.getHeight();
    int theW = meta.getWidth();
		    
    
    /* Start up MPI */
    MPI_Init (&argc, &argv);
    
    /* Find out process rank */
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    
    /* Find out number of processes */
    MPI_Comm_size (MPI_COMM_WORLD, &my_size);
    
    for (int i = 0; i < ColormapSize; ++i)
    {	
	colormap[ i ] = c.rgbFromWaveLength(380.0 + (i * 400.0 / ColormapSize));
    }
    
    oneProcess = theH / my_size;
    startProcess = my_rank * oneProcess;
    endProcess = startProcess + oneProcess;
    
    indexSize = oneProcess * theW;
    
    count = 4;
        
    if( my_rank == root )
    {
	syslog(LOG_DEBUG,"I am master node, THE ROOT");
	
	QApplication app(argc, argv);
	
	Fractal widget;	
	app.setMainWidget( & widget );
	
	widget.image.create( theW , theH , 32 , 512 );
	
	Coordinates[ 0 ] = meta.getMinX();
	Coordinates[ 1 ] = meta.getMaxX();
	Coordinates[ 2 ] = meta.getMinY();
	Coordinates[ 3 ] = meta.getMaxY();
	
	TypeDimension[ 0 ] = meta.getType();
	TypeDimension[ 1 ] = theH;
	TypeDimension[ 2 ] = theW;
	TypeDimension[ 3 ] = 0;		
	
	MPI_Bcast( Coordinates , count , MPI_DOUBLE , root , MPI_COMM_WORLD );
	MPI_Bcast( TypeDimension , 4 , MPI_INT , root , MPI_COMM_WORLD );
	
	// according to the type of the julia fractal, we change the value of the constants for x and y
	
	    if( TypeDimension[ 0 ] == JULIA_TYPE1 )
	    {
		cons_x = JULIA1_CONS_X;
		cons_y = JULIA1_CONS_Y;
	    } // if
	
	    if( TypeDimension[ 0 ] == JULIA_TYPE2 )
	    {
		cons_x = JULIA2_CONS_X;
		cons_y = JULIA2_CONS_Y;
	    } // if

	    
	    if( TypeDimension[ 0 ] == JULIA_TYPE3 )
	    {
		cons_x = JULIA3_CONS_X;
		cons_y = JULIA3_CONS_Y;
	    } // if
	    
	    if( TypeDimension[ 0 ] == JULIA_TYPE4 )
	    {
		cons_x = JULIA4_CONS_X;
		cons_y = JULIA4_CONS_Y;
	    } // if
	    
	    if( TypeDimension[ 0 ] == JULIA_TYPE5)
	    {
		cons_x = JULIA5_CONS_X;
		cons_y = JULIA5_CONS_Y;
	    } // if 
	
	if( TypeDimension[ 0 ] == MANDEL_TYPE1 || TypeDimension[ 0 ] == MANDEL_TYPE2 || TypeDimension[ 0 ] == MANDEL_TYPE3 || TypeDimension[ 0 ] == MANDEL_TYPE4 || TypeDimension[ 0 ] == MANDEL_TYPE5 )
	{
	    for(j = startProcess ; j < endProcess ; j++)    
	    {
		//	only pixels of the image height will be distributed to the
		//	processors
		for( i = 0;i < theW ; i++)
		{
		    
		    onePixel = c.mandel( i , j , Coordinates[ 0 ] , Coordinates[ 1 ] , Coordinates[ 2 ] , Coordinates[ 3 ] , TypeDimension[ 0 ] );
		    
		    A[ counter ] = onePixel;
		    
		    counter++;
		    
		} // for i
	    } // for j
	}
	
	if( TypeDimension[ 0 ] == JULIA_TYPE1 || TypeDimension[ 0 ] == JULIA_TYPE2 || TypeDimension[ 0 ] == JULIA_TYPE3 || TypeDimension[ 0 ] == JULIA_TYPE4 || TypeDimension[ 0 ] == JULIA_TYPE5 )
	{
    
	    for(j = startProcess ; j < endProcess ; j++)    
	    {
		//	only pixels of the image height will be distributed to the
		//	processors
		for( i = 0;i < theW ; i++)
		{
		    
		    onePixel = c.julia( i , j , Coordinates[ 0 ] , Coordinates[ 1 ] , Coordinates[ 2 ] , Coordinates[ 3 ] , cons_x , cons_y );
		    
		    A[ counter ] = onePixel;
		    
		    counter++;
		    
		} // for i
	    } // for j
	}
	
	MPI_Gather( A , indexSize , MPI_INT , B , indexSize , MPI_INT, root , MPI_COMM_WORLD );   

	counter = 0;

	for( j = 0 ; j < theH ; j++ )
	{
	    for( i = 0 ; i < theW ; i++ )
	    {
		theColor = B[ theCounter ];
		
		if( theColor == 0 )
		{
			widget.image.setPixel( i , j , 0 );
		} // if
		
		else
		{
			widget.image.setPixel( i , j , colormap[theColor]);		
		} // else
		
		theCounter++;
		
	    } // for i
	} // for j
	
	widget.pixmap.convertFromImage(widget.image,0);
	
	widget.update();
	
	widget.show();
	
		
	return app.exec();
	
	
    } // if
    
    else
    {
	while( TRUE )
	{		
	    counter=0;
	       
	    MPI_Bcast( Coordinates , count , MPI_DOUBLE , root , MPI_COMM_WORLD );
	    MPI_Bcast( TypeDimension , 4 , MPI_INT , root , MPI_COMM_WORLD );
	    	    
	    if( TypeDimension[ 3 ] == 0 )
	    {
		syslog(LOG_DEBUG,"I am the slave node. My Top-Left Coordinate is ( %f , %f )" , Coordinates[0],Coordinates[2]);
		
		if( TypeDimension[ 0 ] == JULIA_TYPE1 )
		{
			cons_x = JULIA1_CONS_X;
			cons_y = JULIA1_CONS_Y;
		} // if
		
		if( TypeDimension[ 0 ] == JULIA_TYPE2 )
		{
			cons_x = JULIA2_CONS_X;
			cons_y = JULIA2_CONS_Y;
		} // if
		
		if( TypeDimension[ 0 ] == JULIA_TYPE3 )
		{
			cons_x = JULIA3_CONS_X;
			cons_y = JULIA3_CONS_Y;
		} // if
		
		if( TypeDimension[ 0 ] == JULIA_TYPE4 )
		{
			cons_x = JULIA4_CONS_X;
			cons_y = JULIA4_CONS_Y;
		} // if
		
		if( TypeDimension[ 0 ] == JULIA_TYPE5)
		{
			cons_x = JULIA5_CONS_X;
			cons_y = JULIA5_CONS_Y;
		} // if 
		
		if( TypeDimension[ 0 ] == MANDEL_TYPE1 || TypeDimension[ 0 ] == MANDEL_TYPE2 || TypeDimension[ 0 ] == MANDEL_TYPE3 || TypeDimension[ 0 ] == MANDEL_TYPE4 || TypeDimension[ 0 ] == MANDEL_TYPE5 )
		{
			for(j = startProcess ; j < endProcess ; j++)    
			{
			//	only pixels of the image height will be distributed to the
			//	processors
				for( i = 0; i < theW ; i++)
				{
					
					onePixel = c.mandel( i , j , Coordinates[ 0 ] , Coordinates[ 1 ] , Coordinates[ 2 ] , Coordinates[ 3 ] , TypeDimension[ 0 ] );
					
					A[ counter ] = onePixel;
					
					counter++;
					
				} // for i
			} // for j
		} // if
		
		if( TypeDimension[ 0 ] == JULIA_TYPE1 || TypeDimension[ 0 ] == JULIA_TYPE2 || TypeDimension[ 0 ] == JULIA_TYPE3 || TypeDimension[ 0 ] == JULIA_TYPE4 || TypeDimension[ 0 ] == JULIA_TYPE5 )
		{
			
			for(j = startProcess ; j < endProcess ; j++)    
			{
			//	only pixels of the image height will be distributed to the
			//	processors
				for( i = 0;i < theW ; i++)
				{
				
				onePixel = c.julia( i , j , Coordinates[ 0 ] , Coordinates[ 1 ] , Coordinates[ 2 ] , Coordinates[ 3 ] , cons_x , cons_y);
				
				A[ counter ] = onePixel;
				
				counter++;
				
				} // for i
			} // for j
		} // if // julia 1
		
		MPI_Gather( A , indexSize , MPI_INT , B , indexSize , MPI_INT, root , MPI_COMM_WORLD );
	    }//if TypeDimension 3
	    
	    else
	    {
		    break;
	    }
	
	} // while()
    } // else
    
    syslog(LOG_DEBUG,"Closing Phr@X from slave");
       
    MPI_Finalize();
    
    return 0;
}//main
