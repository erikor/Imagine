#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QStyle>
#include <QWidget>
#include <QSlider>

class DoubleSlider : public QSlider
{
Q_OBJECT

public:
  DoubleSlider(QWidget *parent = 0);
  QVector<int> value();
  
public slots:
   void setValue(int, int);
   
signals:
  void valueChanged();
  void minValueChanged(int);
  void minValueChangedString(QString);  
  void maxValueChanged(int);
  void maxValueChangedString(QString);  

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);  
  void mouseReleaseEvent(QMouseEvent*);  
  int min;
  int max;
  int v1;
  int v2;
  bool needUpdate;
};

#endif
