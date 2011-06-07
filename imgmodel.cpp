#include "imgmodel.h"

imgModel::imgModel(QWidget* app) 
  : QObject(app) 
{
  settings = new QSettings("VARI Epi", "Imagine");
  parent =  app;
}

void imgModel::openImages() {
  QString dir = settings->value("lastdir", QString()).toString();
  QList <QImage*> image;
  if (dir.isNull())
  	dir = "/";
  
  QStringList files = QFileDialog::getOpenFileNames(parent, 
                          tr("Open File"),
                          dir,
                          tr("Images (*.tif; *.TIF; *.jpg; *.JPG;)"));
  if(files.size() == 0)
  	return;
  
  QString fn = files.at(0);
  QFileInfo fi(fn);	
  dir = fi.absoluteDir().absolutePath();
  settings->setValue("lastdir", dir);
  foreach (QImage* im, images) {
  	delete(im);
 	}
  images.clear();
  imageNames.clear();
       
  foreach(fn, files) 
  {
    fi.setFile(fn);
    imageNames << fi.fileName();
    if (fi.suffix() == "tif" || fi.suffix() == "TIF"){
      images << loadTiff(fn);
   	} else if (fi.suffix() == "jpg" || fi.suffix() == "JPEG" || fi.suffix() == "JPG"){
   	  QImage *image = new QImage(fn);
   	  images << image;
   	}
  }
  currentImage = 0;
  return;
}

QStringList imgModel::getImageNames() {
	return(imageNames);
}

QImage* imgModel::getCurrentImage(bool red, bool green, bool blue){
  if(red && green && blue)
  	 return(images.at(currentImage));
  int r, g, b, x, y;
  QImage* source = images.at(currentImage);
  QImage* image = new QImage(source->width(), source->height(), 
                     QImage::Format_ARGB32);
  for(y=0; y < source->height(); y++){
    for(x=0; x < source->width(); x++) {        	
      if(red) {
      	r = qRed(source->pixel(x,y));
   	  } else {
    	r=0;
   	  }
      if(green) {
      	g = qGreen(source->pixel(x,y));
   	  } else {
    	g=0;
   	  }
      if(blue) {
      	b = qBlue(source->pixel(x,y));
   	  } else {
    	b=0;
   	  }
      image->setPixel(x, y, qRgb(r, g, b));
    }
  }
  return(image);
}

QList<QImage*> imgModel::getImages() {
  return(images);
}

QImage* imgModel::loadTiff(QString file)
{
    uint32 w, h, *raster;
    int x, y;
    size_t npixels;
    QImage* im;
    
    TIFF* tif = TIFFOpen(file.toLocal8Bit(), "r");


    if (tif) {
      TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w); 
      TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h); 
      npixels = w * h;
      im = new QImage((int)w, (int)h, QImage::Format_ARGB32);
      raster = (uint32*)malloc(npixels * sizeof (uint32));
      
      if (raster != NULL) {
        TIFFReadRGBAImage(tif, w, h, raster, 0);

        for(y=0; y <(int)h; y++){
          for(x=0; x < (int)w; x++)
          {        	
        	int b = (int)GetB(raster[y*w + x]);
        	int g = (int)GetG(raster[y*w + x]);
        	int r = (int)GetR(raster[y*w + x]);
        	int a = (int)GetA(raster[y*w + x]);
        	im->setPixel(x, (h-y)-1, qRgba(r, g, b, a));
          }
        }
        free(raster);
      }
      TIFFClose(tif);
    }
  return(im);
}

void imgModel::setCurrentImage(int i) {
	currentImage = i;
}

int imgModel::getCurrentIndex() {
	return(currentImage);
}
