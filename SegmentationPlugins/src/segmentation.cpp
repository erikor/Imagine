#include "segmentation.h"
#include <QDebug>

basicSegmentation::basicSegmentation(){
}

QStringList basicSegmentation::methods(){
  return QStringList() << "Intensity threshold" << "Halo" << "Threshold and dilate (non merging)" 
    << "Two channel threshold (non merging)";
}

QBitmap basicSegmentation::segmentImage(const QString &method){
  if(currentIndex < 0) {
  	QMessageBox::warning(0, "No images open.", "Please open an image prior to segmentation.");
  	return(QBitmap());
  }

  QImage *im = images.at(currentIndex);
  int upper;
  int lower;
  if(method == "Intensity threshold") {
  	
   	int ath;
   	ath = autoThreshold(im, currentChannel);
    thresholdRange = new rangeDialog();
    thresholdRange->setAutoTh(ath);
    thresholdRange->exec();
    int r = thresholdRange->result();
    if (r==0) {
    	QBitmap a = QBitmap();
    	return(a);
   	}
    upper = thresholdRange->getRange()[1];
    lower = thresholdRange->getRange()[0];
    return(segmentByThreshold(im, upper, lower));
  } else if(method == "Halo") {
   	int ath;
   	ath = autoThreshold(im, currentChannel);
    haloSettings = new haloDialog();
    haloSettings->setAutoTh(ath);
    haloSettings->exec();
    int r = haloSettings->result();
    if (r==0) {
    	return(QBitmap());
   	}
    upper = haloSettings->getIntRange()[1];
    lower = haloSettings->getIntRange()[0];
    int offset = haloSettings->getOffRange()[0];
    int width = haloSettings->getOffRange()[1];
    QBitmap bm = segmentByThreshold(im, upper, lower);
  	QBitmap dbm = nonMergeDilate(bm, offset+1);
  	QBitmap dbm2 = nonMergeDilate(bm, width);
    bm = subtract(dbm, dbm2);
    return(bm);


  } else if(method == "Threshold and dilate (non merging)") {
  	
   	int ath;
   	ath = autoThreshold(im, currentChannel);
    haloSettings = new haloDialog();
    haloSettings->setAutoTh(ath);
    haloSettings->exec();
    int r = haloSettings->result();
    if (r==0) {
    	QBitmap a = QBitmap();
    	return(a);
   	}
    upper = haloSettings->getIntRange()[1];
    lower = haloSettings->getIntRange()[0];
    int offset = haloSettings->getOffRange()[0];
    int width = haloSettings->getOffRange()[1];
    QBitmap bm = segmentByThreshold(im, upper, lower);
  	QBitmap dbm = nonMergeDilate(bm, width);
    //QBitmap dbm2 = erode(dbm, 1);
    return(dbm);
    
  } else if(method == "Two channel threshold (non merging)") {
  	
   	int ath1, ath2;
   	ath1 = autoThreshold(im, currentChannel);
   	//ath2 = autoThreshold(im, 1);
    ath2 = 45;
    QBitmap nuc = segmentByThreshold(im, 255, ath1, 2);
    QBitmap nucg = nonMergeDilate(nuc, 100);
    QBitmap cyt = segmentByThreshold(im, 255, ath2, 1);
    QBitmap cytr = xand(nucg, cyt);
    //QBitmap dbm2 = erode(dbm, 1);
    return(cytr);
  } else {
  	return(QBitmap());
 }
}

QBitmap basicSegmentation::segmentByThreshold(QImage* im, int upper, int lower, int ch) {
  int x, y, pRgb, p;
  int channel;
  if (ch < 0) {
  	channel = currentChannel;
  } else {
 	channel = ch;
  }
  QImage m = QImage(im->width(), im->height(), QImage::Format_Mono);
  for(x=0; x<im->width(); x++) {
  	for(y=0;y<im->height(); y++) {
  		pRgb = im->pixel(x,y);
  		if(channel==0) {
  			p = qRed(pRgb);
 		} else if (channel==1) {
 			p = qGreen(pRgb);
	    } else if (channel==2) {
		    p = qBlue(pRgb);
	    }
	    if (p <= upper && p >= lower) {
	  	   m.setPixel(x,y, Qt::color0);
	    } else {
	  	  m.setPixel(x,y, Qt::color1);
	    }
     }
  }
  return(QBitmap::fromImage(m));
}

