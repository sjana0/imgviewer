#include "../headers/imgview.h"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardPaths>
#include <QStatusBar>
#include <QMimeDatabase>
#include <QMimeType>
// #include <QDebug>

#include <iostream>

#if defined(QT_PRINTSUPPORT_LIB)
#  include <QtPrintSupport/qtprintsupportglobal.h>

#  if QT_CONFIG(printdialog)
#    include <QPrintDialog>
#  endif
#endif

//! [0]
ImageViewer::ImageViewer(QWidget *parent)
   : QMainWindow(parent), imageLabel(new QLabel)
   , scrollArea(new QScrollArea)
{
	setAcceptDrops(true);
	this->setWindowFlags(Qt::Window);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->setStyleSheet("background-color: rgba(158, 150, 150, 0.99)");

	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);

	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(imageLabel);
	scrollArea->setVisible(false);
	scrollArea->setAlignment(Qt::AlignCenter);
	scrollArea->setStyleSheet("background-color: rgba(62, 74, 79, 0.7)");
	this->setMouseTracking(true);
	scrollArea->setMouseTracking(true);
	imageLabel->setMouseTracking(true);
	setCentralWidget(scrollArea);
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(menuBarHide()));
	QWidget::setAttribute(Qt::WA_MouseTracking);

	createActions();

	resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

bool ImageViewer::isImg(const QString &fileName)
{
	QImageReader reader(fileName);
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();
	return newImage.isNull();
}

void ImageViewer::dragEnterEvent(QDragEnterEvent *e)
{
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void ImageViewer::dropEvent(QDropEvent *e)
{
	foreach (const QUrl &url, e->mimeData()->urls()) {
		const QString fileName = url.toLocalFile();
		this->loadFile(fileName);
	}
}

void ImageViewer::menuBarHide()
{
	menuBar()->setVisible(false);
	statusBar()->setVisible(false);
	timer->stop();
	flg = false;
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
	// qDebug()<<"timer id: \n"<<timer->timerId();
	if(!image.isNull())
	{
		static int menuh = this->menuBar()->QWidget::size().height();
		static int statush = this->statusBar()->size().height();
		int statusposy = this->statusBar()->pos().y();
		int y = event->y();
		// qDebug()<<"statusposy: "<<statusposy<<" "<<"statush: "<<statush<<" "<<"menuh: "<<menuh<<"\n";
		// qDebug()<<"enter: "<<event->type()<<" "<<"pos: "<<event->x()<<" "<<event->y()<<"\n\n";
		// qDebug()<<"not null\n";
		if((y <= menuh && y >= 0) || (y >= statusposy && y <= (statush + statusposy)))
		{
			// qDebug()<<"in zone\n"<<timer->timerId();
			if(flg)
			{
				// qDebug()<<"active\n";
				// timer->stop();
				// timer->start();
				timer->setInterval(4000);
				// qDebug()<<"id = "<<timer->timerId()<<"\n";
				// timer->start(4000);
			}
			else
			{
				// qDebug()<<"here\n";
				menuBar()->setVisible(true);
				statusBar()->setVisible(true);
				timer->start(4000);
				flg = true;
			}
		}
	}
}

//! [0]
//! [2]

bool ImageViewer::loadFile(const QString &fileName)
{
	QImageReader reader(fileName);
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();
	if (newImage.isNull()) {
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
								 tr("Cannot load %1: %2")
								 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
		return false;
	}

//! [2]
	setImage(newImage);

	if(dq.empty())
	{
		setOths(fileName);
	}

	QFileInfo flInfo(fileName);
	QString fName = flInfo.baseName();
	setWindowFilePath(fName);

	const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
		.arg(QDir::toNativeSeparators(fName)).arg(image.width()).arg(image.height()).arg(image.depth());
	statusBar()->showMessage(message);
	if(flag)
	{
		timer->start(4000);
		flag = false;
	}
	return true;
}

void ImageViewer::setImage(const QImage &newImage)
{
	image = newImage;
	imageLabel->setPixmap(QPixmap::fromImage(image));
//! [4]
	scaleFactor = 1.0;

	scrollArea->setVisible(true);
	printAct->setEnabled(true);
	fitToWindowAct->setEnabled(true);
	updateActions();

	if (!fitToWindowAct->isChecked())
		imageLabel->adjustSize();
}

void ImageViewer::setOths(const QString &filePath)
{
	imgPath = filePath;

	QFileInfo* tempFileInfo = new QFileInfo(filePath);
	
	QDir dir = tempFileInfo->dir();
	tempFileInfo->~QFileInfo();
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Time | QDir::Reversed);
	QFileInfoList list = dir.entryInfoList();
	int i = 0;
	QString fileInfoPath = list.at(i).filePath();
	while(fileInfoPath != filePath && i < list.size())
	{
		if(!isImg(fileInfoPath))
		{
			dq.push_back(fileInfoPath);
		}
		i++;
		fileInfoPath = list.at(i).filePath();
	}
	i = list.size() - 1;
	fileInfoPath = list.at(i).filePath();
	while(fileInfoPath != filePath && i >= 0)
	{
		if(!isImg(fileInfoPath))
		{
			dq.push_front(fileInfoPath);
		}
		i--;
		fileInfoPath = list.at(i).filePath();
	}
}
//! [4]

