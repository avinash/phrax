#include <math.h>
#include <syslog.h>

#include <qpainter.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qcstring.h>
#include <qtextbrowser.h>
#include <qwhatsthis.h>

#include "fractal.h"
#include "mpi.h"
#include "helpwindow.h"
#include "computation.h"

const double zoomFactor = 0.25;

//the following values specify the minimum size of the image
//values smaller than these values will cause the image size to be too small	
const int minImageWidth = 100;
const int minImageHeight = 100;

//the following values specify the maximum size of the image
//if the values fall beyond these range, then only part of the image will be displayed
const int maxImageWidth = 1016;
const int maxImageHeight = 605;

//the width, height, depth and number of color of the image
const int DefaultImageWidth = 500;
const int DefaultImageHeight = 500; 
const int imageDepth = 32;
const int imageNumOfColors = 512;

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
int indexSize;
int check = 0;


Fractal::Fractal(QWidget *parent , const char *name , int wFlags)
        : MainForm(parent,name,wFlags)
{       
    Computation c;
    
    // put the "What's This" button on the Help menu
    Help->insertItem( "What's &This", this, SLOT(whatsThis()), SHIFT+Key_F1 );

    // puts the "What's This" icon on the toolbar
    QWhatsThis::whatsThisButton( fileToolBar );
    
    // Find out process rank 
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    
    // Find out number of processes 
    MPI_Comm_size (MPI_COMM_WORLD, &my_size);
    
    setImageWidth( DefaultImageWidth );
    setImageHeight( DefaultImageHeight );
        
    filename = QString::null;
    m_filename = QString::null;
  
//    get the height of the menu bar and the tool bar
    menuToolHeight = MenuBar -> height() + fileToolBar -> height();
    
//    get the height of the status bar
//    statusHeight = statusBar() -> height();
       
//    the following values are the top-left coordnate of the screen where the fractal
//    is going to be displayed
    topLeftX = 1;
    topLeftY = menuToolHeight + 4;
    
//    (x,y) is the top - left coordinate of the image to be displayed.
    x = topLeftX; 
    y = topLeftY; 
    
//    (sx, sy) specifies the top-left point in pixmap that is to be drawn
//    (0,0) is the default 
    sx = 0;
    sy = 0;
    
    
//    (sw, sh) specifies the size of the pixmap that is to be drawn
//    The default, (-1, -1), means all the way to the bottom right of the pixmap
    sw = -1;
    sh = -1;
        
    for (int i = 0; i < ColormapSize; ++i)
    {	
	colormap[i] = c.rgbFromWaveLength(380.0 + (i * 400.0 / ColormapSize));
    }
    
//    getting the default values for minimum and maximum x and y
    
    typeT = data.getType();		// the default fractal, mandelbrot or julia
    minimumX = data.getMinX(); 	// Leftmost Real point
    maximumX = data.getMaxX(); 	// Rightmost Real point of plane to be displayed
    minimumY = data.getMinY();	// Lowermost Imaginary point
    maximumY = data.getMaxY();	// Uppermost Imaginary point   
        
    resize(data.getWidth() , data.getHeight() + 87 );
               
    // Create a QLabel to display normal & permanent messages
    status = new QLabel( statusBar() );
    status->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
    status->setFixedHeight( fontMetrics().height() + 4 );
    status->setFixedWidth( width() );
    
    theMessage.sprintf( "Number of nodes = %d " , my_size );
    
    status->setText( theMessage );
       
    saveCount = 0;
    
    QString defaultFileName = "Untitled.xml";
    QString titleBar = QString( "Phr@X - A Parallel Fractal Explorer - [ %1 ]" ).arg( defaultFileName );
    setCaption( titleBar );

} // constructor

/******************************************/

void Fractal::updateStatus( QString moremsg )
{    
    status->clear();
    
    theMessage.sprintf("Number of nodes = %d " , my_size );
       
    theMessage += moremsg;
  
    status->setText( theMessage );
    
} // updateStatus( QString moremsg )

/******************************************/

void Fractal::setImageWidth( int w )
{
    imageWidth = w;
} // setImageWidth()

