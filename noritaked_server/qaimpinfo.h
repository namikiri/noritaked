#ifndef QAIMPINFO_H
#define QAIMPINFO_H

#include <QObject>
#include <windows.h>
#include <qmath.h>
#include <QDebug>

#include <QSharedMemory>

#define AIMP2_RemoteClass "AIMP2_RemoteInfo"
#define AIMP2_RemoteClassSize 2048

// This is original structure of AIMP File Info.
#pragma pack(push, 1)
typedef struct _AIMP2FileInfo
{
    DWORD cbSizeOf;
    //
    BOOL nActive;
    DWORD nBitRate;
    DWORD nChannels;
    DWORD nDuration;
    INT64 nFileSize;
    DWORD nRating;
    DWORD nSampleRate;
    DWORD nTrackID;
    //
    DWORD nAlbumLen;
    DWORD nArtistLen;
    DWORD nDateLen;
    DWORD nFileNameLen;
    DWORD nGenreLen;
    DWORD nTitleLen;
    //
    PWCHAR sAlbum;
    PWCHAR sArtist;
    PWCHAR sDate;
    PWCHAR sFileName;
    PWCHAR sGenre;
    PWCHAR sTitle;
} AIMP2FileInfo, *PAIMP2FileInfo;
#pragma pack(pop)

// This will be returned by getAimpInfo()
struct AimpInfo
{
    int bitrate;
    int channels;
    int duration;
    long fileSize;
    int rating;
    int sampleRate;
    int trackId;

    QString album;
    QString artist;
    QString date;
    QString filename;
    QString genre;
    QString title;
};

class QAimpInfo : public QObject
{
    Q_OBJECT
public:
    explicit QAimpInfo(QObject *parent = 0);
    ~QAimpInfo();

    bool getAimpInfo(AimpInfo &info);

    QString getTrack();
    int getDuration();

};

#endif // QAIMPINFO_H