bool ImageViewer::saveFile(const QString &fileName)
{
	QImageWriter writer(fileName);

	if (!writer.write(image)) {
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
								 tr("Cannot write %1: %2")
								 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
		return false;
	}
	const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
	statusBar()->showMessage(message);
	return true;
}

void ImageViewer::fullScreen()
{
	if(!isF)
	{
		this->showFullScreen();
		this->isF = true;
	}
}

void ImageViewer::escapeFullScreen()
{
	if(isF)
	{
		this->showNormal();
		this->isF = false;
	}
}

//! [1]

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
	static bool firstDialog = true;

	if (firstDialog) {
		firstDialog = false;
		const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
		dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
	}

	QStringList mimeTypeFilters;
	const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
		? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
	for (const QByteArray &mimeTypeName : supportedMimeTypes)
		mimeTypeFilters.append(mimeTypeName);
	mimeTypeFilters.sort();

	QMimeDatabase mimeDB;
	QStringList allSuppFormats;

	for(const QString& mimeTypeFilter: mimeTypeFilters) {
		QMimeType mimeType = mimeDB.mimeTypeForName(mimeTypeFilter);
		if(mimeType.isValid()) {
			allSuppFormats.append(mimeType.globPatterns());
		}
	}
	QString allSuppFormatsFilter = QString("All supported formats (%1)").arg(allSuppFormats.join(' '));
	
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setMimeTypeFilters(mimeTypeFilters);
	QStringList nameFilters = dialog.nameFilters();
	nameFilters.append(allSuppFormatsFilter);
	dialog.setNameFilters(nameFilters);
	dialog.selectNameFilter(allSuppFormatsFilter);
}

void ImageViewer::open()
{
	if(!ImageViewer::dq.empty())
	{
		ImageViewer::dq.clear();
	}
	QFileDialog dialog(this, tr("Open File"));
	initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
	while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}
//! [1]

void ImageViewer::saveAs()
{
	QFileDialog dialog(this, tr("Save File As"));
	initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

	while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::nextImage()
{
	dq.push_back(imgPath);
	imgPath = dq.front();
	loadFile(imgPath);
	scrollArea->verticalScrollBar()->setValue(0);
	dq.pop_front();
}

void ImageViewer::prevImage()
{
	dq.push_front(imgPath);
	imgPath = dq.back();
	loadFile(imgPath);
	dq.pop_back();
}

//! [5]
void ImageViewer::print()
//! [5] //! [6]
{
	Q_ASSERT(!imageLabel->pixmap()->isNull());
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printdialog)
//! [6] //! [7]
	QPrintDialog dialog(&printer, this);
//! [7] //! [8]
	// if (dialog.exec()) {
	//     QPainter painter(&printer);
	//     QPixmap *pixmap = imageLabel->pixmap();
	//     QRect rect = painter.viewport();
	//     QSize size = pixmap->size();
	//     size.scale(rect.size(), Qt::KeepAspectRatio);
	//     painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
	//     painter.setWindow(pixmap->rect());
	//     painter.drawPixmap(0, 0, pixmap);
	// }
#endif
}
//! [8]

void ImageViewer::copy()
{
#ifndef QT_NO_CLIPBOARD
	QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
	if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
		if (mimeData->hasImage()) {
			const QImage image = qvariant_cast<QImage>(mimeData->imageData());
			if (!image.isNull())
				return image;
		}
	}
	return QImage();
}
#endif // !QT_NO_CLIPBOARD

void ImageViewer::paste()
{
#ifndef QT_NO_CLIPBOARD
	const QImage newImage = clipboardImage();
	if (newImage.isNull()) {
		statusBar()->showMessage(tr("No image in clipboard"));
	} else {
		setImage(newImage);
		setWindowFilePath(QString());
		const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
			.arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
		statusBar()->showMessage(message);
	}
#endif // !QT_NO_CLIPBOARD
}

//! [9]
void ImageViewer::zoomIn()
//! [9] //! [10]
{
	scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
	scaleImage(0.8);
}

