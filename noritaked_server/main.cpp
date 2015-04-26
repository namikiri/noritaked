#include <QCoreApplication>
#include "noritakedaemon.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NoritakeDaemon nd;
    nd.comConnect(8);

    return a.exec();
}
