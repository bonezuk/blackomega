//-------------------------------------------------------------------------------------------

bool AOBase::decodeAndResample(engine::Codec *c,AudioItem *outputItem,bool& initF)
{
    tint i,j,k,idx;
    engine::AData& dData = *(outputItem->data());
    bool res = true;
    
    if(m_resampleFlag)
    {
        engine::RData *iData = dynamic_cast<engine::RData *>(m_resampleItem->data());
        engine::RData *oData = dynamic_cast<engine::RData *>(&dData);
        
        if(m_resampleList.size()>0 && m_resampleList.last().complete())
        {
            m_rCodecCompleteFlag = true;
        }

        while(oData->rLength()>0)
        {
            bool pFlag = false;
            
            while(m_resampleList.size()>0 && m_rUsedO<m_rOutNo && oData->rLength()>0)
            {
                tint dLen,rLen;
                tfloat64 dLenActual,diff;
                tfloat32 *out;
                AOResampleInfo& dInfo = m_resampleList.first();
                engine::RData::Part& p = oData->nextPart();
                    
                dLenActual = static_cast<tfloat64>(dInfo.end() - dInfo.start()) * static_cast<tfloat64>(m_frequency);
                dLen = static_cast<int>(dLenActual);
                diff = dLenActual - static_cast<tfloat64>(dLen);
                if(diff >= 0.5)
                {
                    dLen++;
                    m_rOutDrift -= static_cast<tfloat64>(dLen) - dLenActual;
                }
                else
                {
                    m_rOutDrift += dLenActual - static_cast<tfloat64>(dLen);
                }

                if(m_rOutDrift>=1.0)
                {
                    m_rOutDrift -= 1.0;
                    dLen++;
                }
                else if(m_rOutDrift<=-1.0)
                {
                    m_rOutDrift += 1.0;
                    dLen--;
                }

                rLen = p.length();
                out = oData->partData(oData->noParts() - 1);
                
                i = decodeAndResampleInterleaveOutputChannels(out,dLen,rLen);
                if(!decodeAndResampleSetCompletePartTiming(dData,p,i,dLen))
                {
                    return false;
                }
                
                if(initF)
                {
                    p.refStartTime() = m_refStartAudioTime;
                    m_refStartAudioTime = 0;
                    initF = false;
                }
                pFlag = true;
            }
            
            if(oData->rLength()>0)
            {
                while(m_rUsedI<m_rSrcLen && !m_rCodecCompleteFlag)
                {
                    tint partNo,partOffset;
                        
                    partNo = static_cast<tint>((static_cast<tuint32>(m_resampleItem->done()) >> 22) & 0x000003ff);
                    partOffset = static_cast<tint>(static_cast<tuint32>(m_resampleItem->done()) & 0x003fffff);
                    
                    if(partNo<iData->noParts())
                    {
                        tfloat32 *in = iData->partData(partNo);
                        engine::RData::Part& p = iData->part(partNo);
                        AOResampleInfo dInfo;
                        
                        dInfo.start() = p.start() + static_cast<tfloat64>(partOffset) / static_cast<tfloat64>(m_rInFrequency);
                    
                        for(i=partOffset,idx=partOffset,j=m_rUsedI;i<p.length() && j<m_rSrcLen;++i,++j)
                        {
                            for(k=0;k<m_noInChannels;k++,idx++)
                            {
                                tfloat32 *inC = m_rIn[k];
                                inC[j] = in[idx];
                            }
                        }
                        partOffset = i;
                        m_rUsedI = j;
                        
                        if(partOffset>=p.length())
                        {
                            dInfo.end() = p.end();
                            dInfo.complete() = (p.isNext()) ? true : false;
                            partOffset = 0;
                            partNo++;
                        }
                        else
                        {
                            dInfo.end() = p.start() + static_cast<tfloat64>(partOffset) / static_cast<tfloat64>(m_rInFrequency);
                            dInfo.complete() = false;
                        }
                        m_resampleItem->setDone(static_cast<tint>(((static_cast<tuint32>(partNo) << 22) & 0xffc00000) | (static_cast<tuint32>(partOffset) &  0x003fffff)));
                        m_resampleList.append(dInfo);
                    }
                    else
                    {
                        m_resampleItem->setDone(0);
                        iData->reset();
                        
                        if(m_resampleList.size()>0 && m_resampleList.last().complete())
                        {
                            m_rCodecCompleteFlag = true;
                        }
                        else
                        {
                            if(!c->next(*iData))
                            {
                                if(iData->noParts()==0)
                                {
                                    m_rCodecCompleteFlag = true;
                                }
                                else
                                {
                                    iData->part(iData->noParts()-1).setNext(true);
                                    iData->part(iData->noParts()-1).refStartTime() = m_refStartAudioTime;
                                    m_refStartAudioTime = 0;
                                }
                            }
                        }
                    }
                    pFlag = true;
                }
            
                if(m_rUsedO>=m_rOutNo)
                {
                    tint useO;
                    bool last = (m_resampleList.size()>0) ? m_resampleList.last().complete() : m_rCodecCompleteFlag;
                                        
                    for(k=0;k<m_noInChannels;k++)
                    {
                        useO = 0;
                        m_rOutNo = m_resample[k]->process(m_resampleRatio,m_rIn[k],m_rUsedI,last,useO,m_rOut[k],m_rDstLen);
                    }
                    
                    for(i=0,j=useO;j<m_rSrcLen;++i,++j)
                    {
                        for(k=0;k<m_noInChannels;k++)
                        {
                            tfloat32 *inC = m_rIn[k];
                            inC[i] = inC[j];
                        }
                    }
                    m_rUsedI = i;
                    m_rUsedO = 0;
                }
                else if(!pFlag)
                {
                    m_rUsedI = 0;
                    m_rUsedO = m_rOutNo;
                }
            }
        }

        if(oData->noParts()>0)
        {
            oData->start() = oData->part(0).start();
            oData->end() = oData->part(oData->noParts()-1).end();
        }
    }
    else
    {
        res = c->next(dData);
        if(res && initF)
        {
            engine::RData *oData = dynamic_cast<engine::RData *>(&dData);
            if(oData->noParts()>0)
            {
                oData->part(0).refStartTime() = m_refStartAudioTime;
                m_refStartAudioTime = 0;
                initF = false;
            }
        }
        
#if !defined(OMEGA_MAC_STORE)
        if(!m_licenseFlag)
        {
            if(m_mergeCurrentPlayTime > m_mergeNextPlayTime)
            {
                if(m_mergeCodec==0)
                {
                    if(!openMergeCodec(getUnlicensedFilename()))
                    {
                        closeMergeCodec();
                    }
                }
                if(m_mergeCodec!=0)
                {
                    if(!mergeAudioWithCodec(m_mergeCodec,outputItem))
                    {
                        m_mergeNextPlayTime = m_mergeCurrentPlayTime + 10.0;
                        closeMergeCodec();
                    }
                }
                else
                {
                    QCoreApplication::quit();
                }
            }

            {
                engine::RData *oData = dynamic_cast<engine::RData *>(&dData);
                for(i=0;i<oData->noParts();i++)
                {
                    m_mergeCurrentPlayTime += static_cast<tfloat64>(oData->part(i).length()) / static_cast<tfloat64>(m_frequency);
                }
            }
        }
#endif        
    }
    dData.mixChannels();
    return res;
}

