     #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
#include <sys/ioctl.h>
#include "sharp_char.h"
#include "qtprojectdialog.h"
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtimer.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <qlcdnumber.h>
#include <qprogressbar.h>
#include <sys/timeb.h>
#include <math.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <string.h>
#include <qcheckbox.h>

int beatnum = 0;
bool started = false;
timeb starttime;
timeb currenttime;
long currentinttime;
long startinttime;
long avg = 0;
int lastflush = 0;
int ctemp = 0; 
int beatpos = 0;
int tempohist[10];
bool isTone;
int buzzfd;

QTProjectDialog::QTProjectDialog( QWidget* parent, const char* name, WFlags fl ) : QTProjectDialogBase( parent, name, fl )
{
		QObject::connect( Beat, SIGNAL(clicked()), this, SLOT(OnBeat()) );
		QObject::connect( ResetButton, SIGNAL(clicked()), this, SLOT(OnReset()) );
	    timer = new QTimer( this );
		QObject::connect( timer, SIGNAL(timeout()), this, SLOT(UpdateRealtimeElements()) );
		QObject::connect( BeatReset, SIGNAL(clicked()), this, SLOT(OnBeatReset()));
		QObject::connect( SetManual, SIGNAL(clicked()), this, SLOT(OnManualSet()));
		buzzfd = open("/dev/sharp_buz", O_WRONLY);
		Beat->setFocus();
}

void QTProjectDialog::OnManualSet()
{
  QString str = Manual->text();
  if(str.isEmpty() != true)
    {
      unsigned int temporal;
      bool checker;
      temporal = str.toUInt(&checker);
      if(checker == false)
	return;
      TempoDisp->display((int)temporal);
      int interval = (int)((double)60 / (double)temporal*1000);
      timer->stop();
      timer->start(interval);
    }
}

void QTProjectDialog::OnBeat()
{
	ftime(&currenttime);
	currentinttime = currenttime.time * 1000 + currenttime.millitm;
	if(started == false)
	{
		started = true;
		ftime(&starttime);
		startinttime = currenttime.time * 1000 + currenttime.millitm;
		timer->start(1000);

	}
	else
	{
		beatnum++;
		BeatDisplay->display(beatnum);
		DurationDisplay->display((int)(currenttime.time - starttime.time));
		int accutime = currentinttime - startinttime;
		int tempoval = (int)((float)beatnum/(float)accutime * 1000 
* 60);

		TempoDisp->display(tempoval);


		if(beatnum == 1)
		{
			ctemp = 0;
			for(int i = 0 ; i > 10; i++)
			tempohist[i] = tempoval;
			ctemp++;
		}
		else
		{
			tempohist[ctemp] = tempoval;

			int result = 0;

			for(int i = 0 ; i < 10 ; i++)
			{
				result = result + tempohist[i];
			}
			
			int confidence = 0;

			result = result / 10;
			if(result == tempohist[9])
				confidence = 4;
			else if(abs(result - tempohist[0]) == 1)
				confidence = 3;
			else if(abs(result - tempohist[0]) == 2)
				confidence = 2;
			else if(abs(result - tempohist[0]) == 3)
				confidence = 1;
			else
				confidence = 0;

			StabilityGraph->setProgress(25*confidence);

			ctemp++;
			if(ctemp == 10)
				ctemp = 0;
		}
			int interval = (int)((double)60 / (double)tempoval*1000);
			timer->stop();
			timer->start(interval);

			
	}
}


void QTProjectDialog::OnReset()
{
	started= false;
	TempoDisp->display(0);
	BeatDisplay->display(0);
	DurationDisplay->display(0);
	StabilityGraph->setProgress(0);
	beatnum = 0;
	started = false;
	currentinttime = 0;
	startinttime = 0;
	avg = 0;
	lastflush = 0;
	ctemp = 0; 
	beatpos = 0;
	for(int i = 0; i < 0; i++)
	{
		tempohist[i] = 0;
	}
	timer->stop();
	Beat->setFocus();	
}

void QTProjectDialog::OnBeatReset()
{
	beatpos = 0;
	Beat1->setChecked(false);
	Beat2->setChecked(false);
	Beat3->setChecked(false);
	Beat4->setChecked(false);
	Beat->setFocus();

}

void QTProjectDialog::UpdateRealtimeElements()
{

	if(ToneBox->isChecked() == true)
	{
	if(buzzfd != -1)
	{
		ioctl(buzzfd, SHARP_BUZZER_MAKESOUND, SHARP_BUZ_KEYSOUND);	
	}
	}

	switch(DispMode->currentItem())
	{
	case 0:
		{
	Beat1->setChecked(false);
	Beat2->setChecked(false);
	Beat3->setChecked(false);
	Beat4->setChecked(false);
			break;
		}

	case 1:
		{
			if(beatpos > 1)
				beatpos = 0;

			if(beatpos == 0)
			{
				Beat1->setChecked(true);
				beatpos++;
			}
			else if(beatpos == 1)
			{
				Beat2->setChecked(true);
				beatpos = 0;
			}
			break;
		}	

	case 2:
		{
			if(beatpos > 2)
				beatpos = 0;

			if(beatpos == 0)
			{
				Beat1->setChecked(true);
				beatpos++;
			}
			else if(beatpos == 1)
			{
				Beat2->setChecked(true);
				beatpos++;
			}
			else if(beatpos == 2)
			{
				Beat3->setChecked(true);
				beatpos = 0;
			}
			break;
		}	

	case 3:
		{
			if(beatpos == 0)
			{
				Beat1->setChecked(true);
				beatpos++;
			}
			else if(beatpos == 1)
			{
				Beat2->setChecked(true);
				beatpos++;
			}
			else if(beatpos == 2)
			{
				Beat3->setChecked(true);
				beatpos++;
			}
			else if(beatpos == 3)
			{
				Beat4->setChecked(true);
				beatpos = 0;
			}
			break;
		}

	default:
		{
			break;
		}
	}
	Beat->setFocus();
}