QBitmap basicSegmentation::nonMergeDilate(QBitmap mm, int count) {
  int x, y, p, p1, p2, p3, p4, p5, p6, p7, p8, n;
  QIntMatrix mx(mm.width(), mm.height());
  QVector<int> values;
  QIntMatrix source = find(mm, 50);
  int uv = source.uniqueCount();
  
  for(n=0; n < count; n++) {  	
    for(x=0; x < mm.width(); x++) {
      mx.set(x, 0, 0);
      mx.set(x, 1, 0);
      mx.set(x, mm.height()-1, 0);	
      mx.set(x, mm.height()-2, 0);	
    }
    for(y=0;y < mm.height(); y++) {
      mx.set(0, y, 0);	
      mx.set(1, y, 0);	
      mx.set(mm.width()-1, y, 0);	
      mx.set(mm.width()-2, y, 0);	
    }
    for(x=2; x < (mm.width()-2); x++) {
      for(y=2;y < (mm.height()-2); y++) {
  		p=source.at(x,y);
  		if(values.size() > 0) {
  		  values.clear();	
        }
	    if(p == 0){  		  	
  		  p1 = source.at(x-1, y);
  		  if (p1 > 0) values << p1;
  		  p2 = source.at(x+1, y);
  		  if (p2 > 0) values << p2;
  		  p3 = source.at(x, y-1);
  		  if (p3 > 0) values << p3;
  		  p4 = source.at(x, y+1);
  		  if (p4> 0) values << p4;
  		  p5 = source.at(x-2, y);
  		  if (p5 > 0) values << p5;
  		  p6 = source.at(x+2, y);
  		  if (p6 > 0) values << p6;
  		  p7 = source.at(x, y-2);
  		  if (p7 > 0) values << p7;
  		  p8 = source.at(x, y+2);
  		  if (p8 > 0) values << p8;
  		  	
  		  if(uniqueValues(values) > 1) {  		  	
	  	     mx.set(x, y, 0);  			      	
 		  } else {
	  	     mx.set(x,y, qMax(qMax(p1, p2), qMax(p3, p4)));
	   	  }  
 		 } else {
	  	   mx.set(x, y, p);  			       		    	
	     }
        }

      }
    for(x=0; x < mm.width(); x++) {
      for(y=0;y < mm.height(); y++) {
      	source.set(x,y, mx.at(x, y));
      }
    }
     //source = mx;
  }  


  QImage r = QImage(mx.width, mx.height, QImage::Format_Mono);
  for(x=0; x < mx.width; x++) {
    for(y=0;y < mx.height; y++) {
    	if(mx.at(x, y) > 0) {
    		r.setPixel(x, y, Qt::color0);
   		} else {
    		r.setPixel(x, y, Qt::color1);   			
  		}
 	}
  }
  QBitmap rb = QBitmap::fromImage(r);
  return(rb);
}


QBitmap basicSegmentation::dilate(QBitmap mm, int count) {
  int x, y, p, p1, p2, p3, p4, n;
  QImage m = mm.toImage();
  p=qRed(m.pixel(1,1));
  for(n=0; n < count; n++) {  	
    QImage gm = QImage(m.width(), m.height(), QImage::Format_Mono);
    for(x=0; x < m.width(); x++) {
      gm.setPixel(x, 0, Qt::color1);	
      gm.setPixel(x, m.width()-1, Qt::color1);	
    }
    for(y=1;y < (m.height()-1); y++) {
      gm.setPixel(0, y, Qt::color1);	
      gm.setPixel(m.height()-1, y, Qt::color1);	
    }
    for(x=1; x < (m.width()-1); x++) {
      for(y=1;y < (m.height()-1); y++) {
  		p=qRed(m.pixel(x,y));
	    if(p == 255){  		  	
  		  p1 = qRed(m.pixel(x-1, y));
  		  p2 = qRed(m.pixel(x+1, y));
  		  p3 = qRed(m.pixel(x, y-1));
  		  p4 = qRed(m.pixel(x, y+1));
  		  if(p1 == 0 || p2 == 0 || p3 == 0 || p4 == 0) {
	  	     gm.setPixel(x,y, Qt::color0);  			      	
 		  } else {
	  	     gm.setPixel(x,y, Qt::color1);  			      
	   	  }  
 		 } else {
	  	   gm.setPixel(x,y, Qt::color0);  			       		    	
	     }
        }
      }
     m = gm;
  }
  return(QBitmap::fromImage(m));
}

