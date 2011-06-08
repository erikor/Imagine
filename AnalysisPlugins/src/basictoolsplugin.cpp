#include "basictoolsplugin.h"

QStringList basicAnalysis::methods(){
//	return QStringList() << "Count Objects" << "Identify & Measure Objects" << "Identify & Measure Sub-Objects"

	return QStringList() << "Count Objects" << "Identify & Measure Objects" << "ROI Area" 
	                     << "Halo Positivity" << "Average IHC Intensity" << "Average Intensity";
}

void basicAnalysis::init(QList<QImage*> imgList, QBitmap* r, QBitmap* m, int i, int c, QWidget* parent){
	images = imgList;
	roi = r;
	mask = m;
	currentIndex = i;
	currentChannel = c;
	app = parent;
}

basicAnalysis::basicAnalysis(){
  progress = new progressWindow();
  progress->setProgress(0);	
  settings = new QSettings("VARI Epi", "Imagine");
}

Results basicAnalysis::analyzeImage(const QString &method)
{
  Results r;
  //QString ms = QString("Width: %1").arg(roi->width());
  //QMessageBox::information(NULL, "Information", ms);
 
  if(method == "Count Objects") {
    int *sz;
	int avsize=0;
    float sdsize=0;
  	int mnd = settings->value("plugins/basic/minsize", 0).toInt();
    int mxd = settings->value("plugins/basic/maxsize", 0).toInt();
 	sizeNpRange = new sizeNpDialog();
  	sizeNpRange->setSizeRange(mnd, mxd);
    sizeNpRange->exec();
    int rr = sizeNpRange->result();
    if (rr==0) {
      return(Results());
   	}
		progress->show();

    int upper = sizeNpRange->getSizeRange()[1];
    int lower = sizeNpRange->getSizeRange()[0];
    settings->setValue("plugins/basic/minsize", lower);
    settings->setValue("plugins/basic/maxsize", upper);

    r.data = ""; 
    int i;

    QIntMatrix result = find(*mask, *roi, lower, upper);    
 	QVector<int> uv;
    uv = result.uniqueValues();
    r.data.append(QString("<p>%1 objects identified in file %2<p>").arg(uv.size()-1).arg(i));     
	if(uv.size() < 2) {
	  return(r);
	}

    int n=0;
    int ti=0;
    int x, y;
    QImage ri = roi->toImage();
    QImage m = mask->toImage();
    int a=0;
    int p;
	int obj = 0;
	progress->setMessage("Measuring Objects...");
    progress->setProgress(0);
    progress->setProgressRange(0,m.height());
    QImage* im = images.at(currentIndex);
    for(y=0; y<m.height(); y++) {
	progress->setProgress(y);
      for(x=0; x<m.width(); x++) {
   		if(currentChannel == 0) {
    			p = qRed(im->pixel(x,y));
   		} else if(currentChannel == 1){
    			p = qGreen(im->pixel(x,y));
   		} else {
   			p = qBlue(im->pixel(x,y));
   		}
   		if (qRed(ri.pixel(x,y)) > 0 && qRed(m.pixel(x,y)) == 0) {
   			ti += p;
 			n++;
 			a++;
  		} else if(qRed(ri.pixel(x,y)) > 0) {
  			a++;
     	}
		if(result.at(x,y) > 0) {
		  obj++;
		}
      }
   	} 
	progress->setMessage("Calculating SD...");
    progress->setProgress(0);
    progress->setProgressRange(0,uv.size()*2);

	  sz = (int*)malloc(uv.size() * sizeof(int));
	  for(i=1; i<uv.size(); i++) {
        progress->setProgress(i);
        sz[i] = result.in(uv.at(i));	    
		avsize += sz[i];
	  }
	  avsize = avsize / (uv.size()-1);
	  for(i=1; i<uv.size(); i++) {
        progress->setProgress(uv.size()+i);
	    sdsize += (sz[i]-avsize)^2;
	  }
	  sdsize = (int)sqrt(sdsize);
	  progress->hide();
      r.data.append(QString("<p>Average Size: %1, Size SD: %2, Average intensity: %3, Positive Pixel Count: %4, Roi Area: %5").arg(avsize).arg(sdsize).arg(ti/n).arg(n).arg(a));

  } else if(method == "Average Intensity") {
    r.data = ""; 
    int i;
    QImage* im = images.at(currentIndex);
    int n=0;
    int ti=0;
    int x, y;
    QImage ri = roi->toImage();
    QImage m = mask->toImage();
    int a=0;
    int p;
    for(y=0; y<m.height(); y++) {
      for(x=0; x<m.width(); x++) {
   		if(currentChannel == 0) {
   			p = qRed(im->pixel(x,y));
   		} else if(currentChannel == 1){
   			p = qGreen(im->pixel(x,y));
   		} else {
   			p = qBlue(im->pixel(x,y));
   		}
   		if (qRed(ri.pixel(x,y)) > 0 && qRed(m.pixel(x,y)) == 0) {
   			ti += p;
 			n++;
 			a++;
  		} else if(qRed(ri.pixel(x,y)) > 0) {
  			a++;
     	}
      }
   	} 
   	if(n > 0) {
	    r.data.append(QString("<p>Average intensity: %1, Positive Pixel Count: %2, Roi Area: %3").arg(ti/n).arg(n).arg(a));   		
  	} else {
	    r.data.append(QString("<p>Average intensity: NA, Positive Pixel Count: 0, Roi Area: %3").arg(a));
 	}
  } else if(method == "Average IHC Intensity") {
  	int x, y, i, p=0, ti, n=0, th;
    r.data = ""; 
    QImage m = roi->toImage();
    QImage* im = images.at(currentIndex);
     for(y=0; y<im->height(); y++) {
     	for(x=0; x<im->width(); x++) {
     		if(qRed(m.pixel(x,y)) > 0) {
     			p += 255-qGreen(im->pixel(x,y));
     			n++;
   			}
    	}
   	 } 
     r.data.append(QString("<p>Average IHC staining intensity:  %2<p>").arg(p/n));
  } else if(method == "Identify & Measure Objects") {
  	int mnd = settings->value("plugins/basic/minsize", 0).toInt();
    int mxd = settings->value("plugins/basic/maxsize", 0).toInt();
 	sizeNpRange = new sizeNpDialog();
  	sizeNpRange->setSizeRange(mnd, mxd);
    sizeNpRange->exec();
    int rr = sizeNpRange->result();
    if (rr==0) {
      return(Results());
   	}
    int upper = sizeNpRange->getSizeRange()[1];
    int lower = sizeNpRange->getSizeRange()[0];
    settings->setValue("plugins/basic/minsize", lower);
    settings->setValue("plugins/basic/maxsize", upper);
     
  	QIntMatrix result = find(*mask, *roi, lower, upper, true);    

  	QString res = "<table border=1 cellpadding=5><tr><td>Object ID</td><td>Size</td><td>Total intensity</td><td>Average intensity</td></tr>";
  	QVector<int> uv;
  	uv = result.uniqueValues();
  	r.annotations = annotate(&result);
  	
  	//r.annotations = ;

  	for(int i=1; i<uv.size(); i++) {
      	qDebug() << i;
  	    res.append("<tr><td>");
        res.append(QString("%1").arg(i));
        res.append("</td>");
        int v = uv.at(i);
        QRectF rf = regions.at(i-1);
        QPointF p = rf.topLeft();
        p.setY(p.y()-18);
        QGraphicsTextItem* ti = new QGraphicsTextItem(QString("%1").arg(i));
        ti->setPos(p);
        r.annotations << ti;
        res.append(stats(&result, images.at(currentIndex), v, currentChannel));
        res.append("</tr>");
    }
    qDebug() << "ok";
    res.append("</table>");
    QList<QVariant*> rv;
    r.data = res;

  } else if(method == "ROI Area") {
    QImage m = roi->toImage();
    int x, y, a=0;
    for(y=0; y < m.height(); y++) {
      for(x=0; x<m.width(); x++) {
    	if(qRed(m.pixel(x,y)) > 0) {
    		a++;
   		}
  	  }
   	}
   	r.data = QString("<br>ROI Area: %1<br>").arg(a);
  } else if(method == "Identify & Measure Sub-Objects") {

    int mnd = settings->value("plugins/basic/minsize", 0).toInt();
    int mxd = settings->value("plugins/basic/maxsize", 0).toInt();
    int mni = settings->value("plugins/basic/minint", 0).toInt();
    int mxi = settings->value("plugins/basic/maxint", 255).toInt();
 	sizeRange = new sizeDialog();
  	sizeRange->setSizeRange(mnd, mxd);
  	sizeRange->setIntRange(mni, mxi);
  	sizeRange->setPreview(images.at(currentIndex));
    sizeRange->exec();

    int rr = sizeRange->result();

    if (rr==0) {
      return(Results());
   	}

    int szUpper = sizeRange->getSizeRange()[1];
    int szLower = sizeRange->getSizeRange()[0];
    int intUpper = sizeRange->getIntRange()[1];
    int intLower = sizeRange->getIntRange()[0];
    int ch = sizeRange->getChannel();

    settings->setValue("plugins/basic/minsize", szLower);
    settings->setValue("plugins/basic/maxsize", szUpper);
    settings->setValue("plugins/basic/minint", intLower);
    settings->setValue("plugins/basic/maxint", intUpper);

    QBitmap m = xand(*roi, *mask);
    QImage *im = images.at(currentIndex);
   	int ath = autoThreshold(im, 1);
	QBitmap cyt = segmentByThreshold(im, intUpper, intLower, ch);
  	QIntMatrix result = find(cyt, m, szLower, szUpper);    

  	QString res = "<table border=1 cellpadding=5><tr><td>Object ID</td><td>Size</td><td>Total intensity</td><td>Average intensity</td></tr>";
  	QVector<int> uv;
  	uv = result.uniqueValues();
  	//r.annotations = annotate(&result);

  	for(int i=1; i<uv.size(); i++) {
  	  res.append("<tr><td>");
      res.append(QString("%1").arg(uv.at(i)));
      res.append("</td>");
      int v = uv.at(i);
      QRectF rf = regions.at(i-1);
      QPointF p = rf.topLeft();
      p.setY(p.y()-12);
      QGraphicsTextItem* ti = new QGraphicsTextItem(QString("%1").arg(i));
      ti->setPos(p);
      r.annotations << ti;
      res.append(stats(&result, images.at(currentIndex), v, currentChannel));
      res.append("</tr>");
      QPen pen = QPen(Qt::SolidLine);
      pen.setColor(Qt::blue);
      pen.setWidth(1);
      QGraphicsRectItem* rectangle = new QGraphicsRectItem (regions.at(i-1));
      rectangle->setPen(pen);
      rectangle->setBrush(QBrush(QColor(0, 0, 100, 0)));
      r.annotations << rectangle;
    }
    res.append("</table>");
    QList<QVariant*> rv;
    r.data = res;
  } else if(method == "Halo Positivity") {

    QBitmap m = xand(*roi, *mask);
    QImage *im = images.at(currentIndex);
   	int ath = autoThreshold(im, 1, 2);

    int tsize=0;    
    int x, y;
    QImage imm = m.toImage();
    int size=1, total=0, avg=0; 
    for(y = 0; y < m.height(); y++) {
      for(x = 0; x < m.width(); x++) {
  	    if(qRed(imm.pixel(x,y)) > 0 && qGreen(im->pixel(x,y)) > ath) {
          size++;
          tsize++;
          total += qGreen(im->pixel(x,y));
         } else if(qRed(imm.pixel(x,y)) > 0) {
          tsize++;
        }
      }
    }
    tsize = tsize/2;
    avg = (int)((float)total / (float)tsize);
  	QString res = QString("<p>Halo positivity: %1</p>").arg((float)size/(float)tsize);
  	//<table border=1 cellpadding=5><tr><td> </td><td>Positive Pixels</td><td>Total Halo Pixels</td><td>Total intensity</td><td>Average intensity</td></tr>";
    //QString rv = QString("<tr><td>Halo Averages</td><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>").arg(size).arg(tsize).arg(total).arg(avg);  
    //res.append(rv);
    //res.append("</table>");
    r.data = res;
 }
  return(r);     
}
QIntMatrix basicAnalysis::find(QBitmap mask, QBitmap roi, int minsize, int maxsize, bool annotate)
{
  int y, x, a, l, t, i, eqI, eqTI, eqV, w, h, eqMaxI=1, newID = 2;      
  bool done;
  QIntMatrix r(mask.width(), mask.height());
  QImage im = mask.toImage();
  QImage rm = roi.toImage();
  qDebug() << rm.height() << rm.width();
  QVector<int> u, eq, counts;
  w = mask.width();
  h = mask.height();
  qDebug() << qRed(rm.pixel(1,1));
  progress->setMessage("Finding Objects...");
  progress->setProgressRange(0,h);
     
  for(y = 0; y < h; y++) {
    for(x = 0; x < w; x++) {
      a = qRed(im.pixel(x, y));
      l = qRed(rm.pixel(x,y));
      if(a == 0 && l == 255) {
        r.set(x, y, 1);      	
      } else {
      	r.set(x, y, 0);
      }
    }
  }

  eq =  QVector<int>(2);
  counts =  QVector<int>(2);
  for(y = 1; y < (h - 1); y++) {
    progress->setProgress(y);
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
          if (a > eqMaxI) {
            eq.resize(a+1);
            eqMaxI = a;
          }
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
            if (mathmax(a, l) > eqMaxI) {
              eq.resize(mathmax(a, l)+1);
              eqMaxI = mathmax(a, l);
            }
            counts[mathmax(a, l)]++;
        } else  {
           r.set(x, y, newID++);
           counts.resize(newID+1);
           counts[newID]=1;
        }
        if (newID > eqMaxI) {
              eq.resize(newID+1);
              eqMaxI = newID;
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
    resolveEq(&r, eq, counts, minsize, maxsize);
  }

  //clean up...recode connected components with consecutive integers
  if(annotate) {   	
    u = r.uniqueValues();                   
    regions.clear();
    newID = 1;
	progress->setMessage("Cleaning up ids...");
    progress->setProgress(0);
    progress->setProgressRange(0,u.size());

    for (i=1; i<u.size(); i++) 
    {
       progress->setProgress(i); 
       regions << r.replace(u.at(i), newID);
       newID++;
    }
   }
   return(r);
}