//-------------------------------------------------------------------------------------------

bool AOBase::decodeAndResample(engine::Codec *c,AudioItem *outputItem,bool& initF)
{
    tint i,j,k,idx;
    engine::AData& dData = *(outputItem->data());
    bool res = true;
    
    if(m_resampleFlag)
    {
        engine::RData *iData = dynamic_cast<engine::RData *>(m_resampleItem->data());
        engine::RData *oData = dynamic_cast<engine::RData *>(&dData);
        
        if(m_resampleList.size()>0 && m_resampleList.last().complete())
        {
            m_rCodecCompleteFlag = true;
        }

        while(oData->rLength()>0)
        {
            bool pFlag = false;
            
            while(m_resampleList.size()>0 && m_rUsedO<m_rOutNo && oData->rLength()>0)
            {
                tint dLen,rLen;
                tfloat64 dLenActual,diff;
                tfloat32 *out;
                AOResampleInfo& dInfo = m_resampleList.first();
                engine::RData::Part& p = oData->nextPart();
                
                dLen = decodeAndResampleCalculateOutputLength();

                rLen = p.length();
                out = oData->partData(oData->noParts() - 1);
                
                i = decodeAndResampleInterleaveOutputChannels(out,dLen,rLen);
                if(!decodeAndResampleSetCompletePartTiming(dData,p,i,dLen))
                {
                    return false;
                }
                
                if(initF)
                {
                    p.refStartTime() = m_refStartAudioTime;
                    m_refStartAudioTime = 0;
                    initF = false;
                }
                pFlag = true;
            }
            
            if(oData->rLength()>0)
            {
                while(m_rUsedI<m_rSrcLen && !m_rCodecCompleteFlag)
                {
                    tint partNo,partOffset;
                        
                    partNo = static_cast<tint>((static_cast<tuint32>(m_resampleItem->done()) >> 22) & 0x000003ff);
                    partOffset = static_cast<tint>(static_cast<tuint32>(m_resampleItem->done()) & 0x003fffff);
                    
                    if(partNo<iData->noParts())
                    {
                        tfloat32 *in = iData->partData(partNo);
                        engine::RData::Part& p = iData->part(partNo);
                        AOResampleInfo dInfo;
                        
                        dInfo.start() = p.start() + static_cast<tfloat64>(partOffset) / static_cast<tfloat64>(m_rInFrequency);
                    
                        for(i=partOffset,idx=partOffset,j=m_rUsedI;i<p.length() && j<m_rSrcLen;++i,++j)
                        {
                            for(k=0;k<m_noInChannels;k++,idx++)
                            {
                                tfloat32 *inC = m_rIn[k];
                                inC[j] = in[idx];
                            }
                        }
                        partOffset = i;
                        m_rUsedI = j;
                        
                        if(partOffset>=p.length())
                        {
                            dInfo.end() = p.end();
                            dInfo.complete() = (p.isNext()) ? true : false;
                            partOffset = 0;
                            partNo++;
                        }
                        else
                        {
                            dInfo.end() = p.start() + static_cast<tfloat64>(partOffset) / static_cast<tfloat64>(m_rInFrequency);
                            dInfo.complete() = false;
                        }
                        m_resampleItem->setDone(static_cast<tint>(((static_cast<tuint32>(partNo) << 22) & 0xffc00000) | (static_cast<tuint32>(partOffset) &  0x003fffff)));
                        m_resampleList.append(dInfo);
                    }
                    else
                    {
                        m_resampleItem->setDone(0);
                        iData->reset();
                        
                        if(m_resampleList.size()>0 && m_resampleList.last().complete())
                        {
                            m_rCodecCompleteFlag = true;
                        }
                        else
                        {
                            if(!c->next(*iData))
                            {
                                if(iData->noParts()==0)
                                {
                                    m_rCodecCompleteFlag = true;
                                }
                                else
                                {
                                    iData->part(iData->noParts()-1).setNext(true);
                                    iData->part(iData->noParts()-1).refStartTime() = m_refStartAudioTime;
                                    m_refStartAudioTime = 0;
                                }
                            }
                        }
                    }
                    pFlag = true;
                }
            
                if(m_rUsedO>=m_rOutNo)
                {
                    tint useO;
                    bool last = (m_resampleList.size()>0) ? m_resampleList.last().complete() : m_rCodecCompleteFlag;
                                        
                    for(k=0;k<m_noInChannels;k++)
                    {
                        useO = 0;
                        m_rOutNo = m_resample[k]->process(m_resampleRatio,m_rIn[k],m_rUsedI,last,useO,m_rOut[k],m_rDstLen);
                    }
                    
                    for(i=0,j=useO;j<m_rSrcLen;++i,++j)
                    {
                        for(k=0;k<m_noInChannels;k++)
                        {
                            tfloat32 *inC = m_rIn[k];
                            inC[i] = inC[j];
                        }
                    }
                    m_rUsedI = i;
                    m_rUsedO = 0;
                }
                else if(!pFlag)
                {
                    m_rUsedI = 0;
                    m_rUsedO = m_rOutNo;
                }
            }
        }

        if(oData->noParts()>0)
        {
            oData->start() = oData->part(0).start();
            oData->end() = oData->part(oData->noParts()-1).end();
        }
    }
    else
    {
        res = c->next(dData);
        if(res && initF)
        {
            engine::RData *oData = dynamic_cast<engine::RData *>(&dData);
            if(oData->noParts()>0)
            {
                oData->part(0).refStartTime() = m_refStartAudioTime;
                m_refStartAudioTime = 0;
                initF = false;
            }
        }
        
#if !defined(OMEGA_MAC_STORE)
        if(!m_licenseFlag)
        {
            if(m_mergeCurrentPlayTime > m_mergeNextPlayTime)
            {
                if(m_mergeCodec==0)
                {
                    if(!openMergeCodec(getUnlicensedFilename()))
                    {
                        closeMergeCodec();
                    }
                }
                if(m_mergeCodec!=0)
                {
                    if(!mergeAudioWithCodec(m_mergeCodec,outputItem))
                    {
                        m_mergeNextPlayTime = m_mergeCurrentPlayTime + 10.0;
                        closeMergeCodec();
                    }
                }
                else
                {
                    QCoreApplication::quit();
                }
            }

            {
                engine::RData *oData = dynamic_cast<engine::RData *>(&dData);
                for(i=0;i<oData->noParts();i++)
                {
                    m_mergeCurrentPlayTime += static_cast<tfloat64>(oData->part(i).length()) / static_cast<tfloat64>(m_frequency);
                }
            }
        }
#endif        
    }
    dData.mixChannels();
    return res;
}