/******************************************/

void Fractal::setImageHeight( int h )
{
    imageHeight = h;
} // setImageWidth()

/******************************************/

int Fractal::getImageWidth()
{
    return imageWidth;
} // getImageWidth

/******************************************/

int Fractal::getImageHeight()
{
    return imageHeight;
} // getImageWidth

/******************************************/

void Fractal::resizeEvent( QResizeEvent * e )
{
    /*setImageWidth( width() );
    setImageHeight( height() - 80 );
    render( data.getType() , data.getMinX() , data.getMaxX() , data.getMinY() , data.getMaxY() );*/
    
} // resizeEvent()

/******************************************/

void Fractal::save( QString f )
{
    int theType;
    double minx,maxx,miny,maxy;
    int i;
    
    theType = data.getType();       
    minx = data.getMinX();
    maxx = data.getMaxX();
    miny = data.getMinY();
    maxy = data.getMaxY();
    check = 0;	
	
    
    FILE *fpt;
	
    fpt = fopen( f , "w" );
  
    fprintf(fpt,"<fractal>");
    fprintf(fpt,"\n	<type>%d</type>", theType );
    fprintf(fpt,"\n	<x>");
    fprintf(fpt,"\n		<minx>%f</minx>",minx);
    fprintf(fpt,"\n		<maxx>%f</maxx>",maxx);
    fprintf(fpt,"\n	</x>");
    fprintf(fpt,"\n	<y>");
    fprintf(fpt,"\n		<miny>%f</miny>",miny);
    fprintf(fpt,"\n		<maxy>%f</maxy>",maxy);
    fprintf(fpt,"\n	</y>");
    fprintf(fpt,"\n</fractal>");
    
    
    fclose(fpt);
} // save()

/******************************************/

void Fractal::fileSave()
{ 
    updateStatus("");
    
    if( saveCount == 0 )
    {
    
	QString fileName = QFileDialog::getSaveFileName(
                            QString::null, "(*.xml)", this,
                            "file save as", "Fractal -- File Save" );
    
	
	if ( !fileName.isEmpty() ) 
	{
	    fileName.append(".xml");
	    
	    
//	    the caption that appears on the title bar
//	    fileName also includes the path of the file
	    QString titleBar = QString( "A Parallel Fractal Explorer - [ %1 ]" ).arg( fileName );
	    
	    int answer = 0;
	    if ( QFile::exists( fileName ) )
		answer = QMessageBox::warning(
			this, "Fractal -- Overwrite File",
			QString( "Overwrite\n\'%1\'?" ).
			arg( fileName ),
			"&Yes", "&No", QString::null, 1, -1 );
	    if ( answer == 0 ) 
	    {
		m_filename = fileName;
		//	    updateRecentFiles( filename );
		save( m_filename );
		setCaption( titleBar );
		saveCount++;
		return;
	    } // if
	} // if
	
//	statusBar()->message( "Saving abandoned", 2000 );
	updateStatus( "	|	Saving abandonned" );
    }
    
    else
    {
	save( m_filename );
//	updateStatus("Save");
	

    }
   
    

    
} // save()

/******************************************/

void Fractal::fileSaveAs()
{
    updateStatus("");
    
    QString fileName = QFileDialog::getSaveFileName(
                            QString::null, "(*.xml)", this,
                            "file save as", "Fractal -- File Save As" );
    
//	    the caption that appears on the title bar
//	    fileName also includes the path of the file
	    QString titleBar = QString( "A Parallel Fractal Explorer - [ %1 ]" ).arg( fileName );

    if ( !fileName.isEmpty() ) 
    {
	fileName.append(".xml");
	int answer = 0;
	if ( QFile::exists( fileName ) )
	    answer = QMessageBox::warning(
                            this, "Fractal -- Overwrite File",
                            QString( "Overwrite\n\'%1\'?" ).
                                arg( fileName ),
                            "&Yes", "&No", QString::null, 1, -1 );
	if ( answer == 0 ) 
	{
	    m_filename = fileName;
//	    updateRecentFiles( filename );
	    save( m_filename );
	    setCaption( titleBar );
	    return;
	} // if
    } // if
    
//    statusBar()->message( "Saving abandoned", 2000 );
    updateStatus("	|	Saving abandonned");
   
} // fileSaveAs()

