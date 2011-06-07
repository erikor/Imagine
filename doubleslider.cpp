#include <QtGui>

#include "doubleslider.h"
#include <QMessageBox>

DoubleSlider::DoubleSlider(QWidget *parent)
   : QSlider(parent)
{
  min = 0;
  max = 100;
  v1 = 0;
  v2 = 100;
  QStyleOptionSlider option;
  setTracking(true);
  setMinimum(0);
  setMaximum(100);
}


QVector<int> DoubleSlider::value()
{
  QVector<int> v;
  v << v1;
  v << v2;
  return(v);
}


void DoubleSlider::mousePressEvent(QMouseEvent* e)
{
  int x = (maximum()-minimum()) * e->x() / width();
  if(abs(v1-x) < abs(v2-x)) { 
     v1 = x;
  } else {
     v2 = x;
  }
  this->update();
}

void DoubleSlider::mouseMoveEvent(QMouseEvent* e)
{
  int x = (maximum()-minimum()) * e->x() / width();
  if(abs(v1-x) < abs(v2-x)) { 
     v1 = x;
  } else {
     v2 = x;
  }
  this->update();
}

void DoubleSlider::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  QStyleOptionSlider option;
  option.initFrom(this);
  option.maximum = maximum();
  option.minimum = minimum();
  option.sliderPosition = v1;
  style()->drawComplexControl(QStyle::CC_Slider, &option, &painter, this);
  QRect handle1 = style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, this);

  option.sliderPosition = v2;
  option.subControls = QStyle::SC_SliderHandle;
  style()->drawComplexControl(QStyle::CC_Slider, &option, &painter, this);
  QRect handle2 = style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, this);
  QPen p;
  p.setColor(QColor(0, 0, 200, 150));
  p.setWidth(handle1.height()-6);
  p.setCapStyle(Qt::FlatCap);
  painter.setPen(p);
  QRect groove = style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderGroove, this);
  int wi = handle1.width();
  painter.drawLine(handle1.x()+wi, handle1.height()/2,  handle2.x(), handle1.height()/2);

   painter.end();
}