//! [10] //! [11]
void ImageViewer::normalSize()
//! [11] //! [12]
{
	imageLabel->adjustSize();
	scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImageViewer::fitToWindow()
//! [13] //! [14]
{
	bool fitToWindow = fitToWindowAct->isChecked();
	scrollArea->setWidgetResizable(fitToWindow);
	if (!fitToWindow)
		normalSize();
	updateActions();
}
//! [14]


//! [15]
void ImageViewer::about()
//! [15] //! [16]
{
	QMessageBox::about(this, tr("About Image Viewer"),
			tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
			   "and QScrollArea to display an image. QLabel is typically used "
			   "for displaying a text, but it can also display an image. "
			   "QScrollArea provides a scrolling view around another widget. "
			   "If the child widget exceeds the size of the frame, QScrollArea "
			   "automatically provides scroll bars. </p><p>The example "
			   "demonstrates how QLabel's ability to scale its contents "
			   "(QLabel::scaledContents), and QScrollArea's ability to "
			   "automatically resize its contents "
			   "(QScrollArea::widgetResizable), can be used to implement "
			   "zooming and scaling features. </p><p>In addition the example "
			   "shows how to use QPainter to print an image.</p>"));
}
//! [16]

//! [17]
void ImageViewer::createActions()
//! [17] //! [18]
{
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

	QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::open);
	openAct->setShortcut(QKeySequence::Open);
	actions.push_back(openAct);

	QAction *rightOpenAct = fileMenu->addAction(tr("&Next..."), this, &ImageViewer::nextImage);
	rightOpenAct->setShortcut(tr("Right"));
	actions.push_back(rightOpenAct);

	QAction *fullScr = fileMenu->addAction(tr("&Full Screen..."), this, &ImageViewer::fullScreen);
	fullScr->setShortcut(tr("F"));
	actions.push_back(fullScr);

	QAction *escFullScr = fileMenu->addAction(tr("&Full Screen..."), this, &ImageViewer::escapeFullScreen);
	escFullScr->setShortcut(tr("esc"));
	actions.push_back(escFullScr);

	QAction *leftOpenAct = fileMenu->addAction(tr("&Prev..."), this, &ImageViewer::prevImage);
	leftOpenAct->setShortcut(tr("Left"));
	actions.push_back(leftOpenAct);

	saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
	saveAsAct->setEnabled(false);

	printAct = fileMenu->addAction(tr("&Print..."), this, &ImageViewer::print);
	printAct->setShortcut(QKeySequence::Print);
	printAct->setEnabled(false);

	fileMenu->addSeparator();

	QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
	exitAct->setShortcut(tr("Ctrl+Q"));
	actions.push_back(exitAct);

	QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

	copyAct = editMenu->addAction(tr("&Copy"), this, &ImageViewer::copy);
	copyAct->setShortcut(QKeySequence::Copy);
	copyAct->setEnabled(false);

	QAction *pasteAct = editMenu->addAction(tr("&Paste"), this, &ImageViewer::paste);
	pasteAct->setShortcut(QKeySequence::Paste);

	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

	zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
	zoomInAct->setShortcut(QKeySequence::ZoomIn);
	zoomInAct->setEnabled(false);
	actions.push_back(zoomInAct);

	zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
	zoomOutAct->setShortcut(QKeySequence::ZoomOut);
	zoomOutAct->setEnabled(false);
	actions.push_back(zoomOutAct);

	normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
	normalSizeAct->setShortcut(tr("Ctrl+S"));
	normalSizeAct->setEnabled(false);
	actions.push_back(normalSizeAct);

	viewMenu->addSeparator();

	fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ImageViewer::fitToWindow);
	fitToWindowAct->setEnabled(false);
	fitToWindowAct->setCheckable(true);
	fitToWindowAct->setShortcut(tr("Ctrl+F"));
	actions.push_back(fitToWindowAct);

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
	QAction* aboutAct = helpMenu->addAction(tr("&About"), this, &ImageViewer::about);
	actions.push_back(aboutAct);
	this->addActions(actions);
}
//! [18]

//! [21]
void ImageViewer::updateActions()
//! [21] //! [22]
{
	saveAsAct->setEnabled(!image.isNull());
	copyAct->setEnabled(!image.isNull());
	zoomInAct->setEnabled(!fitToWindowAct->isChecked());
	zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
	normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}
//! [22]

//! [23]
void ImageViewer::scaleImage(double factor)
//! [23] //! [24]
{
	scaleFactor *= factor;
	imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

	adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
	adjustScrollBar(scrollArea->verticalScrollBar(), factor);

	zoomInAct->setEnabled(scaleFactor < 3.0);
	zoomOutAct->setEnabled(scaleFactor > 0.333);
}
//! [24]

//! [25]
void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
	scrollBar->setValue(int(factor * scrollBar->value()
							+ ((factor - 1) * scrollBar->pageStep()/2)));
}
//! [26]
