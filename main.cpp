#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <QtGui>
#include "ui_mw.h"
#include "mainwindow.h"
#include "dataviewer.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QMainWindow *mw = new mainWindow(&app);
  mw->show();
  return app.exec();
}
