#ifndef DATAVIEWER_H
#define DATAVIEWER_H
//
#include "ui_data.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>

//
class dataViewer : public QMainWindow
{
Q_OBJECT
public:
	dataViewer( QWidget * parent = 0, Qt::WFlags f = 0 );
	void newSection(QString);
	void addData(QString);
private slots:

private:
    Ui::DataViewer ui;
    QTreeWidgetItem *currentItem;
    
};
#endif