void basicAnalysis::resolveEq(QIntMatrix* m, QVector<int> eq, QVector<int> counts, int minsize, int maxsize)
{
  int i, x, y, p, c;
  progress->setMessage("Resolving Objects...");
  progress->setProgressRange(0,eq.size()+m->height);
  progress->setProgress(0);

  for(i=eq.size()-1; i>=0; i--)
  { 
    progress->setProgress(eq.size()-i);
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
    progress->setProgress(eq.size()+y);
    for(x = 0; x < m->width; x++) {
      p = m->at(x, y);
      if(p > 0 && p < eq.size()) {
       	if(eq.at(p) > 0) {
       	  if(counts.at(eq.at(p)) >= minsize && counts.at(eq.at(p)) <= maxsize) {       	  	
              m->set(x, y, eq.at(p));
      	  } else {
              m->set(x, y, 0);   
              eq[p] = 0;   	  	
     	  }
        } else if (counts.at(p) < minsize || counts.at(p) > maxsize) {
              m->set(x, y, 0);      	  
       	}
      }
    }
  }
}

void dumpMatrix(QIntMatrix* m)
{
	int w = m->width;
	int h = m->height;
	int x,y;
	QStringList line;
	for(y=0; y<h; y++) {
		line = QStringList();
		for(x=0; x<w; x++) {
			line << QString("%1").arg(m->at(x,y));
		}
		qDebug() << line.join(" ");
	}
	qDebug() << "Done";
}

