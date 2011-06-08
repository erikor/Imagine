#ifndef IVIEW_H_
#define IVIEW_H_

#include <QGraphicsView>
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
#include <QGraphicsRectItem>
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

#include "imganalyzer.h"

#define RED 0
#define GREEN 1
#define BLUE 2
#define FREESELECT 1
#define RECTSELECT 0
#define LOWPASS 0
#define HIGHPASS 1






class iView : public QGraphicsView
{
Q_OBJECT

public:
    iView(QWidget *parent=0);
    void reset();
    void zoom(int z);
    int zoomAll();
    void openImage();
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
    void setAutoThreshold(bool);
    void setRoiLocked(bool);
    void setMaskLocked(bool);
    void switchImage(int);
    void setChannel(int);
    void openFlatField();
    void normalize();
    void saveImage();
    void printImage();
    void chooseColor();
    void chooseFont();
    void switchChannel(bool, bool, bool)  ;
          
signals:
   void roiChanged(int);
   
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
  void highPass();
  void lowPass();
  int autoThreshold(QImage*, int);  
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
