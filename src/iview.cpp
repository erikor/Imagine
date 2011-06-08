#include "iview.h"

iView::iView(QWidget *parent)  : QGraphicsView(parent)
{
	roiRect = NULL;
	opencount=0;
	roiFree = NULL;
    this->cursor().setShape(Qt::CrossCursor);
    scene = new QGraphicsScene();
    this->setScene(scene);
    this->setAlignment(Qt::AlignCenter);
    this->show();
    select = RECTSELECT;
    filter = LOWPASS;
    imageLoaded = false;
    useAutoThreshold = true;
    roiLocked = false;
    maskLocked = false;
    flatField = false;
    mask = QPixmap();
    pixmap=NULL;
    annotationGroup.clear();
    channel = 0;
    settings = new QSettings("VARI Epi", "Imagine");
    haveimage = false;
    setFont();
    setColor();
    r_ch = true;
    g_ch = true;
    b_ch = true;

}

void iView::mousePressEvent(QMouseEvent *e) {
  if (e->buttons() == Qt::LeftButton && imageLoaded && !roiLocked) {
  	 if(!roiRect) {   	 	
        QPen pen = QPen(Qt::DashDotLine);
        pen.setColor(Qt::red);
        pen.setWidth(2);
        roiRect = scene->addRect(QRectF(0, 0, 0, 0), pen, QBrush(color));
        roiPoly = QPolygonF();
        roiFree = scene->addPolygon(roiPoly, pen, QBrush(color));
 	 }
     
     roiPoly.clear();
  	 roiRect->hide();
  	 roiFree->hide();
	 roiStart = mapToScene(e->x(), e->y());
     roiRect->setRect(QRectF(roiStart, QSize(0,0)));
     roiPoly.append(roiStart);
     roiRect->setZValue(10);
     roiFree->setZValue(10);
  }
}

void iView::mouseReleaseEvent(QMouseEvent *e) {
  if (imageLoaded && !roiLocked) {
	roiPoly.append(roiStart);  
    QRectF brect = roiFree->boundingRect();
    roiFreeImage = new QImage(image.width(), image.height(), QImage::Format_Mono);
    QPainter* painter = new QPainter(roiFreeImage);
    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);
    painter->drawPolygon(roiFree->polygon());
    painter->end();
 	emit roiChanged(select);
  }
}

void iView::mouseMoveEvent(QMouseEvent *e) {
  if (e->buttons() == Qt::LeftButton && imageLoaded && !roiLocked) {
	int x, y;
    int w = image.width();
    int h = image.height();
	if (e->buttons() == Qt::LeftButton) {
	   QPointF p = mapToScene(e->x(), e->y());	   
	   x = (int)p.x();
	   y = (int)p.y();
	   if(y < 1)
	   	   	y = 1;
	   if(x < 1)
	   	   	x = 1;
	   if(y >= h)
	   	   	y = h-1;
	   if(x >= w)
	   	   	x = w-1;

       roiPoly.append(QPoint(x,y));
       roiFree->setPolygon(roiPoly);
       if(select == FREESELECT) {
  	     roiFree->show();	
  	     roiRect->hide();	
       } else {
  	     roiFree->hide();	
  	     roiRect->show();	
       }
	   QSizeF size = QSizeF(x - roiStart.x(), y - roiStart.y());
       roiRect->setRect(QRectF(roiStart, size));
	} 
  }
  QGraphicsView::mouseMoveEvent(e);
}
 
void iView::reset() {
	roiFree = NULL;
	roiRect = NULL;
}

void iView::zoom(int z)
{
    float sf = ((float)z + 1.0)/100.0;
    this->resetMatrix(); 
    this->scale(sf, sf);
	return;
}

