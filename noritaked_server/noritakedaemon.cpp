#include "noritakedaemon.h"

NoritakeDaemon::NoritakeDaemon(QObject *parent) : QObject(parent)
{
    log ("Noritake Itron Display Server daemon v0.2\n");
    log ("Setting up COM library...\n");

    // I was taught at school that vars must be initialized.
    miscVariant = 0;

    com = new QSerialPort(this);
    //com->setPortName("COM8");
    com->setBaudRate(9600);
    com->setParity(QSerialPort::NoParity);
    com->setStopBits(QSerialPort::OneStop);
    com->setDataBits(QSerialPort::Data8);
    com->setFlowControl(QSerialPort::NoFlowControl);

    connect (com, SIGNAL(readyRead()), this, SLOT(comReadyRead()));

    log ("Setting up timers...\n");
    trackChecker = new QTimer(this);
    connect (trackChecker, SIGNAL(timeout()), this, SLOT(checkTrack()));
    trackChecker->start(1000);
}

NoritakeDaemon::~NoritakeDaemon()
{
    com->close();
}

void NoritakeDaemon::log(QString m, bool isHw)
{
    m = m.prepend(isHw ? "[hardware]: " : "[software]: ");
    std::cout << m.toStdString();
}

void NoritakeDaemon::comConnect(int comPort)
{
    log ("Trying to connect to COM"+QString::number(comPort)+": \n");
    if (com->isOpen())
        com->close();

    com->setPort(QSerialPortInfo("COM"+QString::number(comPort)));
    if (com->open(QIODevice::ReadWrite))
    {
        log ("Connected.\n");
    }
        else
        log ("Failed to connect!\n");
}

void NoritakeDaemon::setTrack(QString track)
{
    lastTrack = track;
    if (com->isOpen())
    {
        log ("Setting track: "+track+"\n");
        com->write(QString(START_TRACK+track+STOP_TRACK).toUtf8());
    }
    else
        log ("Can't set track because COM isn't open!\n");
}

void NoritakeDaemon::setMisc(QString misc)
{
    lastMisc = misc;
    if (com->isOpen())
    {
        log ("Setting misc info: "+misc+"\n");
        com->write(QString(START_MISC+misc+STOP_MISC).toUtf8());
    }
    else
        log ("Can't set misc info because COM isn't open!\n");
}



void NoritakeDaemon::comReadyRead()
{
    QByteArray buff = com->readAll();
    for (int i = 0; i < buff.length(); i++)
    {
        short int ch = buff.at(i);

        switch (ch)
        {
            case SIG_INIT_OK :
                log ("INIT OK (device has rebooted)\n", true);
                setTrack(lastTrack);
                setMisc (lastMisc);
                break;

            case SIG_TRACK_UPDATED :
                log ("Track update succeeded.\n", true);
                break;

            case SIG_MISC_UPDATED :
                log ("Misc info update succeeded.\n", true);
                break;

            case SIG_BAD_SEQUENCE :
                log ("Protocol mismatch!\n", true);
                break;

            case SIG_LEN_EXCEEDED :
                log ("Length of the string is corrected to right value.\n", true);
                break;

            default :
                log ("Unknown HW command code: "+QString::number(ch, 16)+"h\n");
        }
    }
}

void NoritakeDaemon::checkTrack()
{
    AimpInfo info;
    if (!ai.getAimpInfo(info))
    {
        setTrack("- NO TRACK -");
        setMisc("Check your AIMP.");
        return;
    }

    QString currentTrack = (info.artist.isEmpty()) ? info.title : info.artist +  " - " + info.title;
    if (lastTrack != currentTrack)
    {
        setTrack(currentTrack);
        lastTrack = currentTrack;
        miscVariant = 0;
    }

    // Miscellaneous info switching.
    QString misc2send;

    if (miscVariant % 6 == 0)
    {
        if (miscVariant % 12 == 0)
        {
            misc2send = QString("%1kbps @ %2kHz").arg(info.bitrate).arg(qFloor(info.sampleRate / 1000));
        }
            else
        {
            misc2send = (info.duration > 0) ?
                        QString ("%1:%2 / %3").arg(qFloor(info.duration / 60), 2, 10, QChar('0'))
                                              .arg(qFloor(info.duration % 60), 2, 10, QChar('0'))
                                              .arg((info.channels == 1) ? "mono" : "stereo") :
                        QString ("no time / %1").arg((info.channels == 1) ? "mono" : "stereo");
        }

        setMisc(misc2send);
    }

    if (miscVariant > 12)
        miscVariant = 1;
    else
        miscVariant++;

}

