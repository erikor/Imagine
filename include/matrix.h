#ifndef QINTMATRIX_H_
#define QINTMATRIX_H_

#include <QVector>
#include <QtDebug>
#include <QStringList>
#include <QRect>

class QIntMatrix
{

public:
  QIntMatrix (int nr, int nc);
  ~QIntMatrix();
  int in(int v);
  QRect replace(int v, int r);
  int uniqueCount();
  QVector<int> uniqueValues();
  int max();
  void set(int, int, int);
  int at(int, int);
  int width;
  int height;
  void dumpMatrix();

private:
  int** data;

};

int mathmin (int a, int b);
int mathmax (int a, int b);

#endif
