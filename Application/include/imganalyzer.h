#ifndef IMGANALYZER_H_
#define IMGANALYZER_H_

#include "imgmodel.h"
#include "interfaces.h"

#include <QApplication>
#include <QPolygonF>
#include <QFileDialog>
#include <QBitmap>
#include <QImage>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QCursor>
#include <QList>
#include <QSettings>
#include <QPainter>
#include <QDebug>
#include <QPluginLoader>
#include <QAction>
#include <QObject>


struct analysisResults {
	QList<QGraphicsItem*> annotations;
	QString data;
};

struct pluginList {
	QList<QAction*> actions;
	QStringList names;
	QStringList descriptions;
};


enum Channel {
  	Red,
  	Green,
  	Blue,
  	Cyan,
  	Magenta,
  	Yellow
};
  
enum Filter {
  	HighPass,
  	LowPass,
  	BandPass
};


class imgAnalyzer : public QObject
{
Q_OBJECT

public:
  imgAnalyzer(QWidget* parent);

  void loadPlugins();
  void initPlugin(QObject*);
  void segmentImage(QString);
  void analyzeImage(QString);
  QList<QGraphicsItem*> getAnnotationItems();
  QString getAnalysisResults();
  QBitmap* getCurrentMask();
  void setChannel(Channel);
  void setFilter(Filter);
  QStringList getSegmentationMethods();
  QStringList getAnalysisMethods();
  void openImages();
  QImage* getCurrentImage(bool, bool, bool);
  QStringList getFileNames();
  
  void setCurrentImage(int);
    
signals:
  void annotationChanged();
  void bitmaskChanged(QBitmap);
  void analysisCompleted(Results);
 
public slots:
  void segmentationPluginGo(int);
  void analysisPluginGo(int);
  void segmentImageExec();
  void analyzeImageExec();
  void setRoi(QBitmap);
  
private:
  //QList<QGraphicsItem*> annotationItems;
  imgModel* model;
  QBitmap bitmask;
  QBitmap roi;
  pluginList analysisPlugins;
  pluginList segmentationPlugins;
  Results result;
  Channel currentChannel;
  QSettings *settings;
};



#endif