void iView::openImages() {
  analyzer->openImages();
  	if(pixmap) {
 		scene->removeItem(pixmap);
 		delete(pixmap);
	}

    clearAnnotations();
    if (!roiLocked)
    {
    	if(roiRect)
    		scene->removeItem(roiRect);
    		roiRect=NULL;
   		if(roiFree)
   			scene->removeItem(roiFree);    			
    		roiFree=NULL;
   	}

    image = analyzer->getCurrentImage(r_ch, g_ch, b_ch);  
    if(flatField)
   	    flatFieldAdjust();
    QPixmap pm = QPixmap::fromImage(*image);
    pixmap = scene->addPixmap(pm);	 
    pixmap->setZValue(0);

    this->resetMatrix(); 
    this->setSceneRect(pixmap->mapToParent(pixmap->boundingRect()).boundingRect());
    pixmap->ensureVisible(pixmap->boundingRect());

	if(!roiRect) {   	 	
        QPen pen = QPen(Qt::DashDotLine);
        pen.setColor(Qt::red);
        pen.setWidth(1);
        roiRect = scene->addRect(QRectF(0, 0, 0, 0), pen, QBrush(color));
        roiPoly = QPolygonF();
        roiFree = scene->addPolygon(roiPoly, pen, QBrush(color));
 	}
 	
    imageLoaded = true;
    if(useAutoThreshold)
    	threshold = autoThreshold(&image, channel);

    if(filter == HIGHPASS && !maskLocked) {
	 highPass();
    }  else if(!maskLocked){
  	 lowPass();
    } else if(maskLocked) {
  	   roiFree->setBrush(mask);
       roiRect->setBrush(mask);
   	}
  }
  return;
}

int iView::zoomAll() {
	qreal zf, zw, zh;
	zw = 100.0 * (qreal)(this->width()) / (qreal)image.width();
	zh = 100.0 * (qreal)(this->height()) / (qreal)image.height();		
	
	if(zw < zh) {
		zf = zw;
	} else {
		zf = zh;
	}
	
	//add a little padding to ensure no scroll bars
    zf -= 10;
  	this->zoom((int)zf);	
  	return((int)zf);
}


void iView::switchImage(int index)
{
	if(index < 0)
		index = 0;
	analyzer->setCurrentImage(index);
  	if(pixmap) {
 		scene->removeItem(pixmap);
 		delete(pixmap);
	}
    clearAnnotations();       	
    image = analyzer->getCurrentImage(r_ch, g_ch, b_ch);
    if(flatField)
   	    flatFieldAdjust();

    QPixmap pm =  QPixmap::fromImage(*image);
    pixmap = scene->addPixmap(pm);	 
    pixmap->setZValue(0);
    pixmap->show();
 	
    if(useAutoThreshold)
       threshold = autoThreshold(&image, channel);

    if(filter == HIGHPASS && !maskLocked) {
	 highPass();
    }  else if(!maskLocked){
  	 lowPass();
    } else if(maskLocked) {
  	   roiFree->setBrush(mask);
       roiRect->setBrush(mask);
   	}  
   	
  return;	
}

void iView::openFlatField(void){
  QString fn = QFileDialog::getOpenFileName(this, 
                          tr("Open File"),
                          "c:/",
                          tr("Images (*.tif; *.TIFF)"));
  if(fn.size() == 0) 
   	return;
  flatFieldImage = loadTiff(fn);
  if(pixmap) {
	scene->removeItem(pixmap);
    clearAnnotations();       	  
    flatFieldAdjust();
    pixmap = scene->addPixmap(QPixmap::fromImage(image));

    if(filter == HIGHPASS && !maskLocked) {
  	  highPass();
    }  else if(!maskLocked){
  	  lowPass();
    }  
  }
  flatField = true;
}
void iView::lowPass() {
  if(!imageLoaded) 
  	return;

  int w = image.width();
  int h = image.height();
  int x, y, r=0;

  QImage m = QImage(w, h, QImage::Format_Mono);
  m.fill(Qt::color1);
  
  QRgb p;
  
  for(y=0; y < h; y++) {
    for(x=0; x < w; x++) {
      	p = image.pixel(x,y);
      	if (channel == RED) {
      		r = qRed(p);	 
   		} else if(channel == GREEN){
      		r = qGreen(p);	 
   		} else if(channel == BLUE){
      		r = qBlue(p);	    			
		}
         
        if (r < threshold) {
           m.setPixel(x, y, Qt::color0);      	
        }
 	}
  }

  mask = QPixmap(w, h);
  mask.fill(color);
  mask.setMask(QBitmap::fromImage(m));
  roiFree->setBrush(mask);
  roiRect->setBrush(mask);
  emit roiChanged(select);
  return;
}