QBitmap basicSegmentation::erode(QBitmap mm, int count) {
  int x, y, p, p1, p2, p3, p4, n;
  QImage m = mm.toImage();
  p=qRed(m.pixel(1,1));
  for(n=0; n < count; n++) {  	
    QImage gm = QImage(m.width(), m.height(), QImage::Format_Mono);
    for(x=0; x < m.width(); x++) {
      gm.setPixel(x, 0, Qt::color1);	
      gm.setPixel(x, m.width()-1, Qt::color1);	
    }
    for(y=1;y < (m.height()-1); y++) {
      gm.setPixel(0, y, Qt::color1);	
      gm.setPixel(m.height()-1, y, Qt::color1);	
    }
    for(x=1; x < (m.width()-1); x++) {
      for(y=1;y < (m.height()-1); y++) {
  		p=qRed(m.pixel(x,y));
	    if(p == 0){  		  	
  		  p1 = qRed(m.pixel(x-1, y));
  		  p2 = qRed(m.pixel(x+1, y));
  		  p3 = qRed(m.pixel(x, y-1));
  		  p4 = qRed(m.pixel(x, y+1));
  		  if(p1 == 255 || p2 == 255 || p3 == 255 || p4 == 255) {
	  	     gm.setPixel(x,y, Qt::color1);  			      	
 		  } else {
	  	     gm.setPixel(x,y, Qt::color0);  			      
	   	  }  
 		 } else {
	  	   gm.setPixel(x,y, Qt::color1);  			       		    	
	     }
        }
      }
     m = gm;
  }
  return(QBitmap::fromImage(m));
}



QBitmap basicSegmentation::subtract(QBitmap am, QBitmap bm) {
  int x, y, ap, bp;
  QImage a = am.toImage();
  QImage b = bm.toImage();
  QImage r = QImage(am.width(), am.height(), QImage::Format_Mono);
  for(x=0; x < am.width(); x++) {
    for(y=0;y < am.height(); y++) {
 	  ap=qRed(a.pixel(x,y));
 	  bp=qRed(b.pixel(x,y));
	  if(ap == 0){  		  	
	  	r.setPixel(x, y, Qt::color1);
  	  } else if(bp==0) {
  	  	r.setPixel(x, y, Qt::color0);
 	  } else {
 	  	r.setPixel(x, y, Qt::color1);
	  }
    }
  }
  return(QBitmap::fromImage(r));
}

QBitmap basicSegmentation::xand(QBitmap am, QBitmap bm) {
  int x, y, ap, bp;
  QImage a = am.toImage();
  QImage b = bm.toImage();
  QImage r = QImage(am.width(), am.height(), QImage::Format_Mono);
  for(x=0; x < am.width(); x++) {
    for(y=0;y < am.height(); y++) {
 	  ap=qRed(a.pixel(x,y));
 	  bp=qRed(b.pixel(x,y));
	  if(ap == 0 && bp == 0){  		  	
	  	r.setPixel(x, y, Qt::color0);
  	  } else {
  	  	r.setPixel(x, y, Qt::color1);
 	  } 
    }
  }
  return(QBitmap::fromImage(r));
}


void basicSegmentation::setOptions(const QString &opt, QWidget* parent) {
	return;
}

int basicSegmentation::autoThreshold(QImage* im, int ch) {
  QRgb* pp = (QRgb*)im->bits();
  int p=0, p1, p2, x, y, n1, n2, est1, est2=0, est=120,
		w = im->width(),
		h = im->height(),
		changed=1;

  while (changed == 1) 
  {
	p1 = 1; p2=1; n1=1; n2=1;
	for (y=0; y<h; y++) {
  	  for (x=0; x<w; x++) {
  	  	if (ch == 0) {
  		  p = (int)qRed(pp[y*w+x]);
 		} else if (ch == 1) {
  		  p = (int)qGreen(pp[y*w+x]);
 		} else if (ch == 2) {
  		  p = (int)qBlue(pp[y*w+x]);
 		}
		if (p < est) {
	  	  p1 = p1 + p;
		  n1++;
		} else {
  		  p2 = p2 + p;
		  n2++;
		}
	  }
    }
    p1 = p1/n1;
    p2 = p2/n2;
    
	est1 = (p1 + p2) / 2;
	
	if (est == est1 || est1 == est2) 	{
		changed = 0;
	}
	est2 = est;
	est = est1;
  }

  return (int)est;
}


