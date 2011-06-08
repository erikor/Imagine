#include "../Imagine/interfaces.h"
#include "../Imagine/matrix.h"
#include "../doubleSlider/doubleslider.h"

#include "ui_size.h"
#include "ui_sizenp.h"
#include <QObject>
#include <QStringList>
#include <QString>
#include <QInputDialog>
#include <QPixmap>
#include <QBitmap>
#include <QWidget>
#include <QImage>
#include <QtCore/qplugin.h>
#include <QMessageBox>
#include <QtDebug>
#include "ui_progress.h"
#include <QSettings>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QtDebug>
#include <math.h>

class sizeDialog : public QDialog
{
Q_OBJECT

public:
    sizeDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    QVector<int> getIntRange();
    QVector<int> getSizeRange();
    int getChannel();
    void setSizeRange(int, int);
    void setIntRange(int, int);
    void setPreview(QImage*);

public slots:
    void updatePreview();
private:
    Ui::Dialog ui;
    QImage* image;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* maskItem;
    QBitmap segmentByThreshold(QImage* im, int upper, int lower, int ch);
};


class sizeNpDialog : public QDialog
{
Q_OBJECT

public:
    sizeNpDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    QVector<int> getSizeRange();
    void setSizeRange(int, int);

private:
    Ui::sizeNp ui;
};

class progressWindow : public QDialog
{
Q_OBJECT

public:
    progressWindow( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    void setProgress(int);
    void setProgressRange(int, int);
    void resetProgress();
    void setMessage(QString);

private:
    Ui::Progress ui;
};

class basicAnalysis : public QObject, public analysisInterface
{
	Q_OBJECT
	Q_INTERFACES(analysisInterface)

public:
  // analysisInterface
   basicAnalysis();
   QStringList methods();
   Results analyzeImage(const QString &);
   void setOptions(const QString&, QWidget*);
   void init(QList<QImage*>, QBitmap*, QBitmap*, int, int, QWidget* parent=0);

   
signals:
  void progressRange(int, int);
  void progressSet(int);
  void statusMessage(QString);
  
private:
   sizeDialog* sizeRange;
   sizeNpDialog* sizeNpRange;
   QIntMatrix find(QBitmap, QBitmap, int, int, bool annotate=true);
   QIntMatrix find(QBitmap, int, int);
   //QImage flux(iView*, QString*);
   void resolveEq(QIntMatrix*, QVector<int>,  QVector<int>, int, int);
   QList<QGraphicsItem*> annotate(QIntMatrix*);
   QVector<QRect> regions;
   QString stats(QIntMatrix*, QImage*, int, int);   
   progressWindow* progress;
   QBitmap segmentByThreshold(QImage*, int, int, int ch=-1);
   int autoThreshold(QImage*, int, int bias=0);
   QBitmap xand(QBitmap, QBitmap);
   QSettings* settings;
};

void dumpMatrix(QIntMatrix*);



