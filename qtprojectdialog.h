#include "qtprojectdialogbase.h"


class QTProjectDialog : public QTProjectDialogBase
{
	Q_OBJECT
public:
	QTProjectDialog( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );



private:
	QTimer *timer;

private slots:
  void OnManualSet();
	void OnBeat();
	void OnReset();
	void UpdateRealtimeElements();
	void OnBeatReset();
};

