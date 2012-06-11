#ifndef FRACTAL_H
#define FRACTAL_H

#include <qpixmap.h>
#include <qwidget.h>
#include <qimage.h>
#include <qlabel.h>
#include <qxml.h>
#include <qtextedit.h>
#include <qaction.h>

#include "mainform.h"
#include "metadata.h"
#include "structureparser.h"

class Fractal : public MainForm
{
    Q_OBJECT
    
public:
    Fractal(QWidget *parent = 0 , const char *name = 0 , int wFlags = 0);
    
    void setImageWidth( int w );
    void setImageHeight( int h);
    int getImageWidth();
    int getImageHeight();
    
    void render(int , int , int , double , double , double , double);
    int mandel(int,int,double,double,double,double);
    int julia(int,int,double,double,double,double);
    
    QImage image;
    QPixmap pm,pixmap,pmScaled;
    void load(QString);
    void save( QString );
    void updateStatus( QString );
    
    
protected:
    void resizeEvent( QResizeEvent * );
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void closeEvent( QCloseEvent * );
       
private:

    
    uint rgbFromWaveLength(double wave);
       

    
    enum { ColormapSize = 512 };
    uint colormap[ColormapSize];
    
    StructureParser handler;
    StructureParser s;

    MetaData data;
    
    int imageWidth;
    int imageHeight;
    
    int menuToolHeight;
    int statusHeight;
    
    int topLeftX;
    int topLeftY;
    
    int x;
    int y;
    int sx;
    int sy;
    int sw;
    int sh;	
    
    int typeT;
    double minimumX;
    double maximumX;
    double minimumY;
    double maximumY;
    
    double pixCorx;
    double pixCory;
    double consx;
    double consy;
    
    QPoint pixmapOffset;
    QPoint lastDragPos;
    
    int lastDragPosx;
    int lastDragPosy;
    
    QString theMessage;
    QString moremsg;
    QString filename;
    QString m_filename;
    QString defaultFileName;
    
    QString fileOpenText;
    
    QLabel *status;
    
    int maxWidth;    
    int maxHeight;

    int minWidth;    
    int minHeight;
    
    int saveCount;
    
    float constant_x;
    float constant_y;
    
public slots:
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileSaveJPEG();
    void fileSavePNG();
    void fractalReset();
    void selectType( QAction * );
    void helpContents();
    void quickHelp();
    void about();
    void aboutPhrax();

    
};

#endif
