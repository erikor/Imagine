#ifndef IMGMODEL_H_
#define IMGMODEL_H_

#include <tiffio.h>
#include <QGraphicsView>
#include <QFileDialog>
#include <QBitmap>
#include <QImage>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QList>
#include <QSettings>
#include <QDebug>

#ifndef GetR
#define GetR(abgr) ((abgr) & 0xff)
#define GetG(abgr) (((abgr) >> 8) & 0xff)
#define GetB(abgr) (((abgr) >> 16) & 0xff)
#define GetA(abgr) (((abgr) >> 24) & 0xff)
#endif


class imgModel : public QObject
{
	Q_OBJECT

public:
  imgModel(QWidget* parent=0);
  //void setParent(QWidget*);
  
  void openImages();
  QStringList getImageNames();
  QImage* getCurrentImage(bool, bool, bool);
  void setCurrentImage(int);
  QList<QImage*> getImages();
  int getCurrentIndex();
  
signals:
  void imageChanged(); 
   
private:
  QImage* loadTiff(QString);
  QList<QImage*> images;
  QStringList imageNames;
  int currentImage;
  QSettings* settings;
  QWidget *parent;
};

#endif
