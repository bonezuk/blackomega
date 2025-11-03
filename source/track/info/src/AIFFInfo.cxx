#include "track/info/inc/AIFFInfo.h"
#include "track/info/inc/ID3Info1.h"
#include "track/info/inc/ID3Info2.h"
#include "engine/violetomega/inc/IFFFile.h"
#include "engine/violetomega/inc/IFFID3Chunk.h"
#include "engine/violetomega/inc/IFFTextChunk.h"
#include "engine/violetomega/inc/IFFFormChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

AIFFInfo::AIFFInfo() : EmbeddedID3Info()
{
    m_type = e_InfoAIFF;
}

//-------------------------------------------------------------------------------------------

AIFFInfo::~AIFFInfo()
{}

//-------------------------------------------------------------------------------------------

void AIFFInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "AIFFInfo::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool AIFFInfo::read(common::BIOStream *input)
{
    bool res = false;

    if(input!=0)
    {
        engine::violetomega::IFFFileSPtr pIFFFile(new engine::violetomega::IFFFile(input));
        engine::violetomega::IFFChunkSPtr pRoot;

        pRoot = pIFFFile->root();
        if(!pRoot.isNull())
        {
            QString sName,sAuthor,sAnnotation,sCopyright;
            engine::violetomega::IFFFormChunkSPtr pFormChunk = pRoot.dynamicCast<engine::violetomega::IFFFormChunk>();

            if(!pFormChunk.isNull() && (pFormChunk->formType()==IFF_ID('A','I','F','F') || pFormChunk->formType() == IFF_ID('A', 'I', 'F', 'C')))
            {
                for(engine::violetomega::IFFFormChunk::Iterator ppI = pFormChunk->begin();ppI!=pFormChunk->end();++ppI)
                {
                    engine::violetomega::IFFChunkSPtr pChunk = *ppI;

                    if(pChunk->id()==IFF_NAME_ID && pChunk->scan())
                    {
                        engine::violetomega::IFFTextChunkSPtr pText = pChunk.dynamicCast<engine::violetomega::IFFTextChunk>();
                        if(!pText.isNull())
                        {
                            sName = pText->text();
                        }
                    }
                    else if(pChunk->id()==IFF_AUTH_ID && pChunk->scan())
                    {
                        engine::violetomega::IFFTextChunkSPtr pText = pChunk.dynamicCast<engine::violetomega::IFFTextChunk>();
                        if(!pText.isNull())
                        {
                            sAuthor = pText->text();
                        }
                    }
                    else if(pChunk->id()==IFF_CPRT_ID && pChunk->scan())
                    {
                        engine::violetomega::IFFTextChunkSPtr pText = pChunk.dynamicCast<engine::violetomega::IFFTextChunk>();
                        if(!pText.isNull())
                        {
                            sCopyright = pText->text();
                        }
                    }
                    else if(pChunk->id()==IFF_ANNO_ID && pChunk->scan())
                    {
                        engine::violetomega::IFFTextChunkSPtr pText = pChunk.dynamicCast<engine::violetomega::IFFTextChunk>();
                        if(!pText.isNull())
                        {
                            sAnnotation = pText->text();
                        }
                    }
                    else if(pChunk->id()==IFF_ID3_ID)
                    {
                        engine::violetomega::IFFID3ChunkSPtr pID3Chunk = pChunk.dynamicCast<engine::violetomega::IFFID3Chunk>();
                        if(!pID3Chunk.isNull())
                        {
                            common::BIOMemory *idFile = pID3Chunk->file();
                            readID3(idFile);
                            delete idFile;
                        }
                    }
                }

                if(title().isEmpty() && !sName.isEmpty())
                {
                    title() = sName;
                    res = true;
                }
                else if(!title().isEmpty())
                {
                    res = true;
                }

                if(artist().isEmpty())
                {
                    artist() = sAuthor;
                }
                if(copyright().isEmpty())
                {
                    copyright() = sCopyright;
                }
                if(comment().isEmpty())
                {
                    comment() = sAnnotation;
                }

                if(m_chapters.isEmpty() && isSeparateCuesheet(input->name()))
                {
                    readCueSheet(separateCueFilename(input->name()));
                    if(!m_chapters.isEmpty())
                    {
                        res = true;
                    }
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