//-------------------------------------------------------------------------------------------

int AOBase::decodeAndResampleInterleaveOutputChannelsForNextPart(engine::AData& dData,engine::RData *oData,tint dLen)
{
    engine::RData::Part& p = oData->nextPart();
    tint rLen = p.length();
    tfloat32 *out = oData->partData(oData->noParts() - 1);
    return decodeAndResampleInterleaveOutputChannels(out,dLen,rLen);
}


//-------------------------------------------------------------------------------------------

bool AOBase::decodeAndResampleDoPartOutputFromResampler(engine::AData& dData,engine::RData *oData,bool& initF,bool& pFlag)
{
    bool res = true;
    
    while(res && m_resampleList.size()>0 && m_rUsedO<m_rOutNo && oData->rLength()>0)
    {
        tint dLen,rLen;
        tfloat64 dLenActual,diff;
        tfloat32 *out;
        AOResampleInfo& dInfo = m_resampleList.first();
        engine::RData::Part& p = oData->nextPart();
                
        dLen = decodeAndResampleCalculateOutputLength();

        rLen = p.length();
        out = oData->partData(oData->noParts() - 1);
        
        i = decodeAndResampleInterleaveOutputChannels(out,dLen,rLen);
        if(decodeAndResampleSetCompletePartTiming(dData,p,i,dLen))
        {
            if(initF)
            {
                p.refStartTime() = m_refStartAudioTime;
                m_refStartAudioTime = 0;
                initF = false;
            }
            pFlag = true;
        }
        else
        {
            return false;
        }
    }
    return res;
}