rangeDialog::rangeDialog( QWidget *parent, Qt::WindowFlags f) 
   : QDialog(parent, f) 
{
  ui.setupUi(this);
  autoTh = 125;
  settings = new QSettings("VARI Epi", "Imagine");	
  int mxi = settings->value("plugins/basicseg/maxint", 255).toInt();
  int mni = settings->value("plugins/basicseg/minint", 0).toInt();

  ui.thresholdSlider->setRange(0,255);
  ui.thresholdSlider->setValue(mni,mxi);
  QObject::connect(ui.thresholdSlider, SIGNAL(minValueChangedString(QString)), ui.min, SLOT(setText(QString)));
  QObject::connect(ui.thresholdSlider, SIGNAL(maxValueChangedString(QString)), ui.max, SLOT(setText(QString)));
  QObject::connect(ui.autoHigh, SIGNAL(toggled(bool)), this, SLOT(autoToggle()));
  QObject::connect(ui.autoLow, SIGNAL(toggled(bool)), this, SLOT(autoToggle()));
  ui.min->setText(QString("%1").arg(mni));
  ui.max->setText(QString("%1").arg(mxi));
  
}

void rangeDialog::setAutoTh(int i) {
	autoTh = i;
}

void rangeDialog::autoToggle() {
	if (sender()->objectName() == "autoHigh" && ui.autoHigh->isChecked()) {
		ui.autoLow->setChecked(false);
		ui.thresholdSlider->setValue(autoTh, 255);
		ui.min->setText(QString("%1").arg(autoTh));
		ui.max->setText(QString("%1").arg(255));
		ui.thresholdSlider->setValue(autoTh, 255);
		ui.thresholdSlider->setEnabled(false);
	} else if (ui.autoLow->isChecked()) {
		ui.autoHigh->setChecked(false);
		ui.thresholdSlider->setValue(0, autoTh);
		ui.min->setText(QString("%1").arg(0));
		ui.max->setText(QString("%1").arg(autoTh));
		ui.thresholdSlider->setValue(0, autoTh);
		ui.thresholdSlider->setEnabled(false);
	} else {
		ui.thresholdSlider->setEnabled(true);
	}
}

QVector<int> rangeDialog::getRange()
{
    settings->setValue("plugins/basicseg/maxint", ui.thresholdSlider->value()[1]);
    settings->setValue("plugins/basicseg/minint", ui.thresholdSlider->value()[0]);
	return(ui.thresholdSlider->value());
}

haloDialog::haloDialog( QWidget *parent, Qt::WindowFlags f) 
   : QDialog(parent, f) 
{
  ui.setupUi(this);
  autoTh = 125;
  ui.thresholdSlider->setRange(0,255);
  ui.thresholdSlider->setValue(0,255);
  QObject::connect(ui.thresholdSlider, SIGNAL(minValueChangedString(QString)), ui.min, SLOT(setText(QString)));
  QObject::connect(ui.thresholdSlider, SIGNAL(maxValueChangedString(QString)), ui.max, SLOT(setText(QString)));
  QObject::connect(ui.autoHigh, SIGNAL(toggled(bool)), this, SLOT(autoToggle()));
  QObject::connect(ui.autoLow, SIGNAL(toggled(bool)), this, SLOT(autoToggle()));
  QObject::connect(ui.haloSlider, SIGNAL(minValueChangedString(QString)), ui.min_2, SLOT(setText(QString)));
  QObject::connect(ui.haloSlider, SIGNAL(maxValueChangedString(QString)), ui.max_2, SLOT(setText(QString)));
}

void haloDialog::setAutoTh(int i) {
	autoTh = i;
}

void haloDialog::autoToggle() {
	if (sender()->objectName() == "autoHigh" && ui.autoHigh->isChecked()) {
		ui.autoLow->setChecked(false);
		ui.thresholdSlider->setValue(autoTh, 255);
		ui.min->setText(QString("%1").arg(autoTh));
		ui.max->setText(QString("%1").arg(255));
		ui.thresholdSlider->setValue(autoTh, 255);
		ui.thresholdSlider->setEnabled(false);
	} else if (ui.autoLow->isChecked()) {
		ui.autoHigh->setChecked(false);
		ui.thresholdSlider->setValue(0, autoTh);
		ui.min->setText(QString("%1").arg(0));
		ui.max->setText(QString("%1").arg(autoTh));
		ui.thresholdSlider->setValue(0, autoTh);
		ui.thresholdSlider->setEnabled(false);
	} else {
		ui.thresholdSlider->setEnabled(true);
	}
}

QVector<int> haloDialog::getIntRange()
{
	return(ui.thresholdSlider->value());
}
 
