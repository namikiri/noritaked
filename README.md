# noritaked #
A simple project for Arduino and Noritake's Vacuum-Fluorescent Display which shows current track (nowplaying). **[How it works](https://www.youtube.com/watch?v=v_6D0qztpcE)**.

## Parts ##
The project consists of two parts: Arduino firmware and server-side running on host PC. They're communicating via COM port. Check main.cpp in server side for correct COM port number.

### Firmware ###
Arduino firmware accepts specially formed sequences of bytes to display current track and additional information. The protocol is simple. To set the track name, you should send this:

```
\x01Unknown Artist - Track 8\x02
```

To set the misc info, send this:

```
\x03228kbps @ 44kHz\x04
```

Track name length is limited to 256 chars, misc info length is limited to 16 chars. If the sequence is broken, a special signal will be sent. See `noritaked_hardware.ino` for constants specifying signals sent by Arduino.


### Server side ###
Server side is written with help of Qt as it have fancy QSerialPort class. It fetches infromation from [AIMP](http://aimp.ru) media player and sends metadata to hardware via COM port according to protocol above. It uses [QAimpInfo](https://bitbucket.org/identsoft/qaimpinfo) class to fetch metadata from media player. Unfortunately it's bound to Windows because AIMP runs only on this operating system.