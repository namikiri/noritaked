#include <QCoreApplication>
#include "noritakedaemon.h"

/*

	Noritake Display Daemon Server Side
	by IDENT Software ~ http://identsoft.org

*/

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NoritakeDaemon nd;
    nd.comConnect(8); // 8 is the number of COM port (e.g. COM8)

    return a.exec();
}
