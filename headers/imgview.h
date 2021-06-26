#pragma once

// #include "./cusmenu.h"
#include <QMainWindow>
#include <QImage>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QTimer>
#include <iostream>
#include <deque>
#if defined(QT_PRINTSUPPORT_LIB)
#  include <QtPrintSupport/qtprintsupportglobal.h>

#  if QT_CONFIG(printer)
#    include <QPrinter>
#  endif
#endif

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

//! [0]
class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget *parent = nullptr);
	bool loadFile(const QString &);

private slots:
	void open();
	void saveAs();
	void fullScreen();
	void escapeFullScreen();
	void print();
	void copy();
	void paste();
	void zoomIn();
	void zoomOut();
	void normalSize();
	void fitToWindow();
	void about();
	void nextImage();
	void prevImage();
	void menuBarHide();

private:
	void createActions();
	void createMenus();
	void updateActions();
	bool saveFile(const QString &fileName);
	void setImage(const QImage &newImage);
	void setOths(const QString &filePath);
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	bool isImg(const QString &fileName);
	void printFileList();
	void dragEnterEvent(QDragEnterEvent*) override;
	void dropEvent(QDropEvent*) override;
	// void enterEvent(QEvent*) override;

	QImage image;
	QString imgPath;
	QTimer *timer;
	std::deque <QString> dq;
	QLabel *imageLabel;
	QScrollArea *scrollArea;
	double scaleFactor = 1;
	bool isF = false;
	// CusMenu *menu;

#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
	QPrinter printer;
#endif

	QList<QAction*> actions;

	QAction *saveAsAct;
	QAction *printAct;
	QAction *copyAct;
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *normalSizeAct;
	QAction *fitToWindowAct;

protected:
	void mouseMoveEvent(QMouseEvent*) override;


};
//! [0]