/******************************************/

void Fractal::fileOpen()
{
    updateStatus("");
    
    // displays only XML files in the dialog box
    QString fn = QFileDialog::getOpenFileName( QString::null, "XML Files (*.xml)", this );
      
    if ( !fn.isEmpty() )
	load( fn );
    else
	updateStatus("	|	Loading aborted");
    
//    update();

} // fileOpen()

/******************************************/

void Fractal::load( QString fileName)
{
    int t,h,w;
    double miniX,maxiX,miniY,maxiY;
          
    QFile xmlFile( fileName );
    QXmlInputSource source( &xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.parse( source );
    
//    set the minimum x & y and maximum x & y according to the values in the
//    xml file
    handler.setValues();
    
//    once the values are set we get the values from the class
    t = handler.getType();
    
    // if the type isn't between 0 and 9 inclusive (suppose the user tried changing the parameter)
    // the application will load the default fractaal
    if( ( t != 0 ) && ( t != 1 ) && ( t != 2 ) && ( t != 3 ) && ( t != 4 ) && ( t != 5 ) && ( t != 6 ) && ( t != 7 ) && ( t != 8 ) && ( t != 9 ) )
    {
	static QMessageBox* error = new QMessageBox( "Error", "File has been modified with wrong parameters. \n" "The default fractal will be loaded instead." , QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
	
	error->setButtonText( 1, "Ok" );
	error->show();
	
	data.setDefaultData( MANDEL_TYPE1 );
	
	mandel1Action->setOn( TRUE );
	
	update();
	
	render( data.getType() , data.getHeight() , data.getWidth() , data.getDefaultMinX() , data.getDefaultMaxX() , data.getDefaultMinY() , data.getDefaultMaxY() );
    } // if
    
    else
    {
	h = data.getHeight();
	w = data.getWidth();    
	miniX = handler.getMinX();
	maxiX = handler.getMaxX();
	miniY = handler.getMinY();
	maxiY = handler.getMaxY();
	
	
	//    according to the image we load (i.e. whether the image is mandelbrot or julia)
	//    we check the appropriate action in Fractal -> Select Fractal
	
	if( t == MANDEL_TYPE1 )
	    mandel1Action->setOn( TRUE );
	
	if( t == MANDEL_TYPE2)
	    mandel2Action->setOn( TRUE );
	
	if( t == MANDEL_TYPE3)
	    mandel3Action->setOn( TRUE );
	
	if( t == MANDEL_TYPE4)
	    mandel4Action->setOn( TRUE );
	
	if( t == MANDEL_TYPE5)
	    mandel5Action->setOn( TRUE );
	
	if( t == JULIA_TYPE1)
	    julia1Action->setOn( TRUE );
	
	if( t == JULIA_TYPE2 )
	    julia2Action->setOn( TRUE );
	
	if( t == JULIA_TYPE3 )
	    julia3Action->setOn( TRUE );
	
	if( t == JULIA_TYPE4 )
	    julia4Action->setOn( TRUE );
	
	if( t == JULIA_TYPE5 )
	    julia5Action->setOn( TRUE );
	
	//    now we set the data in the metadata class (the class which stores the minimum 
	//    and maximum values of x & y)   				       
	data.setMetaData( t , h , w , miniX , maxiX , miniY , maxiY);
	
	update();	
	
	render( t , h , w , miniX , maxiX , miniY , maxiY );       
    } // else
    
    check = 0;
	
} // load()

/******************************************/

void Fractal::fileSaveJPEG()
{
    updateStatus("");
    
    const char* fmt = "JPEG";
    QString savefilename = QFileDialog::getSaveFileName(QString::null, "JPEG Image (.jpeg)",
					this, filename);
    
    savefilename.append( ".jpeg" );
    
    // saving a JPEG image with 75 % quality level
    image.save( savefilename , "JPEG" , 75 );
    
    if ( !savefilename.isEmpty() )
	if ( !pixmap.save( savefilename, fmt ) )
	    QMessageBox::warning( this, "Save failed", "Error saving file" ); 
    
} // savePNG() 

/******************************************/

void Fractal::fileSavePNG()
{
    updateStatus("");
    
    const char* fmt = "PNG";
    QString savefilename = QFileDialog::getSaveFileName(QString::null, "PNG Image (.png)",
					this, filename);
    savefilename.append( ".png" );
    
    // saving a PNG image with the default quality level i.e. 100 %
    image.save( savefilename , "PNG" , -1 );
    
    if ( !savefilename.isEmpty() )
	if ( !pixmap.save( savefilename, fmt ) )
	    QMessageBox::warning( this, "Save failed", "Error saving file" ); 
    
} // savePNG() 

/********************************************************************************************/

void Fractal::closeEvent( QCloseEvent * ce )
{
/*    QApplication::exit(0);
    
	MPI_Finalize();*/
	double Coordinates[ 4 ];
	int TypeDimension[ 4 ];
	int root = 0;
	int count = 4;
	
	Coordinates[ 0 ] = Coordinates[ 1 ] = Coordinates[ 2 ] = Coordinates[ 3 ] = 0.1;
	TypeDimension[ 0 ] = TypeDimension[ 1 ] = TypeDimension[ 2 ] = TypeDimension[ 3 ] = 1;
	
	if(check == 1)
	{
	    
/*	    static QMessageBox* error = new QMessageBox( "Error", "File has been modified with wrong parameters. \n" "The default fractal will be loaded instead." , QMessageBox::Information, 1, 0, 0, this, 0, FALSE );*/
	
	    switch( QMessageBox::information( this, "Save Changes",		// caption of message box
			"Do you want to save the changes before exiting?",	// text 
					"&Save", "&Discard", "Cancel",	// the 3 buttons
			0, 2 ) ) 						// 7th parameter is the default button. Here it is 0 i.e. the  first button 									
									// 8th parameter is the escape button. Here 2 means "Cancel" 
	    
		{
			case 0:
				fileSave();
				ce->accept();
				MPI_Bcast( Coordinates , count , MPI_DOUBLE , root , MPI_COMM_WORLD );
				MPI_Bcast( TypeDimension , 4 , MPI_INT , root , MPI_COMM_WORLD );
				break;
			case 1:
				ce->accept();
				MPI_Bcast( Coordinates , count , MPI_DOUBLE , root , MPI_COMM_WORLD );
				MPI_Bcast( TypeDimension , 4 , MPI_INT , root , MPI_COMM_WORLD );
				break;
			case 2:
				default: // just for sanity
					ce->ignore();
					break;
		} // switch
	}
	
	else
	{
		ce->accept();
		MPI_Bcast( Coordinates , count , MPI_DOUBLE , root , MPI_COMM_WORLD );
		MPI_Bcast( TypeDimension , 4 , MPI_INT , root , MPI_COMM_WORLD );
		

	}
	
	syslog(LOG_DEBUG,"Closing Phr@X from master");
    
} // closeEvent( QCloseEvent * )


/********************************************************************************************/

void Fractal::selectType( QAction * action )
{
    updateStatus("");
    
    int theType = 0;

	check = 0; 
    
    if( action == mandel1Action )
    {
	theType = MANDEL_TYPE1;
	mandel1Action->setOn( TRUE );
    } // if
    
    if( action == mandel2Action )
    {
	theType = MANDEL_TYPE2;
	mandel2Action->setOn( TRUE );
    } // if
    
    if( action == mandel3Action )
    {
	theType = MANDEL_TYPE3;
	mandel3Action->setOn( TRUE );
    } // if
    
    if( action == mandel4Action )
    {
	theType = MANDEL_TYPE4;
	mandel4Action->setOn( TRUE );
    } // if
	
    if( action == mandel5Action )
    {
	theType = MANDEL_TYPE5;
	mandel5Action->setOn( TRUE );
    } // if
    
    if( action ==julia1Action )
    {
	theType = JULIA_TYPE1;
	julia1Action->setOn( TRUE );

    } // if
    
    if( action ==julia2Action )
    {
	theType = JULIA_TYPE2;
	julia2Action->setOn( TRUE );
    } // if
    
    if( action ==julia3Action )
    {
	theType = JULIA_TYPE3;
	julia3Action->setOn( TRUE );
    } // if
	
    if( action ==julia4Action )
    {
	theType = JULIA_TYPE4;
	julia4Action->setOn( TRUE );
    } // if
	    
    if( action ==julia5Action )
    {
	theType = JULIA_TYPE5;
	julia5Action->setOn( TRUE );
    } // if
    
    data.setDefaultData( theType );
        
    render( data.getType() , data.getHeight() , data.getWidth() , data.getMinX() , data.getMaxX() , data.getMinY() , data.getMaxY() );
               
    update();
 	
} // selectType()

/******************************************/
/*
  * At any point in time if the user wants to go back to the initial fractal,
  *he just have to click the reset button
*/
void Fractal::fractalReset()
{
    check = 0;
	
    updateStatus("");
    
    if( data.getType() == MANDEL_TYPE1 )			// if the fractal being zoomed is the mandelbrot set
    {
	data.setDefaultData( MANDEL_TYPE1 );		// set the type to mandelbrot
    } // if
    
    if( data.getType() == MANDEL_TYPE2 )
    {
	data.setDefaultData( MANDEL_TYPE2 );
    } // if
    
    if( data.getType() == MANDEL_TYPE3 )
    {
	data.setDefaultData( MANDEL_TYPE3 );
    } // if
    
    if( data.getType() == MANDEL_TYPE4 )
    {
	data.setDefaultData( MANDEL_TYPE4 );
    } // if
       
    if( data.getType() == MANDEL_TYPE5 )
    {
	data.setDefaultData( MANDEL_TYPE5 );
    } // if
    
    if( data.getType() == JULIA_TYPE1 )			// if  julia set
    {
	data.setDefaultData( JULIA_TYPE1 );
    } // if
    
    if( data.getType() == JULIA_TYPE2 )			// if  julia set
    {
	data.setDefaultData( JULIA_TYPE2 );
    } // if
    
    if( data.getType() == JULIA_TYPE3 )			// if  julia set
    {
	data.setDefaultData( JULIA_TYPE3 );
    } // if
	
    if( data.getType() == JULIA_TYPE4 )			// if  julia set
    {
	data.setDefaultData( JULIA_TYPE4 );
    } // if
	    
    if( data.getType() == JULIA_TYPE5 )			// if  julia set
    {
	data.setDefaultData( JULIA_TYPE5 );
    } // if
    
//    printf( "\ndefault minimum x = %f " , data.getDefaultMinX() );
    
    // render the fractal with all the initial values
    render( data.getType() , data.getHeight() , data.getWidth() , data.getDefaultMinX() , data.getDefaultMaxX() , data.getDefaultMinY() , data.getDefaultMaxY() );
           
    update();

} // fractalReset()

/******************************************/

void Fractal::quickHelp()
{
    updateStatus("");
       
    static QMessageBox* about = new QMessageBox( "Phr@X Quick Help",
						 "<h3>How To Use Phr@X</h3>"
						 "<ul>"
						 "<li> To Zoom In, Left Click."
						 "<li> To Zoom Out, Right Click."
						 "</ul>", QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
    about->setButtonText( 1, "Ok" );
    about->show();
    
} // quickHelp()

/******************************************/

void Fractal::about()
{
    updateStatus("");
    
    static QMessageBox* about = new QMessageBox( "Phr@X",
						 "<h3>About</h3>"
						 "<p>Copyright (C) 2005-2006 Imtiaz Ellaheebuksh and another student.<\p>"
						 "<p><b><i>Phr@X</i></b> is licensed to you under the terms of the GNU General Public License. See http://www.gnu.org/.</p>"
						 , QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
    
    about->setButtonText( 1, "Ok" );
    about->show();
	
}  // about()

/******************************************/

/*
  * This function displays a help page that tells the user how to use Phr@X, and @ the same time it
  * gives the new users an introduction to fractals
  */

void Fractal::aboutPhrax()
{
    updateStatus("");
        
    static QMessageBox* about = new QMessageBox( "Phr@X",
						 "<h3>About Phr@X</h3>"
						"<p><b><i>Phr@X</i></b> is a free software to explore fractals using parallel processing.</p><p><b><i>Phr@X</i></b> has been developed using the Qt Toolkit. See http://www.trolltech.com for more information on Qt.</p>"
						 , QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
    about->setButtonText( 1, "Ok" );
    about->show();
    
} // aboutPhrax()

/******************************************/

void Fractal::helpContents()
{
    updateStatus("");
    
    QString home = QDir( "./PhraX-Help/home.htm" ).absPath();       
//    QString home = QDir( "/usr/share/doc/doxygen-1.4.2/html/index.html" ).absPath();       

    HelpWindow *help = new HelpWindow(home, ".", 0, "help viewer");
    
    help->showMaximized();
    
} // helpContents()

/******************************************/

void Fractal::paintEvent(QPaintEvent * e)
{   
    //    provides a 2D drawable surface
    QPainter painter( this );  

    //    paint the rectangle black
    painter.fillRect( rect() , Qt::black );    

       
    painter.drawPixmap( x , y , pixmap , sx , sy , sw , sh );

}

/******************************************/


void Fractal::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {	
//	get the mouse position
	lastDragPosx = event->x();
	lastDragPosy = event->y();

	check = 1;	   
	   
	int typeT = data.getType();
	double xxmin = data.getMinX();
	double xxmax = data.getMaxX();
	double yymin = data.getMinY();
	double yymax = data.getMaxY();
	
	double rangeX;
	double rangeY;
	double scaleX;
	double scaleY;
	double distanceMaxx;
	double distanceMaxy;
	double maxSectionX;
	double maxSectionY;
	double minSectionX;
	double minSectionY;
	
	
//	specify the distance between minimum x and maximum x
	rangeX = xxmax - xxmin;
//	specify the distance between minimum y and maximum y
	rangeY = yymax - yymin;
	
//	converting the screen coordinate (in pixels) to plane coordinate
	double Zx = ( rangeX / data.getWidth() ) * lastDragPosx + xxmin;
	double Zy = yymax - ( rangeY / data.getHeight() ) * lastDragPosy;
	
//	decrease the range by the zoom factor
	scaleX = zoomFactor * rangeX;
	scaleY = zoomFactor * rangeY;
	
//	distance between the mouse position (in plane coordinate) and maximum x
	distanceMaxx = xxmax - Zx;
//	distance between the mouse position (in plane coordinate) and maximum y
	distanceMaxy = yymax - Zy;
	
	maxSectionX = ( distanceMaxx / rangeX ) * scaleX;
	maxSectionY = ( distanceMaxy / rangeY ) * scaleY;
	
	minSectionX = scaleX - maxSectionX;
	minSectionY = scaleY - maxSectionY;
	
	
	xxmin = xxmin + minSectionX;
	xxmax = xxmax - maxSectionX;	
	
	yymin = yymin + minSectionY;
	yymax = yymax - maxSectionY;
	
	int h = data.getHeight();
	int w = data.getWidth();
	
	data.setMetaData( typeT , h , w , xxmin,xxmax,yymin,yymax);
	
	render(typeT , h , w , xxmin,xxmax,yymin,yymax);
	
	updateStatus("");

/*	
  
Plane view
		
	
           xxmin	                                      xxmax
	---------------------------------------------------------
	|							|	
	|<------- Zx ---------->  <---- distanceMaxx ---------->|	
	|-----------------------/\------------------------------|
	|		  	|				|
	|		 	|				|
	|	     	mouse position				|
	|							|
	|							|
	|							|
	---------------------------------------------------------
	<----------------------- rangeX------------------------->
		
	      
yymin	-----------------------------------------	/\
 	|       /\				|	|
	|	|				|	|
	|       Zy				|	|
	|	|				|	|
	|       \/				|	|
	|	<------  mouse position 	|     rangeY
	|       /\	 			|	|
	|	|   				|	|
	|	|				|	|
	|      distanceMaxY			|	|
	|	|				|	|
	|	|				|	|
	|       \/				|       \/
yymax	-----------------------------------------	
	
*/	
	

    } // if zoom in
    
    
    
    if (event->button() == Qt::RightButton)
    {	
//	get the mouse position
	lastDragPosx = event->x();
	lastDragPosy = event->y();
	check = 1;
	
	int typeT = data.getType();
	double xxmin = data.getMinX();
	double xxmax = data.getMaxX();
	double yymin = data.getMinY();
	double yymax = data.getMaxY();
	
	double rangeX;
	double rangeY;
	double scaleX;
	double scaleY;
	double distanceMaxx;
	double distanceMaxy;
	double maxSectionX;
	double maxSectionY;
	double minSectionX;
	double minSectionY;
	
	
//	specify the distance between minimum x and maximum x
	rangeX = xxmax - xxmin;
//	specify the distance between minimum y and maximum y
	rangeY = yymax - yymin;
	
//	converting the screen coordinate (in pixels) to plane coordinate
	double Zx = ( rangeX / data.getWidth() ) * lastDragPosx + xxmin;
	double Zy = yymax - ( rangeY / data.getHeight() ) * lastDragPosy;
	
//	decrease the range by the zoom factor
	scaleX = zoomFactor * rangeX;
	scaleY = zoomFactor * rangeY;
	
//	distance between the mouse position (in plane coordinate) and maximum x
	distanceMaxx = xxmax - Zx;
//	distance between the mouse position (in plane coordinate) and maximum y
	distanceMaxy = yymax - Zy;
	
	maxSectionX = ( distanceMaxx / rangeX ) * scaleX;
	maxSectionY = ( distanceMaxy / rangeY ) * scaleY;
	
	minSectionX = scaleX - maxSectionX;
	minSectionY = scaleY - maxSectionY;
	
	xxmin = xxmin - minSectionX;
	xxmax = xxmax + maxSectionX;
	
	yymin = yymin - minSectionY;
	yymax = yymax + maxSectionY;
	
		int h = data.getHeight();
	int w = data.getWidth();
	
	data.setMetaData( typeT , h , w , xxmin,xxmax,yymin,yymax);
	
	render(typeT , h , w , xxmin,xxmax,yymin,yymax);
	
	updateStatus("");
    } // if zoom out
      
} // mousePressEvent()	

/******************************************/
/*
uint Fractal::rgbFromWaveLength(double wave)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    if (wave >= 380.0 && wave <= 440.0) {
        r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
        b = 1.0;
    } else if (wave >= 440.0 && wave <= 490.0) {
        g = (wave - 440.0) / (490.0 - 440.0);
        b = 1.0;
    } else if (wave >= 490.0 && wave <= 510.0) {
        g = 1.0;
        b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
    } else if (wave >= 510.0 && wave <= 580.0) {
        r = (wave - 510.0) / (580.0 - 510.0);
        g = 1.0;
    } else if (wave >= 580.0 && wave <= 645.0) {
        r = 1.0;
        g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
    } else if (wave >= 645.0 && wave <= 780.0) {
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
} // rgbFromWaveLength()*/

/******************************************/

void Fractal::render( int type , int h , int w , double minX,double maxX,double minY,double maxY)
{
    Computation c;
    int i,j;
    
    oneProcess = h / my_size;				// number of pixels (row-wise) calculated by one processor
    startProcess = my_rank * oneProcess;		// where the process starts calculating
    endProcess = startProcess + oneProcess;		// where the process ends calculating	
    
    indexSize = oneProcess * w;			
     
    double Coordinates[ 4 ];
    int TypeDimension[ 4 ];
    int root = 0;
    int count = 4;
    		    
    
    Coordinates[ 0 ] = minX;
    Coordinates[ 1 ] = maxX;
    Coordinates[ 2 ] = minY;
    Coordinates[ 3 ] = maxY;
    
    TypeDimension[ 0 ] = type;			// the type of the fractal
    TypeDimension[ 1 ] = h;				// height of image
    TypeDimension[ 2 ] = w;				// width of image
    TypeDimension[ 3 ] = 0;

    
    counter = 0;
    theCounter = 0;
    
    
    MPI_Bcast( Coordinates , count , MPI_DOUBLE , root , MPI_COMM_WORLD );
    MPI_Bcast( TypeDimension , 4 , MPI_INT , root , MPI_COMM_WORLD );

	syslog(LOG_DEBUG,"I am the master node. My Coordinates are ( %f , %f )" , Coordinates[0] , Coordinates[2]);
        
    if( TypeDimension[0] == JULIA_TYPE1 )
    {
	constant_x = JULIA1_CONS_X;
	constant_y = JULIA1_CONS_Y;
    }
    
   
    if( TypeDimension[0] == JULIA_TYPE2 )
    {
	constant_x = JULIA2_CONS_X;
	constant_y = JULIA2_CONS_Y;
    }
    
    if( TypeDimension[ 0 ] == JULIA_TYPE3 )
    {
	 constant_x = JULIA3_CONS_X;
	 constant_y = JULIA3_CONS_Y;
    }
    
    if( TypeDimension[ 0 ] == JULIA_TYPE4 )
    {
	constant_x = JULIA4_CONS_X;
	constant_y = JULIA4_CONS_Y;
    }
    
    if( TypeDimension[ 0 ] == JULIA_TYPE5 )
    {
	constant_x = JULIA5_CONS_X;
	constant_y = JULIA5_CONS_Y;
    }
	
    
    if( type == MANDEL_TYPE1 || type == MANDEL_TYPE2 || type == MANDEL_TYPE3 || type == MANDEL_TYPE4 || type == MANDEL_TYPE5 )
    {
	for(j = startProcess ; j < endProcess ; j++)    
	{
	    //	only pixels of the image height will be distributed to the
	    //	processors
		// i is for the width
	    for( i = 0;i < w ; i++)
	    {
		
		onePixel = c.mandel( i , j , Coordinates[ 0 ] , Coordinates[ 1 ] , Coordinates[ 2 ] , Coordinates[ 3 ] , TypeDimension[ 0 ] );
		
		A[ counter ] = onePixel;
		
		counter++;
		
	    } // for i
	} // for j
    } // if mandelbrot set	
    
    if( type == JULIA_TYPE1 || type == JULIA_TYPE2 || type == JULIA_TYPE3 || type == JULIA_TYPE4 || type == JULIA_TYPE5 )
    {

	
	for(j = startProcess ; j < endProcess ; j++)    
	{
	    //	only pixels of the image height will be distributed to the
	    //	processors
	    for( i = 0;i < w ; i++)
	    {
		
		onePixel = c.julia( i , j , Coordinates[ 0 ] , Coordinates[ 1 ] , Coordinates[ 2 ] , Coordinates[ 3 ] , constant_x , constant_y);
		
		A[ counter ] = onePixel;
		
		counter++;
		
	    } // for i
	} // for j
    } // if julia set
        
    MPI_Gather( A , indexSize , MPI_INT , B , indexSize , MPI_INT, root , MPI_COMM_WORLD );   
    	
    for( j = 0 ; j < h ; j++ )
    {
	for( i = 0 ; i < w ; i++ )
	{
	    theColor = B[ theCounter ];
	    
	    if( theColor == 0 )
	    {
	    	image.setPixel( i , j , 0 );
	    } // if
	    
	    else
	    {
		image.setPixel( i , j , colormap[theColor]);
	    } // else
	    
	    theCounter++;
	} // for i
    } // for j
     
    
//    Converts image and sets this pixmap
//    Returns TRUE if successful; otherwise returns FALSE
//    The second argument is the conversion flags and here we pass 0, which sets
//    all the default option   
    pixmap.convertFromImage(image,0);
    
    update();	
    
} // render()