int AOBase::decodeAndResampleInterleaveOutputChannelsForNextPart(AudioItem *outputItem,tint dLen)
{
    engine::RData *data= dynamic_cast<engine::RData *>(outputItem->data());
    engine::RData::Part& p = data->nextPart();
    tint rLen = p.length();
    tfloat32 *out = oData->partData(oData->noParts() - 1);
    return decodeAndResampleInterleaveOutputChannels(out,dLen,rLen);
}


bool AOBase::decodeAndResampleSetCompleteLastPartTiming(AudioItem *outputItem,tint idx,tint dLen)
{
    engine::AData& dData = *(outputItem->data());
    engine::RData *rData = dynamic_cast<engine::RData *>(outputItem->data());
    engine::RData::Part& p = rData->part(rData->noParts()-1);
    return decodeAndResampleSetCompletePartTiming(dData,p,idx,dLen);
}

bool AOBase::decodeAndResampleDoPartOutputFromResampler(AudioItem *outputItem,bool& pFlag)
{
    engine::AData& dData = *(outputItem->data());
    bool res = true;
    
    while(res && m_resampleList.size()>0 && m_rUsedO<m_rOutNo && oData->rLength()>0)
    {
        tint dLen,index;
        
        dLen = decodeAndResampleCalculateOutputLength();
        index = decodeAndResampleInterleaveOutputChannelsForNextPart(outputItem,dLen);
        if(decodeAndResampleSetCompleteLastPartTiming(outputItem,index,dLen))
        {
            if(initF)
            {
                p.refStartTime() = m_refStartAudioTime;
                m_refStartAudioTime = 0;
                initF = false;
            }
            pFlag = true;
        }
        else
        {
            res = false;
        }
    }
    return res;
}


