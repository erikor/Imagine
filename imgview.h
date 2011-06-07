#ifndef IMGVIEW_H_
#define IMGVIEW_H_

#include "imganalyzer.h"
#include "interfaces.h"
#include <QGraphicsView>
#include <QObject>
#include <QGraphicsTextItem>
#include <QRubberBand>
#include <QGraphicsRectItem>
#include <QPolygonF>
#include <tiffio.h>
#include <QFileDialog>
#include <QBitmap>
#include <QImage>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPen>
#include <QCursor>
#include <QList>
#include <QSettings>
#include <QPainter>
#include <QDebug>
#include <QPrinter>
#include <QPrintDialog>
#include <QColorDialog>
#include <QFontDialog>

#define RED 0
#define GREEN 1
#define BLUE 2
#define FREESELECT 1
#define RECTSELECT 0
#define LOWPASS 0
#define HIGHPASS 1



class imgView : public QGraphicsView
{
	Q_OBJECT

public:
    imgView(QWidget *parent=0);
    void setAnalyzer(imgAnalyzer*);
    void reset();
    void zoom(int z);
    int zoomAll();
	void setFreeSelect();
	void setRectSelect();
    void setLowPass();
    void setHighPass();
    QRectF getRectRoi();
    QPolygonF getFreeRoi();    
    QBitmap getRoi();
    QBitmap getBitmask();
    QImage* getImage();
    int getThreshold();
    int getChannel();
    void setThreshold(int);
    QStringList getFileNames();
    QStringList intAverage();
    bool isMaskLocked();
    bool isRoiLocked();
    QGraphicsScene* getScene();
    void addAnnotationItem(QGraphicsItem*);
    void clearAnnotations();
    bool flatField;
    void addAnnotationText(QString, QPoint);
    void clearTextItems();
    
public slots:
    void openImages();
    void setAutoThreshold(bool);
    void setRoiLocked(bool);
    void setMaskLocked(bool);
    void switchImage(int);
    void setChannel(int);
    void openFlatField();
    void saveImage();
    void printImage();
    void chooseColor();
    void chooseFont();
    void switchChannel(bool, bool, bool);
    QStringList getSegmentationMethods();
    void setMask(QBitmap);
    void annotate(Results);
    void clearMask();
          
signals:
   void roiChanged(QBitmap);
   
protected:
  void mouseMoveEvent(QMouseEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  
private:
  imgAnalyzer* analyzer;
  int opencount;
  QPointF roiStart;  
  QStringList files;
  QGraphicsRectItem* roiRect;
  bool haveRoi;
  QGraphicsPolygonItem* roiFree; 
  QImage* roiFreeImage;
  QPolygonF roiPoly;
  QImage loadTiff(QString);
  QGraphicsPixmapItem* pixmap;
  QList<QGraphicsItem*> annotationGroup;
  QImage* image;
  QGraphicsScene* scene;
  QPixmap mask;
  int select;
  int filter;
  bool imageLoaded;
  int threshold;
  bool useAutoThreshold;
  QString fileName;
  bool roiLocked;
  bool maskLocked;
  int channel;
  QImage flatFieldImage;
  void flatFieldAdjust();
  QVector<QGraphicsTextItem*> textItems;
  QSettings* settings;
  bool haveimage;  
  QColor color;
  QFont font;
  void setFont();
  void setColor();
  bool r_ch;
  bool g_ch;
  bool b_ch;
};


#endif
