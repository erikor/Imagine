#include "imgview.h"

imgView::imgView(QWidget *parent)  : QGraphicsView(parent)
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

void imgView::setAnalyzer(imgAnalyzer* an) {
	analyzer = an;
	QObject::connect(analyzer, SIGNAL(bitmaskChanged(QBitmap)), this, SLOT(setMask(QBitmap)));
    QObject::connect(analyzer, SIGNAL(analysisCompleted(Results)), this, SLOT(annotate(Results)));
    QObject::connect(this, SIGNAL(roiChanged(QBitmap)), analyzer, SLOT(setRoi(QBitmap)));
}



void imgView::annotate(Results r) {
	clearAnnotations();
	QGraphicsItem *item;
	foreach(item, r.annotations) {
	  annotationGroup << item; 
	  if(qgraphicsitem_cast<QGraphicsTextItem*>(item)) {
        qgraphicsitem_cast<QGraphicsTextItem*>(item)->setFont(font);
        qgraphicsitem_cast<QGraphicsTextItem*>(item)->setDefaultTextColor(QColor(color.red(),color.green(),color.blue())); 	
      }
	  scene->addItem(item);	
	  item->setZValue(100);
	  item->show();
	}
	return;
}

QStringList imgView::getSegmentationMethods() {
	return analyzer->getSegmentationMethods();
}