QList<QGraphicsItem*> basicAnalysis::annotate(QIntMatrix* r)
{
	QList<QGraphicsItem*> rv;
    QVector<int> u = r->uniqueValues();                   
    QPen pen = QPen(Qt::SolidLine);
    int i;
    float ii;
    ii = (float)r->height / (float)regions.size();
    pen.setColor(Qt::blue);
    pen.setWidth(1);
    
    for(i=0; i<regions.size(); i++)
   	{   		
       QGraphicsRectItem* rectangle = new QGraphicsRectItem (regions.at(i));
       progress->setMessage(QString("Annotating item %1 of %2").arg(i+1).arg(regions.size()));
       rectangle->setPen(pen);
       rectangle->setBrush(QBrush(QColor(0, 0, 100, 0)));
       rv << rectangle;       
    }
    return(rv);
}


QString basicAnalysis::stats(QIntMatrix *m, QImage *i, int v, int ch)
{ 
  int x, y;
  int size=1, total=0, avg=0;
  for(y = 0; y < m->height; y++) {
    for(x = 0; x < m->width; x++) {
      if (m->at(x, y) == v) {
        size++;
        if(ch == 0) {
          total += qRed(i->pixel(x,y));        	
       	} else if(ch == 1) {
          total += qGreen(i->pixel(x,y));        	
       	} else {
          total += qBlue(i->pixel(x,y));        	
       	}
      }
    }
  }
  avg = (int)((float)total / (float)size);
  QString rv = QString("<td>%1</td><td>%2</td><td>%3</td>").arg(size).arg(total).arg(avg);
  return(rv);
}

