TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

LIBS	+= -pthread -llammpio -llammpi++ -llamf77mpi -lmpi -llam -lutil -ldl

HEADERS	+= fractal.h \
	metadata.h \
	structureparser.h \
	computation.h \
	helpwindow.h

SOURCES	+= main.cpp \
	fractal.cpp \
	metadata.cpp \
	structureparser.cpp \
	computation.cpp \
	helpwindow.cpp

FORMS	= mainform.ui

IMAGES	= images/gtk/gtk-redo.png \
	images/gtk/gnome-unknown.png \
	images/gtk/gtk-print.png \
	images/gtk/gtk-save-as.png \
	images/gtk/gtk-save.png \
	images/gtk/gtk-stock-fullscreen.png \
	images/gtk/gtk-undo.png \
	images/gtk/panel-logout.png \
	images/gtk/stock-import.png \
	images/gtk/stock-new.png \
	images/gtk/stock-open.png \
	images/gtk/stock-zoom-in.png \
	images/gtk/stock-zoom-out.png \
	images/gtk/handbook.png \
	images/gtk/gtk-refresh.png

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}





