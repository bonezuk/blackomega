#include "network/inc/Resource.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/info/inc/MPCChapterTag.h"
#include "engine/cyanomega/inc/MusePackIF.h"
#include "track/info/inc/APETag.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------
// MPCChapterTagItem
//-------------------------------------------------------------------------------------------

MPCChapterTagItem::MPCChapterTagItem() : m_sampleIndex(0),
    m_time(0),
    m_title(),
    m_performer(),
    m_songWriter(),
    m_composer(),
    m_arranger(),
    m_message(),
    m_genre(),
    m_trackIndex(0),
    m_numberOfTracks(0)
{}

//-------------------------------------------------------------------------------------------

MPCChapterTagItem::MPCChapterTagItem(const MPCChapterTagItem& rhs) : m_sampleIndex(0),
    m_time(0),
    m_title(),
    m_performer(),
    m_songWriter(),
    m_composer(),
    m_arranger(),
    m_message(),
    m_genre(),
    m_trackIndex(0),
    m_numberOfTracks(0)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

MPCChapterTagItem::~MPCChapterTagItem()
{}

//-------------------------------------------------------------------------------------------

const MPCChapterTagItem& MPCChapterTagItem::operator = (const MPCChapterTagItem& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void MPCChapterTagItem::copy(const MPCChapterTagItem& rhs)
{
    m_sampleIndex = rhs.m_sampleIndex;
    m_time = rhs.m_time;
    m_title = rhs.m_title;
    m_performer = rhs.m_performer;
    m_songWriter = rhs.m_songWriter;
    m_composer = rhs.m_composer;
    m_arranger = rhs.m_arranger;
    m_message = rhs.m_message;
    m_genre = rhs.m_genre;
    m_trackIndex = rhs.m_trackIndex;
    m_numberOfTracks = rhs.m_numberOfTracks;
}

//-------------------------------------------------------------------------------------------

const tint& MPCChapterTagItem::sampleIndex() const
{
    return m_sampleIndex;
}

//-------------------------------------------------------------------------------------------

const QString& MPCChapterTagItem::title() const
{
    return m_title;
}

//-------------------------------------------------------------------------------------------

const QString& MPCChapterTagItem::performer() const
{
    return m_performer;
}

//-------------------------------------------------------------------------------------------

const QString& MPCChapterTagItem::songWriter() const
{
    return m_songWriter;
}

//-------------------------------------------------------------------------------------------

const QString& MPCChapterTagItem::composer() const
{
    return m_composer;
}

//-------------------------------------------------------------------------------------------

const QString& MPCChapterTagItem::arranger() const
{
    return m_arranger;
}

//-------------------------------------------------------------------------------------------

const QString& MPCChapterTagItem::message() const
{
    return m_message;
}

//-------------------------------------------------------------------------------------------

const QString& MPCChapterTagItem::genre() const
{
    return m_genre;
}

//-------------------------------------------------------------------------------------------

const tint& MPCChapterTagItem::trackIndex() const
{
    return m_trackIndex;
}

//-------------------------------------------------------------------------------------------

const tint& MPCChapterTagItem::numberOfTracks() const
{
    return m_numberOfTracks;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& MPCChapterTagItem::time() const
{
    return m_time;
}

//-------------------------------------------------------------------------------------------

bool MPCChapterTagItem::read(common::BIOStream *file)
{
    static const char c_CTHeader[2] = {'C','T'};
    
    tchar tmp[2];
    bool res = false;
    
    if(file!=0)
    {
        if(file->read(tmp,2)==2 && ::memcmp(c_CTHeader,tmp,2)==0)
        {
            tint tagSize,sampleOffset,pos = 0;
        
            tagSize = decodeSize(file,pos);
            if(tagSize>=0)
            {
                sampleOffset = decodeSize(file,pos);
                if(sampleOffset>=0)
                {
                    tint len = tagSize - (pos + 2);
                    if(len>=28)
                    {
                        tchar *mem = new tchar [len];
                        
                        if(file->read(mem,len)==len)
                        {
                            tint version;
                            
                            version = static_cast<tint>(APETagItem::unsignedIntFromMemory(&mem[4]));
                            if(version==1000 || version==2000)
                            {
                                tint tagLen = static_cast<tint>(APETagItem::unsignedIntFromMemory(&mem[8]));

                                if(tagLen==(len-28))
                                {
                                    tint i,offset,itemCount = static_cast<tint>(APETagItem::unsignedIntFromMemory(&mem[12]));
                                    
                                    res = true;
                                    offset = 28;
                                    
                                    for(i=0;i<itemCount && offset<len && res;i++)
                                    {
                                        tint amount;
                                        APETagItem item;
                                        
                                        amount = item.read(&mem[offset],len - offset);
                                        if(amount>0)
                                        {
                                            if(item.isString())
                                            {
                                                if(item.key().toLower()=="title")
                                                {
                                                    m_title = item.text();
                                                }
                                                else if(item.key().toLower()=="artist")
                                                {
                                                    m_performer = item.text();
                                                }
                                                else if(item.key().toLower()=="songwriter")
                                                {
                                                    m_songWriter = item.text();
                                                }
                                                else if(item.key().toLower()=="composer")
                                                {
                                                    m_composer = item.text();
                                                }
                                                else if(item.key().toLower()=="arranger")
                                                {
                                                    m_arranger = item.text();
                                                }
                                                else if(item.key().toLower()=="comment")
                                                {
                                                    m_message = item.text();
                                                }
                                                else if(item.key().toLower()=="genre")
                                                {
                                                    m_genre = item.text();
                                                }
                                                else if(item.key().toLower()=="track")
                                                {
                                                    res = trackNumber(item.text().trimmed(),m_trackIndex,m_numberOfTracks);
                                                }
                                            }
                                            
                                            offset += amount;
                                        }
                                        else
                                        {
                                            res = false;
                                        }
                                    }
                                    
                                    if(offset!=len || i!=itemCount)
                                    {
                                        res = false;
                                    }
                                }
                            }
                        }
                        delete [] mem;
                    }
                    if(res)
                    {
                        m_sampleIndex = sampleOffset;
                    }
                }
            }
        }
    }
    return res;
}        

//-------------------------------------------------------------------------------------------

tint MPCChapterTagItem::decodeSize(common::BIOStream *file,tint& pos)
{
    tubyte x;
    tint size = 0;
    
    if(file!=0)
    {
        bool loop = true;
        
        while(loop && size>=0)
        {
            if(file->read(&x,1)==1)
            {
                size <<= 7;
                size |= static_cast<tint>(x & 0x7f);
                loop = (x & 0x80) ? true : false;
                pos++;
            }
            else
            {
                size = -1;
            }
        }
    }
    else
    {
        size = -1;
    }
    return size;
}

//-------------------------------------------------------------------------------------------

bool MPCChapterTagItem::trackNumber(const QString& data,int& trackIndex,int& noOfTracks)
{
    tint i,state = 0,pos = 0;
    bool res = false;
    
    for(i=0;i<data.length() && state>=0;i++)
    {
        const QChar& c = data.at(i);

        if(!c.isNumber())
        {
            if(c=='/' && state==0)
            {
                trackIndex = data.mid(0,i).toInt();
                if(trackIndex>0)
                {
                    pos = i + 1;
                    trackIndex--;
                    state++;
                }
                else
                {
                    state = -1;
                }
            }
            else
            {
                state = -1;
            }
        }
    }
    if(state==1)
    {
        noOfTracks = data.mid(pos).toInt();
        if(trackIndex < noOfTracks)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void MPCChapterTagItem::setFrequency(tint frequency)
{
    m_time = static_cast<tfloat64>(m_sampleIndex) / static_cast<tfloat64>(frequency);
}

//-------------------------------------------------------------------------------------------
// MPCChapterTag
//-------------------------------------------------------------------------------------------

MPCChapterTag::MPCChapterTag() : m_items(),
    m_length()
{}

//-------------------------------------------------------------------------------------------

MPCChapterTag::~MPCChapterTag()
{}

//-------------------------------------------------------------------------------------------

const QVector<MPCChapterTagItem>& MPCChapterTag::items() const
{
    return m_items;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& MPCChapterTag::length() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------

bool MPCChapterTag::read(common::BIOStream *file)
{
    bool res = false;
    
    m_items.clear();
    
    if(file!=0)
    {
        int bkPos = file->bookmark();
        if(bkPos>=0)
        {        
            mpc_status err;
            mpc_reader reader;
            engine::cyanomega::MusePackIF mpcIF;
        
            ::memset(&reader,0,sizeof(mpc_reader));
            err = mpcIF.mpc_reader_init_stdio(&reader,file->name().toUtf8().constData());
            if(err==MPC_STATUS_OK)
            {
                mpc_demux *demux;
                
                demux = mpcIF.mpc_demux_init(&reader);
                if(demux!=0)
                {
                    int chapPos;
                    mpc_streaminfo streamInfo;
                    
                    mpcIF.mpc_demux_get_info(demux,&streamInfo);
                
                    chapPos = (demux->chap_pos >> 3) + streamInfo.header_position;
                    
                    if(file->seek(chapPos,common::e_Seek_Start))
                    {
                        QMap<int,MPCChapterTagItem> itemsMap;
                        int noTracks = -1;
                        bool loop = true;
                        
                        res = true;
                        while(loop && res)
                        {
                            
                            MPCChapterTagItem item;
                            
                            if(item.read(file))
                            {
                                if(noTracks<0)
                                {
                                    noTracks = item.numberOfTracks();
                                }
                                
                                if(noTracks==item.numberOfTracks())
                                {
                                    itemsMap.insert(item.trackIndex(),item);
                                    if(itemsMap.size() >= noTracks)
                                    {
                                        loop = false;
                                    }
                                }
                                else
                                {
                                    res = false;
                                }
                            }
                            else
                            {
                                res = false;
                            }
                        }
                        
                        if(res)
                        {
                            for(QMap<int,MPCChapterTagItem>::iterator ppI=itemsMap.begin();ppI!=itemsMap.end();ppI++)
                            {
                                MPCChapterTagItem& item = ppI.value();
                                item.setFrequency(streamInfo.sample_freq);
                                m_items.append(ppI.value());
                            }
                            m_length = static_cast<tfloat64>(mpcIF.mpc_streaminfo_get_length_samples(&streamInfo)) / static_cast<tfloat64>(streamInfo.sample_freq);
                        }
                    }
                    mpcIF.mpc_demux_exit(demux);
                }
            }
            mpcIF.mpc_reader_exit_stdio(&reader);
            
            file->position(bkPos);
            file->deleteBookmark(bkPos);
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