void iView::highPass() {
  if(!imageLoaded) 
  	return;
  	
  int w = image.width();
  int h = image.height();
  int x, y, r=0;

  QImage m = QImage(w, h, QImage::Format_Mono);
  m.fill(Qt::color1);
  
  QRgb p=QRgb();
  
  for(y=0; y < h; y++) {
    for(x=0; x < w; x++) {
      	p = image.pixel(x,y);
      	if (channel == RED) {
      		r = qRed(p);	 
   		} else if(channel == GREEN){
      		r = qGreen(p);	 
   		} else if(channel == BLUE){
      		r = qBlue(p);	    			
		}
        if (r > threshold) {
           m.setPixel(x, y, Qt::color0);      	
        }
 	}
  }

  mask = QPixmap(w, h);
  mask.fill(color);
  mask.setMask(QBitmap::fromImage(m));
  roiFree->setBrush(mask);
  roiRect->setBrush(mask);
  emit roiChanged(select);
  return;
}


int min(int a, int b) {
  if (a < b)
  	 return a;
  return b;
}


int iView::autoThreshold(QImage* im, int ch) {
  QRgb* pp = (QRgb*)im->bits();
  int p=0, p1, p2, x, y, n1, n2, est1, est2=0, est=60,
		w = im->width(),
		h = im->height(),
		changed=1;

  while (changed == 1) 
  {
	p1 = 1; p2=1; n1=1; n2=1;
	for (y=0; y<h; y++) {
  	  for (x=0; x<w; x++) {
  	  	if (ch == RED) {
  		  p = (int)qRed(pp[y*w+x]);
 		} else if (ch == GREEN) {
  		  p = (int)qGreen(pp[y*w+x]);
 		} else if (ch == BLUE) {
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

void iView::setRectSelect() {
	select=RECTSELECT;
    if(!imageLoaded) 
     	return;
     	
	if(roiFree) {
		roiFree->hide();
	}
}

void iView::setFreeSelect() {
	select=FREESELECT;
    if(!imageLoaded) 
    	return;
    	
	if(roiRect) {
		roiRect->hide();
	}
}


void iView::setHighPass() {
	filter=HIGHPASS;
    if(!imageLoaded || maskLocked) 
    	return;
	highPass();
}

void iView::setLowPass() {
	filter=LOWPASS;
    if(!imageLoaded || maskLocked) 
    	return;
	lowPass();
}


QRectF iView::getRectRoi(){
  return(roiRect->rect());	
}

QPolygonF iView::getFreeRoi(){
  return(roiFree->polygon());	
}


QBitmap iView::getRoi(){
  QBitmap mask = QBitmap(image.width(), image.height());
  mask.fill(Qt::color0);
  QPainter* painter = new QPainter(&mask);
  painter->setPen(Qt::color1);
  painter->setBrush(Qt::color1);
  if (select == FREESELECT) {
    painter->drawPolygon(roiFree->polygon());
  } else {
    painter->drawRect(roiRect->rect());
  }
  painter->end();
  return(mask);	
}


QBitmap iView::getBitmask(){
  QBitmap mask;
  if (select == FREESELECT) {
    mask = roiFree->brush().texture().mask();
  } else {
    mask = roiRect->brush().texture().mask();
  }
  return(mask);	
}

QImage* iView::getImage() {
  return(image);
}


void iView::setAutoThreshold(bool th) {
  useAutoThreshold = th;
  if(th) {
  	threshold = autoThreshold(&image, channel);
    if(filter == HIGHPASS  && !maskLocked) {    	
	  highPass();
   	} else if(!maskLocked){
   	  lowPass();
    }  	
  }
}


void iView::setThreshold(int th) {
	threshold=th;
    if(!imageLoaded || maskLocked) 
    	return;
    if(filter == HIGHPASS) {    	
	  highPass();
   	} else {
   	  lowPass();
    }
}


int iView::getThreshold() {
	return(threshold);
}

QStringList iView::getFileNames() {
	return(files);
}

void iView::setRoiLocked(bool l)
{
	roiLocked = l;
}

void iView::setMaskLocked(bool locked)
{
	maskLocked = locked;
	if(!locked) {
      if(filter == HIGHPASS) {    	
	    highPass();
   	  } else {
   	    lowPass();
      }		
    }
}

bool iView::isRoiLocked()
{
	return(roiLocked);
}

bool iView::isMaskLocked()
{
	return(maskLocked);
}

QGraphicsScene* iView::getScene(){
	return(scene);
}

void iView::addAnnotationItem(QGraphicsItem* item) {
  scene->addItem(item);
  item->setZValue(110);
  item->show();
  annotationGroup << item;
}

void iView::addAnnotationText(QString s, QPoint p) {
  QGraphicsTextItem* ti = scene->addText(s);
  ti->setFont(font);
  ti->setPos(p);
  ti->setDefaultTextColor(color);	
  ti->setZValue(200);
  annotationGroup << ti;
}

void iView::clearAnnotations(){	
	QGraphicsItem* item;
    if(annotationGroup.size() > 0) {
    	foreach(item, annotationGroup) {
    		scene->removeItem(item);
   		}
    	annotationGroup.clear();
   	}
}

void iView::setChannel(int ch){
	channel=ch;
    if(useAutoThreshold)
		threshold=autoThreshold(&image, ch);

	if(!maskLocked) {
      if(filter == HIGHPASS) {    	
	    highPass();
   	  } else {
   	    lowPass();
      }		
    }

}

int iView::getChannel() {
	return(channel);
}

void iView::printImage() {
 QPrinter printer(QPrinter::HighResolution);
 QPrintDialog printDialog(&printer);
 if (printDialog.exec() == QDialog::Accepted) {
   QPainter* painter = new QPainter(&printer);
   this->render(painter);
   painter->end();
 }	
}

void iView::saveImage() {
  QString dir = settings->value("lastdir", QString()).toString();

  if (dir.isNull())
  	dir = "/";

  QString fn = QFileDialog::getSaveFileName(this, 
                          tr("Save File"),
                          dir,
                          tr("Images (*.jpg; *.jpeg)"));

  if(fn.size() == 0)
  	return;
  QImage save = image;
  QPainter* painter = new QPainter(&save);
  this->render(painter);
  //QRectF target(0, 0, imageult.width(), result.height());
  //QRectF source(0, 0, result.width(), result.height());
  //painter->drawPixmap(target, QPixmap::fromImage(result), source);
  painter->end();
  save.save(fn, 0, 100);	
}

void iView::chooseColor() {
	
  QColor col = QColorDialog::getColor ();
  if(col.isValid()) {
    settings->setValue("color/red", col.red());
    settings->setValue("color/green", col.green());
    settings->setValue("color/blue", col.blue());  	
    color = col;
  }
}

void iView::chooseFont() {
 bool ok;
 QFont f = QFontDialog::getFont(&ok, QFont("Arial", 10), this);
 if (ok) {
 	font = f;
    settings->setValue("font/bold", font.bold());
    settings->setValue("font/italic", font.italic());
    settings->setValue("font/family", font.family());
    settings->setValue("font/pointsize", font.pointSize());
 }
}

void iView::setFont() {
  int s = settings->value("font/pointsize", 0).toInt();
 if(s == 0) {
    font = QFont("Arial", 10);
    return;  	
  }
  font = QFont();
  font.setPointSize(s);
  font.setBold(settings->value("font/bold", 0).toBool());
  font.setItalic(settings->value("font/italic", 0).toBool());
  font.setFamily(settings->value("font/family", 0).toString());
}


void iView::setColor() {
 int r = settings->value("color/red", -1).toInt();
 if(r == -1) {
    color = QColor(255,0,0);
    return;  	
  }
  int g = settings->value("color/green", 0).toInt();
  int b = settings->value("color/blue", 0).toInt();
  color = QColor(r,g,b);   
  return;
}
