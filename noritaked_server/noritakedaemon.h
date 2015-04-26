#ifndef NORITAKEDAEMON_H
#define NORITAKEDAEMON_H

#include <QObject>
#include <iostream>
#include <QTimer>
#include <qmath.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "qaimpinfo.h"

// SIGNAL defines. They're same in Noritake firmware.
// They define Noritake's signals sent over COM port.
#define SIG_INIT_OK 0x01
#define SIG_TRACK_UPDATED 0x02
#define SIG_MISC_UPDATED 0x03
#define SIG_BAD_SEQUENCE 0x04
#define SIG_LEN_EXCEEDED 0x05

// Noritake hardware protocol
#define START_TRACK "\x01"
#define STOP_TRACK "\x02"
#define START_MISC "\x03"
#define STOP_MISC "\x04"


class NoritakeDaemon : public QObject
{
    Q_OBJECT
public:
    explicit NoritakeDaemon(QObject *parent = 0);
    ~NoritakeDaemon();



    void log (QString m, bool isHw = false);

    void comConnect (int comPort);
    void setTrack(QString track);
    void setMisc (QString misc);

private:
    QAimpInfo ai;
    QString portName,
            lastTrack,
            lastMisc;
    QSerialPort *com;
    QTimer *trackChecker;

    short miscVariant;


signals:

public slots:
    void comReadyRead();
    void checkTrack();
};

#endif // NORITAKEDAEMON_H