void imgView::mousePressEvent(QMouseEvent *e) {
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

void imgView::mouseReleaseEvent(QMouseEvent *e) {
  QImage roi;
  if (imageLoaded && !roiLocked) {
	roiPoly.append(roiStart);  
    QRectF brect = roiFree->boundingRect();
    roi = QImage(image->width(), image->height(), QImage::Format_Mono);
    roi.fill(Qt::color0);
    QPainter* painter = new QPainter(&roi);
    painter->setPen(Qt::color0);
    painter->setBrush(Qt::color0);
    if (select == FREESELECT) {
      painter->drawPolygon(roiFree->polygon());    	
   	} else {
      painter->drawPolygon(roiRect->rect());    	
	}
    painter->end();
  }
  QBitmap pix = QBitmap::fromImage(roi);
/*  QImage m = pix.toImage();
  image = &m;
  QPixmap pm = QPixmap::fromImage(roi);
  pixmap = scene->addPixmap(pm);
    pixmap->setZValue(0);

    this->resetMatrix(); 
    this->setSceneRect(pixmap->mapToParent(pixmap->boundingRect()).boundingRect());
    pixmap->ensureVisible(pixmap->boundingRect());
  qDebug() << "mouseRelease Event: " << qRed(m.pixel(20,20));
*/
  emit roiChanged(pix);
}

void imgView::mouseMoveEvent(QMouseEvent *e) {
  if (e->buttons() == Qt::LeftButton && imageLoaded && !roiLocked) {
	int x, y;
    int w = image->width();
    int h = image->height();
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
 
void imgView::reset() {
	roiFree = NULL;
	roiRect = NULL;
}

void imgView::zoom(int z)
{
    float sf = ((float)z + 1.0)/100.0;
    this->resetMatrix(); 
    this->scale(sf, sf);
	return;
}

void imgView::openImages() {
    clearAnnotations();

    analyzer->openImages();
	if(getFileNames().size() == 0) {
  	  return;
	}
	
  	if(pixmap) {
 		scene->removeItem(pixmap);
 		delete(pixmap);
	}

    if (!roiLocked)
    {
    	if(roiRect) {
    		scene->removeItem(roiRect);
    		roiRect=NULL;    		
   		}
   		if(roiFree) {
   			scene->removeItem(roiFree);    			
    		roiFree=NULL;    		
   		}
    		    		
   	}

    image = analyzer->getCurrentImage(r_ch, g_ch, b_ch);  
    //if(flatField)
   	//    flatFieldAdjust();
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
    //if(useAutoThreshold)
    //	threshold = autoThreshold(image, channel);
    clearMask();
  	roiFree->setBrush(mask);
    roiRect->setBrush(mask);
  return;
}

int imgView::zoomAll() {
	qreal zf, zw, zh;
	zw = 100.0 * (qreal)(this->width()) / (qreal)image->width();
	zh = 100.0 * (qreal)(this->height()) / (qreal)image->height();		
	
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


void imgView::switchImage(int index)
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
    QPixmap pm =  QPixmap::fromImage(*image);
    pixmap = scene->addPixmap(pm);	 
    pixmap->setZValue(0);
    pixmap->show();
 	
  return;	
}

void imgView::openFlatField(void){
  QString fn = QFileDialog::getOpenFileName(this, 
                          tr("Open File"),
                          "c:/",
                          tr("Images (*.tif; *.TIFF)"));
  if(fn.size() == 0) 
   	return;
//  flatFieldImage = loadTiff(fn);
  if(pixmap) {
	scene->removeItem(pixmap);
    clearAnnotations();       	  
    //flatFieldAdjust();
    pixmap = scene->addPixmap(QPixmap::fromImage(*image));
/*
    if(filter == HIGHPASS && !maskLocked) {
  	  highPass();
    }  else if(!maskLocked){
  	  lowPass();
    }  
*/
  }
  
  flatField = true;
}


int min(int a, int b) {
  if (a < b)
  	 return a;
  return b;
}

void imgView::setRectSelect() {
	select=RECTSELECT;
    if(!imageLoaded) 
     	return;
     	
	if(roiFree) {
		roiFree->hide();
	}
}

void imgView::setFreeSelect() {
	select=FREESELECT;
    if(!imageLoaded) 
    	return;
    	
	if(roiRect) {
		roiRect->hide();
	}
}


void imgView::setHighPass() {
	filter=HIGHPASS;

    	return;
}

void imgView::setLowPass() {
	filter=LOWPASS;
 
    	return;
}


QRectF imgView::getRectRoi(){
  return(roiRect->rect());	
}

QPolygonF imgView::getFreeRoi(){
  return(roiFree->polygon());	
}


QBitmap imgView::getRoi(){
  QBitmap mask = QBitmap(image->width(), image->height());
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


QBitmap imgView::getBitmask(){
  QBitmap mask;
  if (select == FREESELECT) {
    mask = roiFree->brush().texture().mask();
  } else {
    mask = roiRect->brush().texture().mask();
  }
  return(mask);	
}

QImage* imgView::getImage() {
  return(image);
}


void imgView::setAutoThreshold(bool th) {
  useAutoThreshold = th;

}


void imgView::setThreshold(int th) {
	threshold=th;

}


int imgView::getThreshold() {
	return(threshold);
}

QStringList imgView::getFileNames() {
	return(analyzer->getFileNames());
}

void imgView::setRoiLocked(bool l)
{
	roiLocked = l;
}

void imgView::setMaskLocked(bool locked)
{
	maskLocked = locked;

}

bool imgView::isRoiLocked()
{
	return(roiLocked);
}

bool imgView::isMaskLocked()
{
	return(maskLocked);
}

QGraphicsScene* imgView::getScene(){
	return(scene);
}

void imgView::addAnnotationItem(QGraphicsItem* item) {
  scene->addItem(item);
  item->setZValue(110);
  item->show();
  annotationGroup << item;
}

void imgView::addAnnotationText(QString s, QPoint p) {
  QGraphicsTextItem* ti = scene->addText(s);
  ti->setFont(font);
  ti->setPos(p);
  ti->setDefaultTextColor(color);	
  ti->setZValue(200);
  annotationGroup << ti;
}

void imgView::clearAnnotations(){	
	QGraphicsItem* item;
    if(annotationGroup.size() > 0) {
    	foreach(item, annotationGroup) {
    		scene->removeItem(item);
   		}
    	annotationGroup.clear();
   	}
}

void imgView::setChannel(int ch){
	channel=ch;


}

int imgView::getChannel() {
	return(channel);
}

void imgView::printImage() {
 QPrinter printer(QPrinter::HighResolution);
 QPrintDialog printDialog(&printer);
 if (printDialog.exec() == QDialog::Accepted) {
   QPainter* painter = new QPainter(&printer);
   this->render(painter);
   painter->end();
 }	
}

void imgView::saveImage() {
  QString dir = settings->value("lastdir", QString()).toString();

  if (dir.isNull())
  	dir = "/";

  QString fn = QFileDialog::getSaveFileName(this, 
                          tr("Save File"),
                          dir,
                          tr("Images (*.jpg; *.jpeg)"));

  if(fn.size() == 0)
  	return;
  QImage save = *image;
  QPainter* painter = new QPainter(&save);
  this->render(painter);
  //QRectF target(0, 0, imageult.width(), result.height());
  //QRectF source(0, 0, result.width(), result.height());
  //painter->drawPixmap(target, QPixmap::fromImage(result), source);
  painter->end();
  save.save(fn, 0, 100);	
}

void imgView::chooseColor() {
	
  QColor col = QColorDialog::getColor ();
  if(col.isValid()) {
    settings->setValue("color/red", col.red());
    settings->setValue("color/green", col.green());
    settings->setValue("color/blue", col.blue());  	
    settings->setValue("color/alpha", 120);  	
    color = QColor(col.red(),col.green(),col.blue(),120); 
  }
}

void imgView::chooseFont() {
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

void imgView::setFont() {
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


void imgView::setColor() {
 int r = settings->value("color/red", -1).toInt();
 if(r == -1) {
    color = QColor(255,0,0,120);
    return;  	
  }
  int g = settings->value("color/green", 0).toInt();
  int b = settings->value("color/blue", 0).toInt();
  color = QColor(r,g,b,120);   
  return;
}

void imgView::switchChannel(bool r, bool g, bool b) {
	r_ch = r;
	b_ch = b;
	g_ch = g;
	  	if(pixmap) {
 		scene->removeItem(pixmap);
 		delete(pixmap);
	}
    image = analyzer->getCurrentImage(r_ch, g_ch, b_ch);
    //if(flatField)
   	//    flatFieldAdjust();

    QPixmap pm =  QPixmap::fromImage(*image);
    pixmap = scene->addPixmap(pm);	 
    pixmap->setZValue(0);
    pixmap->show();

}

void imgView::setMask(QBitmap m) {
  mask = QPixmap(m.width(), m.height());
  mask.fill(color);
  mask.setMask(m);
  roiFree->setBrush(mask);
  roiRect->setBrush(mask);
}

void imgView::clearMask() {
  mask = QPixmap(image->width(), image->height());
  mask.fill(color);
  QBitmap m = QBitmap(mask.width(), mask.height());
  m.fill(Qt::color0);
  mask.setMask(m);
  roiFree->setBrush(mask);
  roiRect->setBrush(mask);
}