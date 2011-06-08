#include "interfaces.h"

/*void analysisInterface::init(QList<QImage*> imgList, QBitmap* r, QBitmap* m, int i, int c, QWidget* parent){
	images = imgList;
	roi = r;
	mask = m;
	currentIndex = i;
	currentChannel = c;
	app = parent;
}
*/

void segmentationInterface::init(QList<QImage*> imgList, QBitmap* m, int i, int c, QWidget* parent){
	images = imgList;
	roi = m;
	currentIndex = i;
	currentChannel = c;
	app = parent;
}
   