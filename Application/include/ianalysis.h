#include <QPixmap>
#include <QBitmap>
#include <QImage>
#include <QGraphicsView>

class iAnalysis : public QGraphicsView
{
Q_OBJECT

enum iFilterType{LowPass, HighPass, BandPass};

public:
  iAnalysis(QImage*);
  QBitmap OR(QBitmap*, QBitmap*);
  QBitmap AND(QBitmap*, QBitmap*);
  QBitmap segmentChannel(int, iFilterType, int);

public slots:
        
signals:

protected:
   
private:
  QImage* image;
  QBitmap* maskR;
  QBitmap* maskG;
  QBitmap* maskB;
  QBitmap* maskM;
  QBitmap highPass(QBitmap*, int, int);
  QBitmap lowPass(QBitmap*, int, int);
};

