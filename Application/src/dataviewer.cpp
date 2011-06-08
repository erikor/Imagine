#include "dataviewer.h"
#include <QTextEdit>

dataViewer::dataViewer( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	ui.setupUi(this);
	ui.dataTree->setColumnCount(3);
    ui.dataTree->setHeaderLabel("");	

}

void dataViewer::newSection(QString name) {
  	QStringList value = QStringList(name);
    currentItem = new QTreeWidgetItem(value);
    ui.dataTree->insertTopLevelItem(0, currentItem);
}

void dataViewer::addData(QString data) {
	QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(data));
	QTextEdit* sb =new QTextEdit();
	sb->setHtml("<h1>Hello <b>World!</b></h1>");
	sb->setAutoFillBackground(true);
    ui.dataTree->setItemWidget(item, 2, sb);
    currentItem->addChild(item);  
}
