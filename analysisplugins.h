class analysisInterface
 {
 public:
     virtual ~analysisInterface() {}

     virtual QStringList filters() const = 0;
     virtual int analyze();
 };

 Q_DECLARE_INTERFACE(analysisInterface,
                     "com.trolltech.imagine.analysisInterface/1.0")