QVector<int> haloDialog::getOffRange()
{
	return(ui.haloSlider->value());
}
 
QIntMatrix basicSegmentation::find(QBitmap mask, int minsize)
{
  int y, x, a, l, t, i, eqI, eqTI, eqV, w, h, eqMaxI=1, newID = 2;      
  bool done;
  QIntMatrix r(mask.width(), mask.height());
  QImage im = mask.toImage();
  QVector<int> u, eq, counts;
  w = mask.width();
  h = mask.height();
     
  for(y = 0; y < h; y++) {
    for(x = 0; x < w; x++) {
      a = qRed(im.pixel(x, y));
      if(a == 0) {
        r.set(x, y, 1);      	
      } else {
      	r.set(x, y, 0);
      }
    }
  }


  eq =  QVector<int>(2);
  counts =  QVector<int>(2);
  for(y = 1; y < (h - 1); y++) {
    for(x = 1; x < (w - 1); x++) {
      a = r.at(x, y-1);
      l = r.at(x-1, y);
      t = r.at(x, y);
      if (t == 1) {
        if (a == l && a > 1) {
          r.set(x, y, a);
          if(a > (counts.size()-1)) {
          	counts.resize(a+1);
          }
          counts[a]++;
        } else if (a != l && a > 1 && l > 1) {
          r.set(x, y, mathmin(a, l));
          if(mathmin(a, l) > (counts.size()-1)) {
          	counts.resize(mathmin(a, l)+1);
          }
          counts[mathmin(a, l)]++;
          eqI = mathmax(a,l);
          eqV = mathmin(a,l);
          done = false;
          while (!done) {
            if (eqI > eqMaxI) {
              eq.resize(eqI+1);
              eqMaxI = eqI;
            }
            if ((eq.at(eqI) > 1)) {
              if (eqV != eq.at(eqI)) {
                eqTI = eqI;
                eqI = mathmax(eqV, eq.at(eqTI));
                eqV = mathmin(eqV, eq.at(eqTI));
                eq[eqTI] = eqV;
              } else {
                eq[eqI] = eqV;
                done = true;
              }
            } else {
              eq[eqI] = eqV;
              done = true;
            }
          }
        } else if (mathmax(a,l) > 1) {
            r.set(x, y, mathmax(a, l)); 
            if(mathmax(a, l) > (counts.size()-1)) {
            	counts.resize(mathmax(a, l)+1);
            }
            counts[mathmax(a, l)]++;
        } else  {
           r.set(x, y, newID++);
           counts.resize(newID+1);
           counts[newID]=1;
        }
      } else {
      	 r.set(x, y, 0);
      }
    }
  }

  //clean up edges 

  r.replace(1, 0);

  //resolve equivalent ids

  
  if(eqMaxI > 1) 
  {
    resolveEq(&r, eq, counts, minsize);
  }

   return(r);
}

void basicSegmentation::resolveEq(QIntMatrix* m, QVector<int> eq, QVector<int> counts, int minsize)
{
  int i, x, y, p, c;
  for(i=eq.size()-1; i>=0; i--)
  { 
  	if(eq.at(i) > 0) {
  	  c=counts[i];
  	  counts[i] = 0;
  	  bool done = false;
      int ii = eq.at(i);
      while(!done) {
        c += counts.at(ii);
        counts[ii] = 0;  	   	  
  	    if(eq.at(ii) > 0) {
  	   	  ii = eq.at(ii);
 	    } else {
 	      done = true;
	    }
      }
      eq[i] = ii;
      counts[ii] = c;
 	}
  }
  for(y = 0; y < m->height; y++) {
    for(x = 0; x < m->width; x++) {
      p = m->at(x, y);
      if(p > 0 && p < eq.size()) {
       	if(eq.at(p) > 0) {
       	  if(counts.at(eq.at(p)) >= minsize) {       	  	
              m->set(x, y, eq.at(p));
      	  } else {
              m->set(x, y, 0);   
              eq[p] = 0;   	  	
     	  }
        } else if (counts.at(p) < minsize) {
              m->set(x, y, 0);      	  
       	}
      }
    }
  }
}

int uniqueValues(QVector<int> v)
{ 
  int x;
  QVector<int> u;
  if(v.size() <= 0)
  {
  	return(0);
  }
  for(x = 0; x < v.size(); x++) {
      if(!u.contains(v.at(x)))
      {
        u << v.at(x);
      }
  }
  return(u.size());
}

Q_EXPORT_PLUGIN2(basicSeg, basicSegmentation)