void progressWindow::setProgress(int v) {
	ui.progress->setValue(v);
	repaint();
}

void progressWindow::setProgressRange(int f, int t){
  ui.progress->setRange(f, t);	
}

void progressWindow::resetProgress(){	
  ui.progress->reset();
}

void progressWindow::setMessage(QString s){
  ui.message->setText(s);	
  repaint();
}

progressWindow::progressWindow( QWidget *parent, Qt::WindowFlags f) 
   : QDialog(parent, f) 
{
  ui.setupUi(this);
}

QBitmap basicAnalysis::segmentByThreshold(QImage* im, int upper, int lower, int ch) {
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



QIntMatrix basicAnalysis::find(QBitmap mask, int minsize, int maxsize)
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
          if (a > eqMaxI) {
            eq.resize(a+1);
            eqMaxI = a;
          }
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
            if (mathmax(a, l) > eqMaxI) {
              eq.resize(mathmax(a, l)+1);
              eqMaxI = mathmax(a, l);
            }
            counts[mathmax(a, l)]++;
        } else  {
           r.set(x, y, newID++);
           counts.resize(newID+1);
           counts[newID]=1;
        }
        if (newID > eqMaxI) {
              eq.resize(newID+1);
              eqMaxI = newID;
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
    resolveEq(&r, eq, counts, minsize, maxsize);
  }

   return(r);
}