//-------------------------------------------------------------------------------------------

const tint& getNoInChannels() const
void setNoInChannels(tint noChs)
const tint& getNoOutChannels() const
void setNoOutChannels(tint noChs)

const bool& getResampleFlag() const
void setResampleFlag(bool flag)
const tfloat64& getResampleRatio() const
void setResampleRatio(tfloat64 v)
AudioItem *getResampleItem()
void setResampleItem(AudioItem *item)
engine::Resample *getResampler(tint ch)
void setResampler(engine::Resample *resampler,tint ch)
const tint& getRInFrequency() const
void setRInFrequency(tint freq)
const tint& getRUsedI() const
void setRUsedI(tint usedI)
const tint& getRUsedO() const
void setRUsedO(tint usedO)
const tint& getRSrcLen() const
void setRSrcLen(tint len)
const tint& getRDstLen() const
void setRDstLen(tint len)
const tint& getROutNo() const
void setROutNo(tint no)
tfloat32 *getRIn(tint ch)
void setRIn(tfloat32 *in,tint ch)
tfloat32 *getROut(tint ch)
void setROut(tfloat32 *out,tint ch)
const tfloat64& getROutDrift() const
void setROutDrift(tfloat64 drift)
const bool& getRCodecCompleteFlag() const
void setRCodecCompleteFlag(bool flag)
QList<AOResampleInfo>& resampleList()
const QList<AOResampleInfo>& resampleListConst() const


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

bool AOBase::processCodecCrossFade(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF)
{
    tint i;
    bool flag,loop = true;
    engine::RData *data = dynamic_cast<engine::RData *>(item->data());
    engine::RData *nextData;

    nextData = dynamic_cast<engine::RData *>(m_crossFadeItem->data());

    if(m_nextCodec!=0)
    {
        bool nextFlag,flag;
        common::TimeStamp endT;
        
        flag = processCodecCrossFadeDecode(pItem,currentT,initF);
        flag = processCodecCrossFadeUpdateCodecLength(pItem,flag);
        
        if(m_nextCodec->isRemote())
        {
        
        }
        else
        {
        
        }
        
    }
    else
    {
        m_state = e_statePlay;
    }
    return loop;
}

