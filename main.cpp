#include <qpe/qpeapplication.h>
#include "qtprojectdialog.h"


int main( int argc, char** argv )
{
	QPEApplication app( argc, argv );

	QTProjectDialog q;

	app.showMainWidget(&q);

	return app.exec();
}