int basicAnalysis::autoThreshold(QImage* im, int ch, int bias) {
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

  if(bias == -1) {
  	est = (est + p1) / 2;
  } else if(bias == -2) {
 		est = p2;
  } else if(bias == 1) {
 		est = (est + p2) / 2;
  } else if(bias == 2) {
 		est = p2;
  }
  
  return (int)est;
}

sizeDialog::sizeDialog( QWidget *parent, Qt::WindowFlags f) 
   : QDialog(parent, f) 
{
  ui.setupUi(this);
  scene = new QGraphicsScene(0,0,512,512);
  ui.preview->setScene(scene);
  QObject::connect(ui.thresholdSlider, SIGNAL(valueChanged()), this, SLOT(updatePreview()));
  maskItem = NULL;
}

void sizeDialog::updatePreview() {
  int min = ui.thresholdSlider->value()[0];
  int max = ui.thresholdSlider->value()[1];
  QBitmap bitmask = segmentByThreshold(image, max, min, 1);
  QPixmap mask = QPixmap(image->width(), image->height());
  mask.fill(Qt::red);
  mask.setMask(bitmask);
  if(maskItem) {
  	scene->removeItem(maskItem);
  }
  maskItem = scene->addPixmap(mask);
  maskItem->setZValue(100);
  maskItem->show();
}

