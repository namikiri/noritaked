#include "qaimpinfo.h"

QAimpInfo::QAimpInfo(QObject *parent) : QObject(parent)
{

}

QAimpInfo::~QAimpInfo()
{

}


bool QAimpInfo::getAimpInfo(AimpInfo &info)
{
    QSharedMemory shm;
    shm.setNativeKey(AIMP2_RemoteClass);

    // Trying to attach to AIMP's shared memory segment
    if (!shm.attach(QSharedMemory::ReadOnly))
        return false;

    PAIMP2FileInfo afi = (PAIMP2FileInfo)shm.data();

    if (!afi) // Bad AIMP info. Could we get shared memory area?
    {
        return false;
    }

#define WRITE_TO_BUFF(x) \
    memcpy(buff, pBuff, (x) * 2); \
    buff[x] = '\0'; \
    pBuff += (x);

    // Sorry for Windows types. AIMP is a Windows program,
    // so it's easiest way to interact with it.
    LPWSTR pBuff = (LPWSTR)((PBYTE)afi + afi->cbSizeOf);
    WCHAR buff[256];


    // Preparing 'album' field
    WRITE_TO_BUFF(afi->nAlbumLen)
    // Filling structure
    info.album = QString::fromWCharArray(buff).trimmed();

    // Repeat as above for every string
    WRITE_TO_BUFF(afi->nArtistLen)
    info.artist = QString::fromWCharArray(buff).trimmed();

    WRITE_TO_BUFF(afi->nDateLen)
    info.date = QString::fromWCharArray(buff).trimmed();

    WRITE_TO_BUFF(afi->nFileNameLen)
    info.filename = QString::fromWCharArray(buff).trimmed();

    WRITE_TO_BUFF(afi->nGenreLen)
    info.genre = QString::fromWCharArray(buff).trimmed();

    WRITE_TO_BUFF(afi->nTitleLen)
    info.title = QString::fromWCharArray(buff).trimmed();


    // Integers are sent without any buffering
    info.bitrate = afi->nBitRate;
    info.channels = afi->nChannels;
    info.duration = qFloor(afi->nDuration / 1000);
    info.fileSize = afi->nFileSize;
    info.rating = afi->nRating;
    info.sampleRate = afi->nSampleRate;
    info.trackId = afi->nTrackID;

    // Detach from shared memory
    shm.detach();

    // All done, returning true
    return true;

}

// Some examples of usage of getAimpInfo()
QString QAimpInfo::getTrack()
{
    AimpInfo ai;
    if (getAimpInfo(ai))
        return (ai.artist.isEmpty()) ? ai.title : ai.artist +  " - " + ai.title;
    else
        return ""; // Else return нихуя
}

int QAimpInfo::getDuration()
{
    AimpInfo ai;
    if (getAimpInfo(ai))
        return ai.duration;
    else
        return 0; // Else return нихуя
}
