#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <qmainwindow.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>

class QComboBox;
class QPopupMenu;

class HelpWindow : public QMainWindow
{
    Q_OBJECT
public:
    HelpWindow( const QString& home_,  const QString& path, QWidget* parent = 0, const char *name=0 );
    ~HelpWindow();

private slots:
    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );

    void sourceChanged( const QString& );
    void openFile();
    void newWindow();

    void pathSelected( const QString & );


private:
    void readHistory();
    void readBookmarks();

    QTextBrowser* browser;
    QComboBox *pathCombo;
    int backwardId, forwardId;


};





#endif

