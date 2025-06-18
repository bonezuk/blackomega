//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_SILVEROMEGA_VOGGPACKETS_H
#define __OMEGA_NETWORK_RTP_SILVEROMEGA_VOGGPACKETS_H
//-------------------------------------------------------------------------------------------

#include "network/inc/UDPRead.h"

#include <QString>
#include <QVector>

#include "common/inc/Array.h"
#include "common/inc/TimeStamp.h"
#include "engine/inc/File.h"
#include "engine/silveromega/inc/VSilverOgg.h"
#include "engine/silveromega/inc/VSilverContainer.h"
#include "engine/silveromega/inc/VSilverSeeker.h"
#include "network/rtp/silveromega/inc/RTPSilverOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class RTPSILVEROMEGA_EXPORT VOggPackets
{
    public:
    
        typedef struct s_PacketOffset
        {
            tuint start;
            tuint end;
            bool continued;
        } PacketOffset;
        
        typedef enum
        {
            e_configInformation = 0,
            e_configComments,
            e_configCodecData
        } ConfigurationType;
    
    public:
        VOggPackets();
        virtual ~VOggPackets();
        
        virtual bool open(const tchar *name);
        virtual bool open(const QString& name);
        virtual void close();
        
        virtual bool configuration(ConfigurationType type,NetArray& mem);
        
        virtual bool next(NetArray& mem);
        virtual bool next(NetArray& mem,common::TimeStamp& t);
        
        virtual bool seek(common::TimeStamp& seekTime,common::TimeStamp& actualTime);

        virtual common::TimeStamp length() const;
        
        tuint serialID() const;
        
    protected:
    
        engine::File *m_file;
        common::TimeStamp m_time;
        common::TimeStamp m_timeInc;
        common::TimeStamp m_timePacket;
        common::TimeStamp m_timePacketEnd;
        
        engine::File *m_streamFile;
        engine::Bitstream *m_stream;
        engine::silveromega::VSilverOgg *m_oggStream;
        engine::silveromega::VSilverContainer *m_container;
        engine::silveromega::VSilverSeeker *m_seeker;
        
        bool m_eofFlag;
        tuint m_bufferOffset,m_bufferLength;
        tubyte *m_buffer;
        
        engine::silveromega::OggPageHeader m_pageHeader;
        tuint m_serialID;
        tuint m_packetNo;
        QVector<PacketOffset> m_packetOffsets;
        
        NetArray m_configInformationMem;
        NetArray m_configCommentsMem;
        NetArray m_configCodecDataMem;
        
        tint m_prevBlockMode;
        tint m_currentBlockMode;
        tint m_centerW;
        tint m_outCurrent;
        tint m_outReturn;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual bool read(tuint amount);
        
        virtual tint16 read16FromMem(tubyte *mem);
        virtual tuint16 read16UFromMem(tubyte *mem);
        virtual tint32 read32FromMem(tubyte *mem);
        virtual tuint32 read32UFromMem(tubyte *mem);
        virtual tint64 read64FromMem(tubyte *mem);
        virtual tuint64 read64UFromMem(tubyte *mem);
        
        virtual bool openStream(const QString& name);
        virtual bool forwardSequenceSearch();
        virtual tint readHeader(engine::silveromega::OggPageHeader *hdr);
        virtual void getPagePackets(engine::silveromega::OggPageHeader *hdr,QVector<PacketOffset>& packets);
        virtual bool getNextPacket(NetArray& mem);
        virtual bool getConfiguration(NetArray& mem);
        virtual bool doPageChecksum(tubyte *mem,tint len);
        
        virtual void initPCMLength();
        virtual tint calculatePCMLength(NetArray& array);
};

//-------------------------------------------------------------------------------------------

inline tuint VOggPackets::serialID() const
{
    return m_serialID;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