void sizeDialog::setPreview(QImage* im) {
	image = im;
	QPixmap pm;
	pm = QPixmap::fromImage(*im);
	scene->addPixmap(pm);
}

QVector<int> sizeDialog::getSizeRange()
{
	QVector<int> r;
	r << ui.minSize->value();
	r << ui.maxSize->value();
	return(r);
}

QVector<int> sizeDialog::getIntRange() {
	return(ui.thresholdSlider->value());
}

int sizeDialog::getChannel()
{
  if(ui.Red->isChecked()) 
  	return(0);
  if(ui.Green->isChecked())
	return(1);
  if(ui.Blue->isChecked())
	return(2);
  return(0);
}

void sizeDialog::setSizeRange(int mn, int mx) {
	ui.minSize->setValue(mn);
	ui.maxSize->setValue(mx);
}

void sizeDialog::setIntRange(int mn, int mx) {
	ui.thresholdSlider->setValue(mn, mx);
}



sizeNpDialog::sizeNpDialog( QWidget *parent, Qt::WindowFlags f) 
   : QDialog(parent, f) 
{
  ui.setupUi(this);
}

QVector<int> sizeNpDialog::getSizeRange()
{
	QVector<int> r;
	r << ui.minSize->value();
	r << ui.maxSize->value();
	return(r);
}


void sizeNpDialog::setSizeRange(int mn, int mx) {
	ui.minSize->setValue(mn);
	ui.maxSize->setValue(mx);
}




QBitmap basicAnalysis::xand(QBitmap am, QBitmap bm) {
  int x, y, ap, bp;
  QImage a = am.toImage();
  QImage b = bm.toImage();
  QImage r = QImage(am.width(), am.height(), QImage::Format_Mono);
  for(x=0; x < am.width(); x++) {
    for(y=0;y < am.height(); y++) {
 	  ap=qRed(a.pixel(x,y));
 	  bp=qRed(b.pixel(x,y));
	  if(ap == 255 && bp == 0){  		  	
	  	r.setPixel(x, y, Qt::color1);
  	  } else {
  	  	r.setPixel(x, y, Qt::color0);
 	  } 
    }
  }
  return(QBitmap::fromImage(r));
}

QBitmap sizeDialog::segmentByThreshold(QImage* im, int upper, int lower, int ch) {
  int x, y, pRgb, p;
  int channel = ch;
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

Q_EXPORT_PLUGIN2(analysisPlugins, basicAnalysis)


// SAVE AN IMAGE
    //QImage save = image->getImage().toImage();
    //QPainter* painter = new QPainter(&save);
    //QRectF target(0, 0, result.width(), result.height());
    //QRectF source(0, 0, result.width(), result.height());
    //painter->drawPixmap(target, QPixmap::fromImage(result), source);
    //painter->end();
    //save.save("c:/result.jpg");
