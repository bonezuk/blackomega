//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_BIOTIMECACHEDSTREAM_H
#define __OMEGA_COMMON_BIOTIMECACHEDSTREAM_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "common/inc/TimeStamp.h"
#include "common/inc/CachedFileStream.h"

#include <QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
//
// Make up of the Time Cached buffer stream.
//  - The caching has to be codec netural. Thus it is based on constant bitrate metric with
//    variation for compensation.
//
// |--- +1s ---|--- B ---|--- C ---|
//
// The buffer is made up of three parts.
// 1. An extra 1 second padding. As codecs are usually variable bit rate and the caching metric
//    is based on a constant bit rate then there is variation between the expected timing and
//    the actual timing. The additional 1 second padding works as means to compensate for this
//    variation.
// 2. The cyclic audio buffer is flushed when audio is unpaused or reset and then repopulated
//    with the audio data by reading in from the codec. The time length of the cyclic audio
//    buffer will be defined by the BIOTimeCachedStreamSettings::getBufferTimeLength() method.
//    This data is also corrispondingly cached below the codec layer in the file cache marked
//    as section 'B' for 'buffer'.
// 3. As audio data is rendered, during playback, it is consumed from the cyclic buffer items
//    one at a time and that cyclic buffer item becomes available. The empty cyclic buffer item
//    is then filled up again by the codec decoder thread. Additionally network latency and
//    congestion can being buffered available in time blocking the repopulation of the cyclic
//    audio buffer. The 'C' for 'cache' buffer caches the file data that is due to be read in
//    and decoded such that the decoder thread is not blocked due to network latency. The length
//    of the cache is defined by BIOTimeCachedStreamSettings::getCacheTimeLength().
//
// There should be an option to specify the size of the cyclic buffer in time length such that
// the amount of decoded audio being processed and hence buffered, along with the cache can be
// specified. The onReadyForNext signal could be made dependant as a multiple of the cyclic
// buffer time length.
//
// When the codec's init method is called a series of queueing requests can be issued to start
// caching the stream. As the audio layer may issue a codec seek instruction to a given time
// position (as in the case where a file contains multiple tracks) then it will have to cancel
// the currently queued requests an issue a series of new ones. This also applies when seeking
// to a new time position and pending asynchronise read requests have to be cancelled and the
// new ones requeued.
//
// Startup initialization sequence. Top level overview:
// 1. Audio layer calls startCodec which creates the codec instance, via factory.
// 2. The codec open method which in turn opens the file.
// 3. Audio layer calls into startAudio which calls the codec init method.
//       - The number of channels, frequency and bitrate of the given codec are known at
//         this point.
// 4. The cyclic buffer is now initialised.
// 5. The actual audio device is now opened.
// 6. The cyclic buffer is now populated with audio from the codec.
// 7. The audio device is started and audio rendering begins.
//
// Processing for seamless processing.
// 1. In startAudio before audio hardware is opened the time position at which to open
//    and start caching is calculated. This is given in the AOBase::m_nextCodecTime value.
//    It is fixed at being 5 seconds before the end of the current track, with adjustment
//    in respect to the crossfade time. In turn the m_nextOutState is set to 0.
// 2. As the codec is processed and decoded each part is checked to see if the value set
//    in m_nextCodecTime has been reached. If so then the onReadyForNext signal is emitted
//    and the m_nextOutState is set to 1 to handle crossfade processing as approriate.
// 3. The onReadyForNext is handled by the PlayerController and passed to the Player dialog's
//    onAudioNext method. Here the playlist is queried and the next track in the list is
//    found and passed into the audio layer via the AOBase::next method. The m_nextCount
//    used by the player ensures that AOBase::next method is only passed one new track at a
//    a time until it has been processed via the onStart signal.
// 4. The AOBase::next method posts an event which in turns calls the startNextCodec method
//    as the current codec is still playing or has finished processing. This then opens the
//    the next codec and enqueues it if the current audio device settings are compatable.
//    Once the codec is enqueued it starts to be cached.
// 5. TODO - Notes on the end of processing the current and the start of decoding the audio
//    data.
//
// Process codec handling of onReadyForNext signal
//
// The time length of the track is calculated by the respective codec when it is first opened.
// The true length of the track is only discovered once the last packet has been decoded. As
// the m_nextCodecTime value is an offset from the initial track length value and this value
// maybe too inaccurate or actually incorrect. If this is the case then it means
// that the time position for when the onReadyForNext signal is emitted could be too late for
// next codec setup to occur, if ever before the current track has completed. Thus seamless
// audio transition will not happen if such is the case.
//
// Make reference to the startNextCodec implementation notes.
//
// TODO - the onNoNext signal is sent in response to the fact that the next codec in sequence
// could not be enqueued. As it stands calling into onAudioNext will increment the next counter
// which has already been incremented by the onReadyForNext signal. Thus the codec is not requeued
// in the approriate manner. But rather the audio device needs to be reopened at the end of the next
// codec. The current implementation probably but this needs to be fixed such the codec can be queue
// and caching started while the current one completes its playback. Once it has completed then at
// the start of the next one the audio device and cyclic buffer are reinitialized accordingly.
//
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT BIOTimeCachedStreamSettings
{
    public:
        virtual ~BIOTimeCachedStreamSettings();

        static QSharedPointer<BIOTimeCachedStreamSettings> instance();
        static void release();

        common::TimeStamp getCacheTimeLength();
        void setCacheTimeLength(const common::TimeStamp& t);

        common::TimeStamp getBufferTimeLength();
        void setBufferTimeLength(const common::TimeStamp& t);

    protected:

        static QSharedPointer<BIOTimeCachedStreamSettings> m_instance;

        BIOTimeCachedStreamSettings();
};

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT BIOTimeCachedStream : public BIOStream
{
    public:
        BIOTimeCachedStream();
        virtual ~BIOTimeCachedStream();

        virtual const QString& name() const;

        virtual bool open(const tchar *name);
        virtual bool open(const BString& name);
        virtual bool open(const QString& name);

        virtual bool close();

        virtual tint read(tbyte *mem,tint len);
        virtual tint read(tubyte *mem,tint len);

        virtual tint write(const tbyte *mem,tint len);
        virtual tint write(const tubyte *mem,tint len);

        virtual bool seek(tint pos,BIOStreamPosition flag);
        virtual bool seek64(tint64 pos,BIOStreamPosition flag);

        virtual bool sof();
        virtual bool eof();

        virtual tint size();
        virtual tint64 size64();

        virtual void setBitrate(tint rateInBitsPerSecond);

        virtual void springCleanTheCache();

    protected:
        // Pointer to file and its associated cache
        CachedFileStream *m_fileCached;
        // Rate as bits per second
        tint m_bitrate;
        // The position where the current beginning of the cache is.
        tint64 m_cachedFrom;
        // The position from which the last spring clean was done from.
        tint64 m_lastCleanPosition;

        // Store the buffer and cache time lengths
        common::TimeStamp m_bufferTimeLength;
        common::TimeStamp m_cachedTimeLength;

        // Secondary level 2 cache, in order to reduce the processing of the main cached file
        // particularly when numerous small read and seek operations are done on the file.
        tchar *m_cacheL2;
        tint m_cacheL2Length;
        tint m_cacheL2Size;

        virtual void PrintError(const tchar *strR,const tchar *strE) const;
        virtual void PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2) const;
        virtual void PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint code) const;

        virtual CachedFileStream *getCachedFile();
        virtual const CachedFileStream *getCachedFileConst() const;
        virtual tint64& readPosition();
        virtual const tint64& readPositionConst() const;

        virtual tint initialCacheSize() const;
        virtual tint64 lengthFromTime(const common::TimeStamp& tLen) const;

        virtual bool isRangeValid(const QPair<tint64,tint64>& range) const;
        virtual QPair<tint64,tint64> getBufferRange(tint64 pos) const;
        virtual QPair<tint64,tint64> getBufferRange(tint64 pos,const common::TimeStamp& errorMargin) const;

        tint indexL2Cache() const;
        tint indexL2Cache(const tint64& pos) const;
        tint offsetL2Cache() const;
        tint offsetL2Cache(const tint64& pos) const;

        virtual void allocateL2Cache();
        virtual void freeL2Cache();
        virtual tint calculateL2CacheSize() const;
};

//-------------------------------------------------------------------------------------------

inline tint BIOTimeCachedStream::indexL2Cache() const
{
    return indexL2Cache(readPositionConst());
}

//-------------------------------------------------------------------------------------------

inline tint BIOTimeCachedStream::indexL2Cache(const tint64& pos) const
{
    return static_cast<tint>(pos / static_cast<tint64>(m_cacheL2Size));
}

//-------------------------------------------------------------------------------------------

inline tint BIOTimeCachedStream::offsetL2Cache() const
{
    return offsetL2Cache(readPositionConst());
}

//-------------------------------------------------------------------------------------------

inline tint BIOTimeCachedStream::offsetL2Cache(const tint64& pos) const
{
    return static_cast<tint>(pos % static_cast<tint64>(m_cacheL2Size));
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
