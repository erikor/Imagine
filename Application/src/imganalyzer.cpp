#include "imganalyzer.h"


imgAnalyzer::imgAnalyzer(QWidget* parent)
  :QObject(parent)
{
  settings = new QSettings("VARI Epi", "Imagine");
  model = new imgModel(parent);
  setParent(parent);
  model->setParent(parent);
  roi = QBitmap();
  loadPlugins();
}

void imgAnalyzer::loadPlugins(){
    QDir pluginsDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
     if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
         pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
     if (pluginsDir.dirName() == "MacOS") {
         pluginsDir.cdUp();
         pluginsDir.cdUp();
         pluginsDir.cdUp();
     }
#endif
     pluginsDir.cd("plugins");

     foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
     	 qDebug() << fileName;
         QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
         QObject *plugin = loader.instance();
         if (plugin) {
             initPlugin(plugin);
             //pluginFileNames += fileName;
         }
     }
}

void imgAnalyzer::initPlugin(QObject* plugin) {
   analysisInterface *iAnalysis = qobject_cast<analysisInterface *>(plugin);
   if (iAnalysis)
   {     	
     foreach (QString text, iAnalysis->methods()) {
       QAction *action = new QAction(text, plugin);
       QObject::connect(action, SIGNAL(triggered()), this, SLOT(analyzeImageExec()));
       analysisPlugins.actions << action;
       analysisPlugins.names << text;
     }
   }
   segmentationInterface *iSegmentation = qobject_cast<segmentationInterface *>(plugin);
   if (iSegmentation)
   {     	
     foreach (QString text, iSegmentation->methods()) {
       QAction *action = new QAction(text, plugin);
       connect(action, SIGNAL(triggered()), this, SLOT(segmentImageExec()));
       segmentationPlugins.actions << action;
       segmentationPlugins.names << text;
     }
   } 
}

void imgAnalyzer::segmentationPluginGo(int index) {
  QAction *action = segmentationPlugins.actions.at(index);
  action->trigger();
  return;
}


void imgAnalyzer::analysisPluginGo(int index) {
  QAction *action = analysisPlugins.actions.at(index);
  action->trigger();
  return;
}


QStringList imgAnalyzer::getAnalysisMethods() {
	return analysisPlugins.names;
}

QStringList imgAnalyzer::getSegmentationMethods() {
	return segmentationPlugins.names;
}

void imgAnalyzer::analyzeImageExec(){
	QAction *action = qobject_cast<QAction *>(sender());
	analysisInterface *iAnalysis = qobject_cast<analysisInterface *>(action->parent());
	QString method = action->text();

	QImage m = roi.toImage();
    qDebug() << "Analyzer roi data: " << qRed(m.pixel(20,20));

	iAnalysis->init(model->getImages(), &roi, &bitmask, model->getCurrentIndex(), (int)currentChannel);
	result = iAnalysis->analyzeImage(method);
	emit analysisCompleted(result);
}

void imgAnalyzer::segmentImageExec(){
	QAction *action = qobject_cast<QAction *>(sender());
	segmentationInterface *iSegment = qobject_cast<segmentationInterface *>(action->parent());
	QString method = action->text();
	iSegment->init(model->getImages(), &roi, model->getCurrentIndex(), (int)currentChannel);
	bitmask = iSegment->segmentImage(method);
	if(bitmask.width() > 1) { // valid bitmap (user did not cancel)
  	  emit bitmaskChanged(bitmask);		
	}
}

void imgAnalyzer::analyzeImage(QString method){
    QRegExp rx(method);  
    int index = analysisPlugins.names.indexOf(rx);
    if (index >= 0) {
	  QAction *action = analysisPlugins.actions.at(index);
	  action->trigger();	    
   	}
}


void imgAnalyzer::segmentImage(QString method){
    QRegExp rx(method);  
    int index = segmentationPlugins.names.indexOf(rx);
    if (index >= 0) {
	  QAction *action =segmentationPlugins.actions.at(index);
	  action->trigger();	    
   	}
}

QList<QGraphicsItem*> imgAnalyzer::getAnnotationItems(){
  return result.annotations;
}

QString imgAnalyzer::getAnalysisResults(){
  return result.data;
}

QBitmap* imgAnalyzer::getCurrentMask(){
  return(&bitmask);
}

QImage* imgAnalyzer::getCurrentImage(bool r, bool g, bool b)
{
	return(model->getCurrentImage(r,g,b));
}

void imgAnalyzer::setCurrentImage(int i) {
	model->setCurrentImage(i);
}

void imgAnalyzer::setChannel(Channel ch){
  	currentChannel = ch;  	
}

void imgAnalyzer::openImages() {
	model->openImages();
}

QStringList imgAnalyzer::getFileNames() {
	return(model->getImageNames());
}

void imgAnalyzer::setRoi(QBitmap b) {
	QImage m = b.toImage();
    qDebug() << "setRoi: " << qRed(m.pixel(20,20));

	roi = b;
}
