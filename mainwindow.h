#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

//
// C++ Interface: mainwindow
//
// Description: 
//
//
// Author: Eric Kort,,, <erikor@L3AC213>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "QProgressBar"

#include "ui_mw.h"
#include "dataviewer.h"
#include "interfaces.h"
#include "imganalyzer.h"
#include "imgmodel.h"
#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QGraphicsView>
#include <QString>
#include <QMessageBox>
#include <QMouseEvent>
#include <QFileDialog>
#include <QStringList>
#include <QDir>
#include <QPluginLoader>
#include <QComboBox>
#include <QSplitter>
#include <QSettings>
#include <QDebug>

#ifndef GetR
#define GetR(abgr) ((abgr) & 0xff)
#define GetG(abgr) (((abgr) >> 8) & 0xff)
#define GetB(abgr) (((abgr) >> 16) & 0xff)
#define GetA(abgr) (((abgr) >> 24) & 0xff)
#define RED 0
#define GREEN 1
#define BLUE 2
#endif


class mainWindow : public QMainWindow
{
Q_OBJECT

public:
    mainWindow(QApplication *app, QWidget *parent=0);
    QPixmap loadTiff(QString fn);
    void addText(QString);
    
public slots:
    void zoom(int);
    void openImages();
    void rectSelect();
    void freeSelect();
    void lowPass();
    void highPass();
    void setAutoThreshold(bool);
    void setThreshold(int);
    void analyzeImage();
    void analysisGo();
    void segmentGo();
    void switchImage(int);
    void setRedChannel();
    void setGreenChannel();
    void setBlueChannel();
    void openFlatField();
    void progressSetRange(int, int);
    void progressSetValue(int);
    void statusMessage(QString);
    void chooseColor();
    void chooseChannel();
    void showResults(Results r);
    void saveData();

protected:
    void closeEvent(QCloseEvent*);
    
private:
    Ui::MainWindow ui;
    QHBoxLayout *layout;
    uint32* raster;
    QGraphicsItem* pixmap;
    QGraphicsItem* mask;
    QBitmap segment(QPixmap*, int);
    QPixmap image;
    QGraphicsPixmapItem* maskItem;
    bool imageOpen;
    QStringList pluginFileNames;
    QList<QAction*> plugins;
    QList<QString> fileSeries;
    QString text;
    QProgressBar* progress;
    QLabel* statusLabel;
    imgModel* model;
    imgAnalyzer* analyzer;
    dataViewer* dataview;

};

int min(int, int);


bool open_browser(QWidget*, const QString&);


#endif /*MAINWINDOW_H_*/
