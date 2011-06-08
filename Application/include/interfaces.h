#ifndef INTERFACES_H_
#define INTERFACES_H_

#include <QStringList>
#include <QString>
#include <QInputDialog>
#include <QPixmap>
#include <QWidget>
#include <QBitmap>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsRectItem>


struct Results {
   	QString data;
   	QList<QGraphicsItem*> annotations;
};

class analysisInterface
{
public:
   
   virtual ~analysisInterface() {};
   virtual QStringList methods() = 0;
   virtual Results analyzeImage(const QString &) = 0;
   virtual void init(QList<QImage*>, QBitmap*, QBitmap*, int, int, QWidget* parent=0) = 0;
   Results analysisResults;
   QList<QImage*> images;
   QBitmap* roi;
   QBitmap* mask;
   int currentIndex;
   int currentChannel;
   QWidget* app;   
   
private:

};

class segmentationInterface
{
public:
   
   virtual ~segmentationInterface() {}
   virtual QStringList methods() = 0;
   virtual QBitmap segmentImage(const QString &) = 0;
   virtual void setOptions(const QString &, QWidget*) = 0;   
   void init(QList<QImage*>, QBitmap*, int, int, QWidget* parent=0);
   QList<QImage*> images;
   QBitmap* roi;
   QBitmap* mask;
   int currentIndex;
   int currentChannel;
   QWidget* app;   
   
private:
     
};

Q_DECLARE_INTERFACE(analysisInterface,
                     "com.vari.imagine.analysisInterface/1.0")
                     
Q_DECLARE_INTERFACE(segmentationInterface,
                     "com.vari.imagine.segmentationInterface/1.0")

#endif 
