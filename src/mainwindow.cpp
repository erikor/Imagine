// mainwindow.cpp
//
// implementation of iDMainWindow class
//
// sets up the main GUI window including action bindings
//
// Author: Eric Kort. <eric.kort@vai.org>, 
//
// Copyright: See COPYING file that comes with this distribution
//
 
#include "mainwindow.h"
#include <QProcess>

mainWindow::mainWindow(QApplication *app, QWidget *parent) 
    : QMainWindow(parent)
{
  ui.setupUi(this);
  ui.actionLowPass->setCheckable(true);
  ui.actionHighPass->setCheckable(true);
  ui.actionRectSelect->setCheckable(true);
  ui.actionFreeSelect->setCheckable(true);

  ui.actionLowPass->setChecked(true);
  ui.actionRectSelect->setChecked(true);

  QObject::connect(ui.zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoom(int)));
  QObject::connect(ui.actionFileOpen, SIGNAL(activated()), this, SLOT(openImages()));
  QObject::connect(ui.actionRectSelect, SIGNAL(activated()), this, SLOT(rectSelect()));
  QObject::connect(ui.actionFreeSelect, SIGNAL(activated()), this, SLOT(freeSelect()));
  QObject::connect(ui.actionHighPass, SIGNAL(activated()), this, SLOT(highPass()));
  QObject::connect(ui.actionLowPass, SIGNAL(activated()), this, SLOT(lowPass()));
  QObject::connect(ui.actionRedChannel, SIGNAL(activated()), this, SLOT(setRedChannel()));
  QObject::connect(ui.actionGreenChannel, SIGNAL(activated()), this, SLOT(setGreenChannel()));
  QObject::connect(ui.actionBlueChannel, SIGNAL(activated()), this, SLOT(setBlueChannel()));
  QObject::connect(ui.checkLockRoi, SIGNAL(toggled(bool)), ui.graphicsView, SLOT(setRoiLocked(bool)));
  QObject::connect(ui.checkLockMask, SIGNAL(toggled(bool)), ui.graphicsView, SLOT(setMaskLocked(bool)));
  QObject::connect(ui.pluginGo, SIGNAL(clicked()), this, SLOT(analysisGo()));
  QObject::connect(ui.segmentationConfigure, SIGNAL(clicked()), this, SLOT(segmentGo()));
  QObject::connect(ui.imageList, SIGNAL(currentRowChanged(int)), this, SLOT(switchImage(int)));
  QObject::connect(ui.actionFlatField, SIGNAL(activated()), this, SLOT(openFlatField()));
  QObject::connect(ui.actionSaveImage, SIGNAL(activated()), ui.graphicsView, SLOT(saveImage()));
  QObject::connect(ui.actionSaveData, SIGNAL(activated()), this, SLOT(saveData()));
  QObject::connect(ui.actionPrintImage, SIGNAL(activated()), ui.graphicsView, SLOT(printImage()));
  QObject::connect(ui.actionChooseFont, SIGNAL(activated()), ui.graphicsView, SLOT(chooseFont()));
  QObject::connect(ui.actionChooseColor, SIGNAL(activated()), this, SLOT(chooseColor()));
  QObject::connect(ui.channelRed, SIGNAL(stateChanged(int)), this, SLOT(chooseChannel()));
  QObject::connect(ui.channelGreen, SIGNAL(stateChanged(int)), this, SLOT(chooseChannel()));
  QObject::connect(ui.channelBlue, SIGNAL(stateChanged(int)), this, SLOT(chooseChannel()));
  imageOpen = false;
  ui.graphicsView->resize(ui.graphicsView->width(), 700);
  QFont f("Courier", 8, QFont::Normal);
  ui.dataView->setFont(f);
  addText("Data output...");
  addText(" ");
  analyzer = new imgAnalyzer(this);
  QObject::connect(analyzer, SIGNAL(analysisCompleted(Results)), this, SLOT(showResults(Results)));

  ui.graphicsView->setAnalyzer(analyzer);
  ui.segmentationMethods->addItems(analyzer->getSegmentationMethods());
  ui.analysisMethods->addItems(analyzer->getAnalysisMethods());
  analyzer->setChannel(Red);
  //dataview = new dataViewer();
  //dataview->show();
  //dataview->newSection("Section 1");
  //dataview->addData("Data item 1");
  //QString program = "Rgui.exe";
  //QStringList arguments;
  //open_browser(this, "http://www.google.com");
  //arguments << "" << "";
  //QProcess *myProcess = new QProcess(parent);
  //myProcess->start(program, arguments);
  return;
}


void mainWindow::progressSetRange(int f, int t)
{
	progress->reset();
	progress->setRange(f, t);
}


void mainWindow::statusMessage(QString s)
{
  statusLabel->setText(s);
}

