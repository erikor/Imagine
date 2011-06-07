#include "matrix.h"


QIntMatrix::QIntMatrix (int nc, int nr)
{ 
  int i, ii;
  //data = (int**)malloc(nc * sizeof(int*));
  data = 0; 
  data = new int*[nc];
  if(data == NULL)
  {
  	qDebug() << "Error allocating int matrix in matrix.cpp!";
    return;
  } 
  for(i = 0; i < nc; i++)
  {
    data[i] = new int[nr];
    //data[i] = (int*)malloc(nr * sizeof(int));
    if(data[i] == NULL) {
  	  qDebug() << "Error allocating int matrix in matrix.cpp!";
      return;
    }
    for(ii = 0; ii < nr; ii++)
    {
      data[i][ii] = 0;
    }
  }
  width = nc;
  height = nr;
}

QIntMatrix::~QIntMatrix(){
  int i;
  for(i = 0; i < width; i++)
  {
    delete [] data[i];
    //free(data[i]);
  }
  //free(data);
  delete [] data;
  data = 0;
  return;	
}


int mathmin (int a, int b)
{
  if(a<b) return(a);
  return(b);
}

int mathmax (int a, int b)
{
  if(a<b) return(b);
  return(a);
}

int QIntMatrix::in(int v)
{ 
  int x, y, c = 0;
  
  for(y = 0; y < height; y++) {
    for(x = 0; x < width; x++) {
      if (data[x][y] == v) 
        c++;
    }
  }
  
  return(c);
}

QRect QIntMatrix::replace(int v, int r)
{ 
  int x, y, c = 0;
  int x1=width, x2=0, y1=height, y2=0;
  for(y = 0; y < height; y++) {
    for(x = 0; x < width; x++) {
      if (data[x][y] == v) 
      {
        data[x][y] = r;
        if(x < x1)
        	x1 = x;
        if(x > x2)
        	x2 = x;
        if(y < y1)
        	y1 = y;
        if(y > y2)
        	y2 = y;
      }
    }
  }  
  return(QRect(x1, y1, x2-x1, y2-y1));
}

int QIntMatrix::uniqueCount()
{ 
  int x, y, c = 1;
  bool isFirst = true;
  QVector<int> u;
  for(y = 0; y < height; y++) {
    for(x = 0; x < width; x++) {
      if (isFirst)
      {
        u << data[x][y];
        isFirst = false;
      } else {        
        if(!u.contains(data[x][y]))
        {
          c++;
          u << data[x][y];
        }
      }
    }
  }
  return(c);
}


QVector<int> QIntMatrix::uniqueValues()
{ 
  int x, y;
  QVector<int> u;
  for(y = 0; y < height; y++) {
    for(x = 0; x < width; x++) {
      if(!u.contains(data[x][y]))
      {
        u << data[x][y];
      }
    }
  }
  return(u);
}

void QIntMatrix::set(int x, int y, int val)
{  
  data[x][y] = val;
}

int QIntMatrix::at(int x, int y)
{  
  return(data[x][y]);
}

void QIntMatrix::dumpMatrix()
{
	int w = width;
	int h = height;
	int x,y;
	QStringList line;
	for(y=0; y<h; y++) {
		line = QStringList();
		for(x=0; x<w; x++) {
			line << QString("%1").arg(data[x][y]);
		}
		qDebug() << line.join(" ");
	}
	qDebug() << "Done";
}