void AOBase::processCodecCrossFadeRemote(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF)
{
    tfloat32 percent = 0.0f;

    if(m_nextCodec->isBuffered(percent))
    {
        tint nState = 0;
        common::TimeStamp tE;
        
        if(m_frameFadeTime > m_crossFadeTime)
        {
            tE = m_frameFadeTime - m_crossFadeTime;
        }
        
        resetNextCrossData(nextData,tE);
        
        do
        {
            if(nextData->rLength() > 0)
            {
                nextFlag = decodeAndResample(m_nextCodec,m_crossFadeItem,initF);
            }
            else
            {
                nState = 2;
            }
            
            switch(nState)
            {
                case 0:
                    if(nextFlag)
                    {
                        for(i=0;i<nextData->noParts();++i)
                        {
                            if(tE<nextData->part(i).start())
                            {
                                break;
                            }
                            else if(nextData->part(i).start()>=tE && tE<nextData->part(i).end())
                            {
                                break;
                            }
                        }
                        if(i<nextData->noParts())
                        {
                            resetNextCrossData(nextData,tE);
                            nState = 1;
                        }
                    }
                    else
                    {
                        nextFlag = (!m_nextCodec->isComplete()) ? true : false;
                    }
                    break;

                case 1:
                    if(!nextFlag)
                    {
                        nextFlag = (!m_nextCodec->isComplete()) ? true : false;
                        nState = 2;
                    }
                    break;
                }
        } while(nState < 2);
        
        crossFade(data,nextData,m_frameFadeTime);
    }
    else
    {
        m_frameFadeTime = data->end();
        nextFlag = true;
    }
}

bool AOBase::processCodecCrossFadeLocal(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF)
{
            // Resample codec
            nextFlag = decodeAndResample(m_nextCodec,m_crossFadeItem,initF);
            crossFade(data,nextData,m_frameFadeTime);
            if(m_audioProcessType==1)
            {
                resetNextCrossData(nextData,m_frameFadeTime);
            }
            else
            {
                nextData->reset();
            }
            m_audioProcessType = 0;
    return nextFlag;
}

bool AOBase::processCodecCrossFadeDecode(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF)
{
    bool flag;
    
    if(m_audioProcessType!=1)
    {
        flag = decodeAndResample(m_codec,item,initF);
        m_codecCurrentTime = data->part(data->noParts() - 1).end();
    }
    return flag;
}

bool AOBase::processCodecCrossFadeUpdateCodecLength(AudioItem **pItem,bool flag)
{
    AudioItem *item = *pItem;
    engine::RData *data = dynamic_cast<engine::RData *>(item->data());

    if(flag)
    {
        for(i=0;i<data->noParts();++i)
        {
            if(m_codecTimeLength<data->part(i).end())
            {
                if(m_codecTimeLengthUpdate)
                {
                    m_codecTimeLength = data->part(i).end();
                }
                else
                {
                    flag = false;
                }
            }
        }
    }
    else
    {
        if(m_codecTimeLengthUpdate)
        {
            m_codecTimeLength = data->part(data->noParts() - 1).end();
        }
        m_trackTimeState = 0;
    }
    return flag;
}


//-------------------------------------------------------------------------------------------

