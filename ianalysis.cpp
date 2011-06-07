#include "iAnalysis.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPen>
#include <QBitmap>

iAnalysis::iAnalysis(QImage* im)
{
  image = im;
  maskR = new QBitmap(image->height(), image->width());  maskR->clear();
  maskG = new QBitmap(image->height(), image->width());  maskG->clear();
  maskB = new QBitmap(image->height(), image->width());  maskB->clear();
  maskM = new QBitmap(image->height(), image->width());  maskM->clear();
}

QBitmap iAnalysis::OR(QBitmap* a, QBitmap* b)
{
  int x, y, 
      w = a->width(),
      h = a->height(),
      pa,
      pb;
  QBitmap r;
  QImage ima = a->toImage();
  QImage imb = b->toImage();
  QImage imm = QImage(w, h, QImage::Format_Mono);
    
  for(y=0; y < h; y++) {
    for(x=0; x < w; x++) {
      pa = ima.pixel(x,y);
      pb = imb.pixel(x,y);

      if (pa || pb) {
        imm.setPixel(x, y, Qt::color1);
      } else {
        imm.setPixel(x, y, Qt::color0);
      }
    }
  }
  r = QBitmap::fromImage(imm);
  return(r);
}

QBitmap iAnalysis::AND(QBitmap* a, QBitmap* b)
{
  int x, y, 
      w = a->width(),
      h = a->height(),
      pa,
      pb;
  QBitmap r;
  QImage ima = a->toImage();
  QImage imb = b->toImage();
  QImage imm = QImage(w, h, QImage::Format_Mono);
    
  for(y=0; y < h; y++) {
    for(x=0; x < w; x++) {
      pa = ima.pixel(x,y);
      pb = imb.pixel(x,y);

      if (pa && pb) {
        imm.setPixel(x, y, Qt::color1);
      } else {
        imm.setPixel(x, y, Qt::color0);
      }
    }
  }
  r = QBitmap::fromImage(imm);
  return(r);
}

QBitmap iAnalysis::segmentChannel(int ch, iFilterType filter, int th = -1) {
  QBitmap* mask;
  if (ch==0) {
  	mask = maskR;
   } else if (ch == 1) {
 		mask = maskG;
   } else if (ch == 2) {
 		mask = maskB;
   }
   if (filter == iAnalysis::HighPass) {
   	   return(highPass(mask, ch, th));
   } else {
   	   return(lowPass(mask, ch, th));
   }
}

QBitmap iAnalysis::lowPass(QBitmap* mask, int ch, int th) {
  int x, y, 
      w = image->width(),
      h = image->height(),
      p;
  QBitmap r;
  QImage m = mask->toImage();
    
  for(y=0; y < h; y++) {
    for(x=0; x < w; x++) {
      if(ch == 0) {
        p = qRed(image->pixel(x,y));	 
      } else if(ch == 1) {
        p = qGreen(image->pixel(x,y));
      } else if(ch == 2) {
        p = qBlue(image->pixel(x,y));
      }
      if (p < th) {
           m.setPixel(x, y, Qt::color1);      	
      }
    }
  }
  r = QBitmap::fromImage(m);
  return(r);
}

QBitmap iAnalysis::highPass(QBitmap* mask, int ch, int th) {
  int x, y, 
      w = image->width(),
      h = image->height(),
      p;
  QBitmap r;
  QImage m = mask->toImage();
    
  for(y=0; y < h; y++) {
    for(x=0; x < w; x++) {
      if(ch == 0) {
        p = qRed(image->pixel(x,y));	 
      } else if(ch == 1) {
        p = qGreen(image->pixel(x,y));
      } else if(ch == 2) {
        p = qBlue(image->pixel(x,y));
      }
      if (p < th) {
           m.setPixel(x, y, Qt::color1);      	
      }
    }
  }
  r = QBitmap::fromImage(m);
  return(r);
}