void mainWindow::progressSetValue(int v)
{
	progress->setValue(v);
}


void mainWindow::openFlatField() {
	ui.graphicsView->openFlatField();
	return;
}

void mainWindow::closeEvent(QCloseEvent *event)
{
	//dataview->close();
    event->accept();
}


void mainWindow::openImages()
{
	ui.graphicsView->openImages();
	QStringList files = ui.graphicsView->getFileNames();
	if(files.size() < 1) {
		return;
	}
	ui.imageList->clear();
	foreach(QString item, files){
      QFileInfo f(item);
      ui.imageList->addItem(f.fileName());
	}
    ui.zoomSlider->setValue(ui.graphicsView->zoomAll());
    QStringList msg;
}

void mainWindow::switchImage(int index)
{
	ui.graphicsView->switchImage(index);
    ui.graphicsView->zoom(ui.zoomSlider->value());  	
}


void mainWindow::zoom(int z)
{
    ui.graphicsView->zoom(z);  	
	return;
}

void mainWindow::freeSelect() {
    ui.actionRectSelect->setChecked(false);		
    ui.actionFreeSelect->setChecked(true);		
	ui.graphicsView->setFreeSelect();
}

void mainWindow::rectSelect() {
    ui.actionFreeSelect->setChecked(false);	
    ui.actionRectSelect->setChecked(true);		
	ui.graphicsView->setRectSelect();
}

void mainWindow::highPass() {
	
    ui.actionLowPass->setChecked(false);
    ui.actionHighPass->setChecked(true);
	ui.graphicsView->setHighPass();
}

void mainWindow::lowPass() {
    ui.actionHighPass->setChecked(false);
    ui.actionLowPass->setChecked(true);
	ui.graphicsView->setLowPass();
}


void mainWindow::setAutoThreshold(bool autoTh){
  if(autoTh) {
  	ui.graphicsView->setAutoThreshold(true);
  } else {
  	ui.graphicsView->setAutoThreshold(false);
  }
}

void mainWindow::setThreshold(int th) {
  	return;
}


void mainWindow::analyzeImage(){

}

void mainWindow::analysisGo() {
	int index = ui.analysisMethods->currentIndex();
    qDebug() << index;
    analyzer->analysisPluginGo(index);
}

void mainWindow::segmentGo() {
  int index = ui.segmentationMethods->currentIndex();
  analyzer->segmentationPluginGo(index);
}

void mainWindow::setRedChannel() {
	ui.graphicsView->setChannel(0);
	ui.actionRedChannel->setChecked(true);
	ui.actionBlueChannel->setChecked(false);
	ui.actionGreenChannel->setChecked(false);
	analyzer->setChannel(Red);
}

void mainWindow::setGreenChannel() {
	ui.graphicsView->setChannel(1);
	ui.actionRedChannel->setChecked(false);
	ui.actionBlueChannel->setChecked(false);
	ui.actionGreenChannel->setChecked(true);
	analyzer->setChannel(Green);
}

void mainWindow::setBlueChannel() {
	ui.graphicsView->setChannel(2);
	ui.actionRedChannel->setChecked(false);
	ui.actionBlueChannel->setChecked(true);
	ui.actionGreenChannel->setChecked(false);
	analyzer->setChannel(Blue);
}

void mainWindow::saveData() {
  QSettings* settings = new QSettings("VARI Epi", "Imagine");
  QString dir = settings->value("lastdir", QString()).toString();

  if (dir.isNull())
  	dir = "/";
  QString fn = QFileDialog::getSaveFileName(this, 
                          tr("Save Data"),
                          dir,
                          tr("HTML (*.htm; *.html)"));

  QFile file(fn);
     if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return;

     QTextStream out(&file);
     out << ui.dataView->toHtml();
 }
     
     
void mainWindow::showResults(Results r){
	text.append(r.data);
	text.append("\n");
  	ui.dataView->setHtml(text);
}

void mainWindow::addText(QString t){
	text.append(t);
	text.append("\n");
  	ui.dataView->setHtml(text);
}

void mainWindow::chooseColor()
{
  ui.graphicsView->chooseColor();
  /* Now a horrible cludge to get the current image annotation to update */
  if(ui.actionLowPass->isChecked()) {
    ui.graphicsView->setLowPass();
  } else {
    ui.graphicsView->setHighPass();   	
  }
}

void mainWindow::chooseChannel()
{
  bool r=false, g=false, b=false;
  if(ui.channelRed->isChecked())
  	r = true;
  if(ui.channelGreen->isChecked())
  	g = true;
  if(ui.channelBlue->isChecked())
  	b = true;
  ui.graphicsView->switchChannel(r,g,b);
  
}


bool open_browser(QWidget* parent, const QString& rUrl)
{
bool result = false;


return result;
}