bool AOBase::processCodecCrossFade(AudioItem* item,const common::TimeStamp& currentT,bool& initF)
{
    tint i;
    bool flag,loop = true;
    engine::RData *data = dynamic_cast<engine::RData *>(item->data());
    engine::RData *nextData;

    nextData = dynamic_cast<engine::RData *>(m_crossFadeItem->data());

    if(m_nextCodec!=0)
    {
        bool nextFlag;
        common::TimeStamp endT;

        if(m_audioProcessType!=1)
        {
            flag = decodeAndResample(m_codec,item,initF);
            m_codecCurrentTime = data->part(data->noParts() - 1).end();
        }

        if(flag)
        {
            for(i=0;i<data->noParts();++i)
            {
                if(m_codecTimeLength<data->part(i).end())
                {
                    if(m_codecTimeLengthUpdate)
                    {
                        m_codecTimeLength = data->part(i).end();
                    }
                    else
                    {
                        flag = false;
                    }
                }
            }
        }
        else
        {
            if(m_codecTimeLengthUpdate)
            {
                m_codecTimeLength = data->part(data->noParts() - 1).end();
            }
            m_trackTimeState = 0;
        }

        if(m_nextCodec->isRemote())
        {
            tfloat32 percent = 0.0f;

            if(m_nextCodec->isBuffered(percent))
            {
                tint nState = 0;
                common::TimeStamp tE;

                if(m_frameFadeTime > m_crossFadeTime)
                {
                    tE = m_frameFadeTime - m_crossFadeTime;
                }

                resetNextCrossData(nextData,tE);

                do
                {
                    if(nextData->rLength() > 0)
                    {
                        nextFlag = decodeAndResample(m_nextCodec,m_crossFadeItem,initF);
                    }
                    else
                    {
                        nState = 2;
                    }

                    switch(nState)
                    {
                        case 0:
                        if(nextFlag)
                        {
                            for(i=0;i<nextData->noParts();++i)
                            {
                                if(tE<nextData->part(i).start())
                                {
                                    break;
                                }
                                else if(nextData->part(i).start()>=tE && tE<nextData->part(i).end())
                                {
                                    break;
                                }
                            }

                            if(i<nextData->noParts())
                            {
                                resetNextCrossData(nextData,tE);
                                nState = 1;
                            }
                        }
                        else
                        {
                            nextFlag = (!m_nextCodec->isComplete()) ? true : false;
                        }
                        break;

                        case 1:
                        if(!nextFlag)
                        {
                            nextFlag = (!m_nextCodec->isComplete()) ? true : false;
                            nState = 2;
                        }
                        break;
                    }
                } while(nState < 2);

                crossFade(data,nextData,m_frameFadeTime);
            }
            else
            {
                m_frameFadeTime = data->end();
                nextFlag = true;
            }
        }
        else
        {
            // Resample codec
            nextFlag = decodeAndResample(m_nextCodec,m_crossFadeItem,initF);
            crossFade(data,nextData,m_frameFadeTime);
            if(m_audioProcessType==1)
            {
                resetNextCrossData(nextData,m_frameFadeTime);
            }
            else
            {
                nextData->reset();
            }
            m_audioProcessType = 0;
        }

        if(flag)
        {
            if(nextFlag)
            {
                postProcess(data);
                item->setState(AudioItem::e_stateFull);
            }
            else
            {
                postProcess(data);
                item->setState(AudioItem::e_stateFull);
                m_state = e_statePlay;
                stopNextCodec();
            }
        }
        else
        {
            engine::RData::Part& p = data->part(data->noParts() - 1);
            p.start() = p.startNext();
            p.end() = p.endNext();
            p.setNext(true);
            p.refStartTime() = m_refStartAudioTime;
            m_refStartAudioTime = 0;

            if(nextFlag)
            {
                if(stopCodecDoNext())
                {
                    postProcess(data);
                    item->setState(AudioItem::e_stateFull);
                }
                else
                {
                    postProcess(data);
                    item->setState(AudioItem::e_stateFullEnd);
                }
            }
            else
            {
                postProcess(data);
                item->setState(AudioItem::e_stateFullEnd);
                m_state = e_stateNoCodec;
            }
        }
    }
    else
    {
        m_state = e_statePlay;
    }
    return loop;
}

