#include "interfaces.h"
#include "matrix.h"
#include <QMessageBox>
#include <QList>
#include <QImage>
#include <QBitmap>
#include <QSettings>
#include <QtCore/qplugin.h>
#include "ui_manual.h"
#include "ui_halo.h"

class rangeDialog : public QDialog
{
Q_OBJECT

public:
    rangeDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    QVector<int> getRange();
    void setAutoTh(int);
    void setSpan(int, int);

public slots:
    void autoToggle();
    void setUpperLabel(int);
    void setLowerLabel(int);
    
private:
    Ui::Dialog ui;
    int autoTh;
    QSettings* settings;
};

class haloDialog : public QDialog
{
Q_OBJECT

public:
    haloDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    QVector<int> getIntRange();
    QVector<int> getOffRange();
    void setAutoTh(int);

public slots:
    void autoToggle();
    void setUpperLabel(int);
    void setLowerLabel(int);
    void setOffsetLabel(int);
    void setWidthLabel(int);
    
private:
    Ui::Halo ui;
    int autoTh;
};

class basicSegmentation : public QObject, public segmentationInterface
{
Q_OBJECT
Q_INTERFACES(segmentationInterface)

public:
   
   basicSegmentation();
   QStringList methods();
   QBitmap segmentImage(const QString &);
   void setOptions(const QString&, QWidget*);
   
private:
   int autoThreshold(QImage*, int);
   rangeDialog* thresholdRange;
   haloDialog* haloSettings;
   QBitmap segmentByThreshold(QImage*, int, int, int ch=-1);
   QBitmap dilate(QBitmap, int);
   QBitmap nonMergeDilate(QBitmap, int);
   QBitmap erode(QBitmap, int);
   QBitmap subtract(QBitmap, QBitmap);
   void resolveEq(QIntMatrix* m, QVector<int> eq, QVector<int> counts, int minsize);
   QIntMatrix find(QBitmap mask, int minsize);
   QBitmap xand(QBitmap am, QBitmap bm);
   QSettings* settings;

//  QList<QImage*> images;
//  QImage* roi;
//  QBitmap* mask;
//  int currentIndex;
//  QWidget* app;      
};

int uniqueValues(QVector<int>);
