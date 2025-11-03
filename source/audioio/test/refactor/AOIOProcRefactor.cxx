//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudio::callbackIOProcI(AudioUnitRenderActionFlags *ioActionFlags, \
                                      const AudioTimeStamp *inTimeStamp, \
                                      UInt32 inBusNumber, \
                                      UInt32 inNumberFrames, \
                                      AudioBufferList *ioData)
{
    tint noChs;
    tint i = 0,j,k,len = inNumberFrames;
    common::TimeStamp streamT,tS;
    tfloat64 *mem;

    m_mutexCount++;

    streamT.nano64(AudioConvertHostTimeToNanos(inTimeStamp->mHostTime));
    if(!m_audioStartFlag)
    {
        m_audioStartClock = streamT - m_pauseTime;
        m_audioStartFlag = true;
        if(!m_trackTimeStateFlag)
        {
            m_trackTimeState = 1;
        }
        m_trackTimeStateFlag = false;
    }
    tS = streamT - m_audioStartClock;
    m_currentPlayTime.set(tS);

    if(m_audioStartFlag && (m_state==e_statePlay || m_state==e_statePreBuffer || m_state==e_stateCrossFade || m_state==e_stateNoCodec))
    {
        tint offset,amount,total;
        AudioItem *item = m_callbackAudioItem, *oItem = m_callbackAudioItem;
        bool loop = true,loopFlag = false;

        while(i<len && loop && !(loopFlag && item==oItem))
        {
            if(item->state()==AudioItem::e_stateFull || item->state()==AudioItem::e_stateFullEnd)
            {
                if(item->state()==AudioItem::e_stateFull)
                {
                    item->setState(AudioItem::e_stateCallback);
                }
                else
                {
                    item->setState(AudioItem::e_stateCallbackEnd);
                }
                m_callbackAudioItem = item;
            }

            if(item->state()==AudioItem::e_stateCallback || item->state()==AudioItem::e_stateCallbackEnd)
            {
                tint pNo;
                engine::RData *data = dynamic_cast<engine::RData *>(item->data());

                pNo = static_cast<tint>((static_cast<tuint32>(item->done()) >> 22) & 0x000003ff);
                if(pNo < data->noParts())
                {
                    engine::RData::Part& part = data->part(pNo);

                    if(part.isNext())
                    {
                        common::TimeStamp iT(static_cast<tfloat64>(i)/static_cast<tfloat64>(m_frequency));
                        m_audioStartClock = streamT + iT - part.start();
                        m_currentOutTime = part.start();
                        m_currentPlayTime.set(part.start());
                        m_currentCallbackTime.set(part.start());
                        if(!m_trackTimeStateFlag)
                        {
                            m_trackTimeState = 1;
                        }
                        m_trackTimeStateFlag = false;
                        m_startNextTrackFlag = true;
                    }

                    if(part.refStartTime()!=0)
                    {
                        common::TimeStamp dT,cRefTS = common::TimeStamp::reference();

                        if(cRefTS >= part.refStartTime())
                        {
                            dT = cRefTS - part.refStartTime();
                            m_currentOutTime = dT + part.start();
                            m_audioStartClock = streamT - m_currentOutTime;
                            m_currentPlayTime.set(m_currentOutTime);
                            part.refStartTime() = 0;
                        }
                        else
                        {
                            dT = part.refStartTime() - cRefTS;
                            amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
                            if(amount > (len - i))
                            {
                                amount = len - i;
                                dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
                            }

                            j = 0;
                            k = 0;
                            while(j<m_noChannels)
                            {
                                tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
                                noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                                ::memset(&out[i * noChs],0,amount * noChs * sizeof(tfloat32));
                                j += noChs;
                                k++;
                            }
                            m_currentOutTime += dT;
                            i += amount;
                        }
                    }
                    else if(m_currentOutTime >= part.start())
                    {
                        if(m_currentOutTime < part.end())
                        {
                            tint pNoChannels = data->noOutChannels();

                            offset = static_cast<tint>(static_cast<tuint32>(item->done()) & 0x003fffff);
                            if(!offset)
                            {
                                offset = static_cast<tint>(static_cast<tfloat64>(m_currentOutTime - part.start()) * static_cast<tfloat64>(m_frequency));
                            }

                            mem = data->partDataOut(pNo);
                            total = part.length();

                            if(offset>=0 && offset<total)
                            {
                                amount = total - offset;

                                if(amount > (len - i))
                                {
                                    amount = len - i;
                                }

                                if(amount > 0)
                                {
                                    common::TimeStamp dT(static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency));

                                    {
                                        tint iA,iB,jC,tAmount;

                                        j = 0;
                                        k = 0;
                                        while(j<m_noChannels)
                                        {
                                            tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);

                                            noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                                            tAmount = amount * noChs;
                                            out = &out[i * noChs];

                                            for(jC=0;jC<noChs && j<m_noChannels;jC++)
                                            {
                                                int cIdx = m_outputChannelArray[j];
                                                iA = jC;
                                                iB = (offset * pNoChannels) + cIdx;
                                                if(cIdx>=0)
                                                {
                                                    while(iA < tAmount)
                                                    {
                                                        out[iA] = engine::sampleToFloat32(mem[iB] * m_volume);
                                                        iA += noChs;
                                                        iB += pNoChannels;
                                                    }
                                                }
                                                else
                                                {
                                                    while(iA < tAmount)
                                                    {
                                                        out[iA] = 0.0f;
                                                        iA += noChs;
                                                    }
                                                }
                                                j++;
                                            }
                                            k++;
                                        }
                                    }

                                    m_currentOutTime += dT;
                                    i += amount;
                                }

                                offset += amount;

                                if(offset >= total)
                                {
                                    m_currentOutTime = part.end();
                                    offset = 0;
                                    pNo++;
                                }
                            }
                            else
                            {
                                if(offset < 0)
                                {
                                    m_currentOutTime = part.start();
                                    offset = 0;
                                }
                                else
                                {
                                    m_currentOutTime = part.end();
                                    offset = 0;
                                    pNo++;
                                }
                            }
                        }
                        else
                        {
                            offset = 0;
                            pNo++;
                        }
                        item->setDone(static_cast<tint>(((static_cast<tuint32>(pNo) << 22) & 0xffc00000) | (static_cast<tuint32>(offset) & 0x003fffff)));
                    }
                    else
                    {
                        common::TimeStamp dT(part.start() - m_currentOutTime);

                        amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
                        if(amount > (len - i))
                        {
                            amount = len - i;
                            dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
                        }

                        if(amount>0)
                        {
                            j = 0;
                            k = 0;
                            while(j<m_noChannels)
                            {
                                tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
                                noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                                ::memset(&out[i * noChs],0,amount * noChs * sizeof(tfloat32));
                                j += noChs;
                                k++;
                            }
                        }

                        m_currentOutTime += dT;
                        i += amount;
                    }
                }
                else
                {
                    if(item->state()==AudioItem::e_stateCallbackEnd)
                    {
                        common::TimeStamp sT;
                        sT = common::TimeStamp::reference() + m_outputLatencyTime + (static_cast<tfloat64>(i) / static_cast<tfloat64>(m_frequency));
                        m_stopTimeClock.set(sT);
                        m_stopTimeFlag = true;
                        loop = false;
                    }
                    item->setState(AudioItem::e_stateDone);
                    item = item->next();
                    loopFlag = true;
                }
            }
            else if(item->state()==AudioItem::e_stateDone)
            {
                item = item->next();
                loopFlag = true;
            }
            else
            {
                break;
            }
        }

        if(i<len)
        {
            common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

            j = 0;
            k = 0;
            while(j<m_noChannels)
            {
                tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
                noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                ::memset(&out[i * noChs],0,(len - i) * noChs * sizeof(tfloat32));
                j += noChs;
                k++;
            }

            m_currentOutTime += dT;
        }
    }
    else
    {
        j = 0;
        k = 0;
        while(j<m_noChannels)
        {
            noChs = ioData->mBuffers[k].mNumberChannels;
            ::memset(ioData->mBuffers[k].mData,0,inNumberFrames * noChs * sizeof(tfloat32));
            j += noChs;
            k++;
        }
    }

    m_currentCallbackTime.set(m_currentOutTime);
    m_mutexCount++;

    return noErr;
}

//-------------------------------------------------------------------------------------------

ASIOTime *AOASIO::bufferSwitchTimeInfoI(ASIOTime *timeInfo,long index,ASIOBool processNow)
{
    tint i = 0,len = m_bufferSizePref;
    common::TimeStamp streamT;

    m_mutexCount++;

    if(timeInfo->timeInfo.flags & kSystemTimeValid)
    {
        common::TimeStamp tS;

        streamT = getTimeFromASIO(&(timeInfo->timeInfo.systemTime));

        if(!m_audioStartFlag)
        {
            m_audioStartClock = streamT - m_pauseTime;
            m_audioStartFlag = true;
            if(!m_trackTimeStateFlag)
            {
                m_trackTimeState = 1;
            }
            m_trackTimeStateFlag = false;
        }
        tS = streamT - m_audioStartClock;
        m_currentPlayTime.set(tS);
    }

    if(m_audioStartFlag && (m_state==e_statePlay || m_state==e_statePreBuffer || m_state==e_stateCrossFade || m_state==e_stateNoCodec))
    {
        writeToAudio(index,streamT);
    }
    else
    {
        for(i=0;i<m_noOutChannels;++i)
        {
            ::memset(m_bufferInfos[i].buffers[index],0,m_bufferSizePref * ASIODriverService::getSampleSize(m_channelInfos[i].type));
        }
    }

    if(m_driverPostOutput)
    {
        m_driver->ASIOOutputReady();
    }

    m_mutexCount++;
    return 0L;
}

//-------------------------------------------------------------------------------------------

void AOASIO::writeToAudio(tint index,const common::TimeStamp& streamT)
{
    tint i=0,j,len = m_bufferSizePref;
    tint offset,amount,total;
    AudioItem *item = m_callbackAudioItem, *oItem = m_callbackAudioItem;
    bool loop = true,loopFlag = false;

    while(i<len && loop && !(loopFlag && item==oItem))
    {
        if(item->state()==AudioItem::e_stateFull || item->state()==AudioItem::e_stateFullEnd)
        {
            if(item->state()==AudioItem::e_stateFull)
            {
                item->setState(AudioItem::e_stateCallback);
            }
            else
            {
                item->setState(AudioItem::e_stateCallbackEnd);
            }
            m_callbackAudioItem = item;
        }

        if(item->state()==AudioItem::e_stateCallback || item->state()==AudioItem::e_stateCallbackEnd)
        {
            tint pNo,sSize;
            tubyte *mem,*out;
            ASIOData *data = dynamic_cast<ASIOData *>(item->data());

            data->setVolume(m_volume);
            data->convert();

            sSize = data->getSampleSize();
            pNo = static_cast<tint>((static_cast<tuint32>(item->done()) >> 22) & 0x000003ff);
            if(pNo < data->noParts())
            {
                engine::RData::Part& part = data->part(pNo);

                if(part.isNext())
                {
                    common::TimeStamp iT(static_cast<tfloat64>(i)/static_cast<tfloat64>(m_frequency));
                    m_audioStartClock = streamT + iT - part.start();
                    m_currentOutTime = part.start();
                    m_currentPlayTime.set(part.start());
                    m_currentCallbackTime.set(part.start());
                    if(!m_trackTimeStateFlag)
                    {
                        m_trackTimeState = 1;
                    }
                    m_trackTimeStateFlag = false;
                    m_startNextTrackFlag = true;
                }

                if(part.refStartTime()!=0)
                {
                    common::TimeStamp dT,cRefTS = common::TimeStamp::reference();

                    if(cRefTS >= part.refStartTime())
                    {
                        dT = cRefTS - part.refStartTime();
                        m_currentOutTime = dT + part.start();
                        m_audioStartClock = streamT - m_currentOutTime;
                        m_currentPlayTime.set(m_currentOutTime);
                        m_currentCallbackTime.set(m_currentOutTime);
                        part.refStartTime() = 0;
                    }
                    else
                    {
                        dT = part.refStartTime() - cRefTS;
                        amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
                        if(amount > (len - i))
                        {
                            amount = len - i;
                            dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
                        }

                        for(j=0;j<m_noChannels;++j)
                        {
                            out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
                            ::memset(&out[i * sSize],0,amount * sSize);
                        }
                        m_currentOutTime += dT;
                        i += amount;
                    }
                }
                else if(m_currentOutTime >= part.start())
                {
                    if(m_currentOutTime < part.end())
                    {
                        offset = static_cast<tint>(static_cast<tuint32>(item->done()) & 0x003fffff);
                        if(!offset)
                        {
                            offset = static_cast<tint>(static_cast<tfloat64>(m_currentOutTime - part.start()) * static_cast<tfloat64>(m_frequency));
                        }

                        total = part.length();
                        if(offset>=0 && offset<total)
                        {
                            amount = total - offset;

                            if(amount > (len - i))
                            {
                                amount = len - i;
                            }

                            if(amount > 0)
                            {
                                common::TimeStamp dT(static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency));

                                for(j=0;j<m_noChannels;++j)
                                {
                                    int idx = m_outputChannelArray[j];
                                    out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
                                    if(idx>=0)
                                    {
                                        mem = reinterpret_cast<tubyte *>(data->asioData(idx,pNo));
                                        if(mem!=0)
                                        {
                                            ::memcpy(&out[i * sSize],&mem[offset * sSize],amount * sSize);
                                        }
                                        else
                                        {
                                            ::memset(&out[i * sSize],0,amount * sSize);
                                        }
                                    }
                                    else
                                    {
                                        ::memset(&out[i * sSize],0,amount * sSize);
                                    }
                                }
                                m_currentOutTime += dT;
                                i += amount;
                            }

                            offset += amount;

                            if(offset >= total)
                            {
                                m_currentOutTime = part.end();
                                offset = 0;
                                pNo++;
                            }
                        }
                        else
                        {
                            if(offset < 0)
                            {
                                m_currentOutTime = part.start();
                                offset = 0;
                            }
                            else
                            {
                                m_currentOutTime = part.end();
                                offset = 0;
                                pNo++;
                            }
                        }
                    }
                    else
                    {
                        offset = 0;
                        pNo++;
                    }
                    item->setDone(static_cast<tint>(((static_cast<tuint32>(pNo) << 22) & 0xffc00000) | (static_cast<tuint32>(offset) &  0x003fffff)));
                }
                else
                {
                    common::TimeStamp dT(part.start() - m_currentOutTime);

                    amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
                    if(amount > (len - i))
                    {
                        amount = len - i;
                        dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
                    }

                    if(amount>0)
                    {
                        for(j=0;j<m_noChannels;++j)
                        {
                            out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
                            ::memset(&out[i * sSize],0,amount * sSize);
                        }
                    }
                    m_currentOutTime += dT;
                    i += amount;
                }
            }
            else
            {
                if(item->state()==AudioItem::e_stateCallbackEnd)
                {
                    common::TimeStamp sT;
                    sT = common::TimeStamp::reference() + m_outputLatencyTime + (static_cast<tfloat64>(i) / static_cast<tfloat64>(m_frequency));
                    m_stopTimeClock.set(sT);
                    m_stopTimeFlag = true;
                    loop = false;
                }
                item->setState(AudioItem::e_stateDone);
                item = item->next();
                loopFlag = true;
            }
        }
        else if(item->state()==AudioItem::e_stateDone)
        {
            item = item->next();
            loopFlag = true;
        }
        else
        {
            break;
        }
    }

    if(i<len)
    {
        common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

        for(j=0;j<m_noChannels;++j)
        {
            tint sSize = ASIODriverService::instance().getSampleSize(m_channelInfos[j].type);
            tubyte *out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
            ::memset(&out[i * sSize],0,(len - i) * sSize);
        }
        m_currentOutTime += dT;
    }

    m_currentCallbackTime.set(m_currentOutTime);
}


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

ASIOTime *AOASIO::bufferSwitchTimeInfoI(ASIOTime *timeInfo,long index,ASIOBool processNow)
{
    AudioHardwareBufferASIO buffer(m_bufferInfos,m_channelInfos,m_bufferSizePref,m_noChannels,index);
    IOTimeStamp systemTime;

    incrementMutexCount();
    systemTime = createIOTimeStamp(timeInfo);
    updateCurrentPlayTimeFromStreamTime(systemTime);

    if(m_audioStartFlag && (m_state==e_statePlay || m_state==e_statePreBuffer || m_state==e_stateCrossFade || m_state==e_stateNoCodec))
    {
        writeToAudio(index,streamT);
    }
    else
    {
        writeToAudioSilence(index);
    }
    writeToAudioPostProcess();

    m_currentCallbackTime.set(m_currentOutTime);
    incrementMutexCount();
    return 0L;
}

void AOASIO::writeToAudioPostProcess()
{
    if(m_driverPostOutput)
    {
        m_driver->ASIOOutputReady();
    }
}

void AOASIO::writeToAudioSilence(tint index)
{
    for(tint i=0;i<m_noOutChannels;++i)
    {
        ::memset(m_bufferInfos[i].buffers[index],0,m_bufferSizePref * ASIODriverService::getSampleSize(m_channelInfos[i].type));
    }
}

void AOASIO::writeToAudio(tint index,const IOTimeStamp& systemTime)
{
    tint i=0,len = m_bufferSizePref;
    tint offset,amount,total;
    AudioItem *item = m_callbackAudioItem, *oItem = m_callbackAudioItem;
    bool loop = true,loopFlag = false;

    while(i<len && loop && !(loopFlag && item==oItem))
    {
        setItemStateToCallbackAsApplicable(item);

        if(item->state()==AudioItem::e_stateCallback || item->state()==AudioItem::e_stateCallbackEnd)
        {
            item = writeToAudioFromItem(item,systemTime,index,i,len,loop,loopFlag);
        }
        else if(item->state()==AudioItem::e_stateDone)
        {
            item = item->next();
            loopFlag = true;
        }
        else
        {
            break;
        }
    }

    if(i<len)
    {
        writeToAudioSilenceForRemainder(index,i,len);
    }
}

void AOASIO::setItemStateToCallback(AudioItem *item)
{
    if(item->state()==AudioItem::e_stateFull || item->state()==AudioItem::e_stateFullEnd)
    {
        if(item->state()==AudioItem::e_stateFull)
        {
            item->setState(AudioItem::e_stateCallback);
        }
        else
        {
            item->setState(AudioItem::e_stateCallbackEnd);
        }
        m_callbackAudioItem = item;
    }
}

AudioItem *AOASIO::writeToAudioFromItem(AudioItem *item,const IOTimeStamp& systemTime,tint index,tint& i,tint len,bool& loop,bool& loopFlag)
{
    tint j,pNo,sSize,amount;
    tubyte *mem,*out;
    ASIOData *data = dynamic_cast<ASIOData *>(item->data());

    data->setVolume(m_volume);
    data->convert();

    sSize = data->getSampleSize();
    pNo = static_cast<tint>((static_cast<tuint32>(item->done()) >> 22) & 0x000003ff);
    if(pNo < data->noParts())
    {
        engine::RData::Part& part = data->part(pNo);

        if(part.isNext())
        {
            common::TimeStamp iT(static_cast<tfloat64>(i)/static_cast<tfloat64>(m_frequency));
            m_audioStartClock = systemTime.time() + iT - part.start();
            m_currentOutTime = part.start();
            m_currentPlayTime.set(part.start());
            m_currentCallbackTime.set(part.start());
            if(!m_trackTimeStateFlag)
            {
                m_trackTimeState = 1;
            }
            m_trackTimeStateFlag = false;
            m_startNextTrackFlag = true;
        }

        if(part.refStartTime()!=0)
        {
            common::TimeStamp dT,cRefTS = common::TimeStamp::reference();

            if(cRefTS >= part.refStartTime())
            {
                dT = cRefTS - part.refStartTime();
                m_currentOutTime = dT + part.start();
                m_audioStartClock = systemTime.time() - m_currentOutTime;
                m_currentPlayTime.set(m_currentOutTime);
                m_currentCallbackTime.set(m_currentOutTime);
                part.refStartTime() = 0;
            }
            else
            {
                tint amount;

                dT = part.refStartTime() - cRefTS;
                amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
                if(amount > (len - i))
                {
                    amount = len - i;
                    dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
                }

                for(j=0;j<m_noChannels;++j)
                {
                    out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
                    ::memset(&out[i * sSize],0,amount * sSize);
                }
                m_currentOutTime += dT;
                i += amount;
            }
        }
        else if(m_currentOutTime >= part.start())
        {
            tint offset;

            if(m_currentOutTime < part.end())
            {
                offset = static_cast<tint>(static_cast<tuint32>(item->done()) & 0x003fffff);
                if(!offset)
                {
                    offset = static_cast<tint>(static_cast<tfloat64>(m_currentOutTime - part.start()) * static_cast<tfloat64>(m_frequency));
                }

                total = part.length();
                if(offset>=0 && offset<total)
                {
                    tint amount;

                    amount = total - offset;
                    if(amount > (len - i))
                    {
                        amount = len - i;
                    }

                    if(amount > 0)
                    {
                        common::TimeStamp dT(static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency));

                        for(j=0;j<m_noChannels;++j)
                        {
                            int idx = m_outputChannelArray[j];
                            out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
                            if(idx>=0)
                            {
                                mem = reinterpret_cast<tubyte *>(data->asioData(idx,pNo));
                                if(mem!=0)
                                {
                                    ::memcpy(&out[i * sSize],&mem[offset * sSize],amount * sSize);
                                }
                                else
                                {
                                    ::memset(&out[i * sSize],0,amount * sSize);
                                }
                            }
                            else
                            {
                                ::memset(&out[i * sSize],0,amount * sSize);
                            }
                        }
                        m_currentOutTime += dT;
                        i += amount;
                    }

                    offset += amount;

                    if(offset >= total)
                    {
                        m_currentOutTime = part.end();
                        offset = 0;
                        pNo++;
                    }
                }
                else
                {
                    if(offset < 0)
                    {
                        m_currentOutTime = part.start();
                        offset = 0;
                    }
                    else
                    {
                        m_currentOutTime = part.end();
                        offset = 0;
                        pNo++;
                    }
                }
            }
            else
            {
                offset = 0;
                pNo++;
            }
            item->setDone(static_cast<tint>(((static_cast<tuint32>(pNo) << 22) & 0xffc00000) | (static_cast<tuint32>(offset) &  0x003fffff)));
        }
        else
        {
            tint amount;
            common::TimeStamp dT(part.start() - m_currentOutTime);

            amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
            if(amount > (len - i))
            {
                amount = len - i;
                dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
            }

            if(amount>0)
            {
                for(j=0;j<m_noChannels;++j)
                {
                    out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
                    ::memset(&out[i * sSize],0,amount * sSize);
                }
            }
            m_currentOutTime += dT;
            i += amount;
        }
    }
    else
    {
        if(item->state()==AudioItem::e_stateCallbackEnd)
        {
            common::TimeStamp sT;
            sT = common::TimeStamp::reference() + m_outputLatencyTime + (static_cast<tfloat64>(i) / static_cast<tfloat64>(m_frequency));
            m_stopTimeClock.set(sT);
            m_stopTimeFlag = true;
            loop = false;
        }
        item->setState(AudioItem::e_stateDone);
        item = item->next();
        loopFlag = true;
    }
    return item;
}

void AOASIO::writeToAudioSilenceForRemainder(tint index,tint i,tint len)
{
    tint j;
    common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

    for(j=0;j<m_noChannels;++j)
    {
        tint sSize = ASIODriverService::instance().getSampleSize(m_channelInfos[j].type);
        tubyte *out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
        ::memset(&out[i * sSize],0,(len - i) * sSize);
    }
    m_currentOutTime += dT;
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudio::callbackIOProcI(AudioUnitRenderActionFlags *ioActionFlags, \
                                      const AudioTimeStamp *inTimeStamp, \
                                      UInt32 inBusNumber, \
                                      UInt32 inNumberFrames, \
                                      AudioBufferList *ioData)
{
    AudioHardwareBufferCoreAudio buffer(ioData,inNumberFrames,sizeof(tfloat32),m_noChannels);
    IOTimeStamp systemTime;

    incrementMutexCount();
    systemTime = createIOTimeStamp(inTimeStamp);
    updateCurrentPlayTimeFromStreamTime(systemTime);

    if(m_audioStartFlag && (m_state==e_statePlay || m_state==e_statePreBuffer || m_state==e_stateCrossFade || m_state==e_stateNoCodec))
    {
        writeToAudio();
    }
    else
    {
        writeToAudioSilence(ioData,inNumberFrames);
    }

    m_currentCallbackTime.set(m_currentOutTime);
    incrementMutexCount();

    return noErr;
}

void AOCoreAudio::setItemStateToCallback(AudioItem *item)
{
    if(item->state()==AudioItem::e_stateFull || item->state()==AudioItem::e_stateFullEnd)
    {
        if(item->state()==AudioItem::e_stateFull)
        {
            item->setState(AudioItem::e_stateCallback);
        }
        else
        {
            item->setState(AudioItem::e_stateCallbackEnd);
        }
        m_callbackAudioItem = item;
    }
}


void AOCoreAudio::writeToAudio(const IOTimeStamp& systemTime)
{
    tint offset,amount,total;
    AudioItem *item = m_callbackAudioItem, *oItem = m_callbackAudioItem;
    bool loop = true,loopFlag = false;

    while(i<len && loop && !(loopFlag && item==oItem))
    {
        setItemStateToCallbackAsApplicable(item);

        if(item->state()==AudioItem::e_stateCallback || item->state()==AudioItem::e_stateCallbackEnd)
        {
            item = writeToAudioFromItem(item,systemTime,i,len,loop,loopFlag);
        }
        else if(item->state()==AudioItem::e_stateDone)
        {
            item = item->next();
            loopFlag = true;
        }
        else
        {
            break;
        }
    }

    if(i<len)
    {
        writeToAudioSilenceForRemainder(i,len);
    }
}

// TODO define parameters.
AudioItem *AOASIO::writeToAudioFromItem(AudioItem *item,const IOTimeStamp& systemTime,tint& i,tint len,bool& loop,bool& loopFlag)
{
    tint pNo;
    engine::RData *data = dynamic_cast<engine::RData *>(item->data());

    pNo = static_cast<tint>((static_cast<tuint32>(item->done()) >> 22) & 0x000003ff);
    if(pNo < data->noParts())
    {
        engine::RData::Part& part = data->part(pNo);

        if(part.isNext())
        {
            common::TimeStamp iT(static_cast<tfloat64>(i)/static_cast<tfloat64>(m_frequency));
            m_audioStartClock = streamT + iT - part.start();
            m_currentOutTime = part.start();
            m_currentPlayTime.set(part.start());
            m_currentCallbackTime.set(part.start());
            if(!m_trackTimeStateFlag)
            {
                m_trackTimeState = 1;
            }
            m_trackTimeStateFlag = false;
            m_startNextTrackFlag = true;
        }

        if(part.refStartTime()!=0)
        {
            common::TimeStamp dT,cRefTS = common::TimeStamp::reference();

            if(cRefTS >= part.refStartTime())
            {
                dT = cRefTS - part.refStartTime();
                m_currentOutTime = dT + part.start();
                m_audioStartClock = streamT - m_currentOutTime;
                m_currentPlayTime.set(m_currentOutTime);
                part.refStartTime() = 0;
            }
            else
            {
                dT = part.refStartTime() - cRefTS;
                amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
                if(amount > (len - i))
                {
                    amount = len - i;
                    dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
                }

                j = 0;
                k = 0;
                while(j<m_noChannels)
                {
                    tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
                    noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                    ::memset(&out[i * noChs],0,amount * noChs * sizeof(tfloat32));
                    j += noChs;
                    k++;
                }
                m_currentOutTime += dT;
                i += amount;
            }
        }
        else if(m_currentOutTime >= part.start())
        {
            if(m_currentOutTime < part.end())
            {
                tint pNoChannels = data->noOutChannels();

                offset = static_cast<tint>(static_cast<tuint32>(item->done()) & 0x003fffff);
                if(!offset)
                {
                    offset = static_cast<tint>(static_cast<tfloat64>(m_currentOutTime - part.start()) * static_cast<tfloat64>(m_frequency));
                }

                mem = data->partDataOut(pNo);
                total = part.length();

                if(offset>=0 && offset<total)
                {
                    amount = total - offset;

                    if(amount > (len - i))
                    {
                        amount = len - i;
                    }

                    if(amount > 0)
                    {
                        common::TimeStamp dT(static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency));

                        {
                            tint iA,iB,jC,tAmount;

                            j = 0;
                            k = 0;
                            while(j<m_noChannels)
                            {
                                tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);

                                noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                                tAmount = amount * noChs;
                                out = &out[i * noChs];

                                for(jC=0;jC<noChs && j<m_noChannels;jC++)
                                {
                                    int cIdx = m_outputChannelArray[j];
                                    iA = jC;
                                    iB = (offset * pNoChannels) + cIdx;
                                    if(cIdx>=0)
                                    {
                                        while(iA < tAmount)
                                        {
                                            out[iA] = engine::sampleToFloat32(mem[iB] * m_volume);
                                            iA += noChs;
                                            iB += pNoChannels;
                                        }
                                    }
                                    else
                                    {
                                        while(iA < tAmount)
                                        {
                                            out[iA] = 0.0f;
                                            iA += noChs;
                                        }
                                    }
                                    j++;
                                }
                                k++;
                            }
                        }

                        m_currentOutTime += dT;
                        i += amount;
                    }

                    offset += amount;

                    if(offset >= total)
                    {
                        m_currentOutTime = part.end();
                        offset = 0;
                        pNo++;
                    }
                }
                else
                {
                    if(offset < 0)
                    {
                        m_currentOutTime = part.start();
                        offset = 0;
                    }
                    else
                    {
                        m_currentOutTime = part.end();
                        offset = 0;
                        pNo++;
                    }
                }
            }
            else
            {
                offset = 0;
                pNo++;
            }
            item->setDone(static_cast<tint>(((static_cast<tuint32>(pNo) << 22) & 0xffc00000) | (static_cast<tuint32>(offset) & 0x003fffff)));
        }
        else
        {
            common::TimeStamp dT(part.start() - m_currentOutTime);

            amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
            if(amount > (len - i))
            {
                amount = len - i;
                dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
            }

            if(amount>0)
            {
                j = 0;
                k = 0;
                while(j<m_noChannels)
                {
                    tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
                    noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                    ::memset(&out[i * noChs],0,amount * noChs * sizeof(tfloat32));
                    j += noChs;
                    k++;
                }
            }

            m_currentOutTime += dT;
            i += amount;
        }
    }
    else
    {
        if(item->state()==AudioItem::e_stateCallbackEnd)
        {
            common::TimeStamp sT;
            sT = common::TimeStamp::reference() + m_outputLatencyTime + (static_cast<tfloat64>(i) / static_cast<tfloat64>(m_frequency));
            m_stopTimeClock.set(sT);
            m_stopTimeFlag = true;
            loop = false;
        }
        item->setState(AudioItem::e_stateDone);
        item = item->next();
        loopFlag = true;
    }
    return item;
}

// TODO define parameters.
void AOCoreAudio::writeToAudioSilenceForRemainder(tint i,tint len)
{
    common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

    j = 0;
    k = 0;
    while(j<m_noChannels)
    {
        tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
        noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
        ::memset(&out[i * noChs],0,(len - i) * noChs * sizeof(tfloat32));
        j += noChs;
        k++;
    }
    m_currentOutTime += dT;
}

void AOCoreAudio::writeToAudioSilence(AudioBufferList *ioData,UInt32 inNumberFrames)
{
    tint j,k,noChs;

    j = 0;
    k = 0;
    while(j<m_noChannels)
    {
        noChs = ioData->mBuffers[k].mNumberChannels;
        ::memset(ioData->mBuffers[k].mData,0,inNumberFrames * noChs * sizeof(tfloat32));
        j += noChs;
        k++;
    }
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudio::callbackIOProcI(AudioUnitRenderActionFlags *ioActionFlags, \
                                      const AudioTimeStamp *inTimeStamp, \
                                      UInt32 inBusNumber, \
                                      UInt32 inNumberFrames, \
                                      AudioBufferList *ioData)
{
    tint noChs;
    tint i = 0,j,k,len = inNumberFrames;
    common::TimeStamp streamT,tS;
    tfloat64 *mem;

    m_mutexCount++;

    streamT.nano64(AudioConvertHostTimeToNanos(inTimeStamp->mHostTime));
    if(!m_audioStartFlag)
    {
        m_audioStartClock = streamT - m_pauseTime;
        m_audioStartFlag = true;
        if(!m_trackTimeStateFlag)
        {
            m_trackTimeState = 1;
        }
        m_trackTimeStateFlag = false;
    }
    tS = streamT - m_audioStartClock;
    m_currentPlayTime.set(tS);

    if(m_audioStartFlag && (m_state==e_statePlay || m_state==e_statePreBuffer || m_state==e_stateCrossFade || m_state==e_stateNoCodec))
    {
        tint offset,amount,total;
        AudioItem *item = m_callbackAudioItem, *oItem = m_callbackAudioItem;
        bool loop = true,loopFlag = false;

        while(i<len && loop && !(loopFlag && item==oItem))
        {
            if(item->state()==AudioItem::e_stateFull || item->state()==AudioItem::e_stateFullEnd)
            {
                if(item->state()==AudioItem::e_stateFull)
                {
                    item->setState(AudioItem::e_stateCallback);
                }
                else
                {
                    item->setState(AudioItem::e_stateCallbackEnd);
                }
                m_callbackAudioItem = item;
            }

            if(item->state()==AudioItem::e_stateCallback || item->state()==AudioItem::e_stateCallbackEnd)
            {
                tint pNo;
                engine::RData *data = dynamic_cast<engine::RData *>(item->data());

                pNo = static_cast<tint>((static_cast<tuint32>(item->done()) >> 22) & 0x000003ff);
                if(pNo < data->noParts())
                {
                    engine::RData::Part& part = data->part(pNo);

                    if(part.isNext())
                    {
                        common::TimeStamp iT(static_cast<tfloat64>(i)/static_cast<tfloat64>(m_frequency));
                        m_audioStartClock = streamT + iT - part.start();
                        m_currentOutTime = part.start();
                        m_currentPlayTime.set(part.start());
                        m_currentCallbackTime.set(part.start());
                        if(!m_trackTimeStateFlag)
                        {
                            m_trackTimeState = 1;
                        }
                        m_trackTimeStateFlag = false;
                        m_startNextTrackFlag = true;
                    }

                    if(part.refStartTime()!=0)
                    {
                        common::TimeStamp dT,cRefTS = common::TimeStamp::reference();

                        if(cRefTS >= part.refStartTime())
                        {
                            dT = cRefTS - part.refStartTime();
                            m_currentOutTime = dT + part.start();
                            m_audioStartClock = streamT - m_currentOutTime;
                            m_currentPlayTime.set(m_currentOutTime);
                            part.refStartTime() = 0;
                        }
                        else
                        {
                            dT = part.refStartTime() - cRefTS;
                            amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
                            if(amount > (len - i))
                            {
                                amount = len - i;
                                dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
                            }

                            j = 0;
                            k = 0;
                            while(j<m_noChannels)
                            {
                                tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
                                noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                                ::memset(&out[i * noChs],0,amount * noChs * sizeof(tfloat32));
                                j += noChs;
                                k++;
                            }
                            m_currentOutTime += dT;
                            i += amount;
                        }
                    }
                    else if(m_currentOutTime >= part.start())
                    {
                        if(m_currentOutTime < part.end())
                        {
                            tint pNoChannels = data->noOutChannels();

                            offset = static_cast<tint>(static_cast<tuint32>(item->done()) & 0x003fffff);
                            if(!offset)
                            {
                                offset = static_cast<tint>(static_cast<tfloat64>(m_currentOutTime - part.start()) * static_cast<tfloat64>(m_frequency));
                            }

                            mem = data->partDataOut(pNo);
                            total = part.length();

                            if(offset>=0 && offset<total)
                            {
                                amount = total - offset;

                                if(amount > (len - i))
                                {
                                    amount = len - i;
                                }

                                if(amount > 0)
                                {
                                    common::TimeStamp dT(static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency));

                                    {
                                        tint iA,iB,jC,tAmount;

                                        j = 0;
                                        k = 0;
                                        while(j<m_noChannels)
                                        {
                                            tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);

                                            noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                                            tAmount = amount * noChs;
                                            out = &out[i * noChs];

                                            for(jC=0;jC<noChs && j<m_noChannels;jC++)
                                            {
                                                int cIdx = m_outputChannelArray[j];
                                                iA = jC;
                                                iB = (offset * pNoChannels) + cIdx;
                                                if(cIdx>=0)
                                                {
                                                    while(iA < tAmount)
                                                    {
                                                        out[iA] = engine::sampleToFloat32(mem[iB] * m_volume);
                                                        iA += noChs;
                                                        iB += pNoChannels;
                                                    }
                                                }
                                                else
                                                {
                                                    while(iA < tAmount)
                                                    {
                                                        out[iA] = 0.0f;
                                                        iA += noChs;
                                                    }
                                                }
                                                j++;
                                            }
                                            k++;
                                        }
                                    }

                                    m_currentOutTime += dT;
                                    i += amount;
                                }

                                offset += amount;

                                if(offset >= total)
                                {
                                    m_currentOutTime = part.end();
                                    offset = 0;
                                    pNo++;
                                }
                            }
                            else
                            {
                                if(offset < 0)
                                {
                                    m_currentOutTime = part.start();
                                    offset = 0;
                                }
                                else
                                {
                                    m_currentOutTime = part.end();
                                    offset = 0;
                                    pNo++;
                                }
                            }
                        }
                        else
                        {
                            offset = 0;
                            pNo++;
                        }
                        item->setDone(static_cast<tint>(((static_cast<tuint32>(pNo) << 22) & 0xffc00000) | (static_cast<tuint32>(offset) & 0x003fffff)));
                    }
                    else
                    {
                        common::TimeStamp dT(part.start() - m_currentOutTime);

                        amount = static_cast<tint>(static_cast<tfloat64>(dT) * static_cast<tfloat64>(m_frequency));
                        if(amount > (len - i))
                        {
                            amount = len - i;
                            dT = static_cast<tfloat64>(amount) / static_cast<tfloat64>(m_frequency);
                        }

                        if(amount>0)
                        {
                            j = 0;
                            k = 0;
                            while(j<m_noChannels)
                            {
                                tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
                                noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                                ::memset(&out[i * noChs],0,amount * noChs * sizeof(tfloat32));
                                j += noChs;
                                k++;
                            }
                        }

                        m_currentOutTime += dT;
                        i += amount;
                    }
                }
                else
                {
                    if(item->state()==AudioItem::e_stateCallbackEnd)
                    {
                        common::TimeStamp sT;
                        sT = common::TimeStamp::reference() + m_outputLatencyTime + (static_cast<tfloat64>(i) / static_cast<tfloat64>(m_frequency));
                        m_stopTimeClock.set(sT);
                        m_stopTimeFlag = true;
                        loop = false;
                    }
                    item->setState(AudioItem::e_stateDone);
                    item = item->next();
                    loopFlag = true;
                }
            }
            else if(item->state()==AudioItem::e_stateDone)
            {
                item = item->next();
                loopFlag = true;
            }
            else
            {
                break;
            }
        }

        if(i<len)
        {
            common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

            j = 0;
            k = 0;
            while(j<m_noChannels)
            {
                tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
                noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
                ::memset(&out[i * noChs],0,(len - i) * noChs * sizeof(tfloat32));
                j += noChs;
                k++;
            }

            m_currentOutTime += dT;
        }
    }
    else
    {
        j = 0;
        k = 0;
        while(j<m_noChannels)
        {
            noChs = ioData->mBuffers[k].mNumberChannels;
            ::memset(ioData->mBuffers[k].mData,0,inNumberFrames * noChs * sizeof(tfloat32));
            j += noChs;
            k++;
        }
    }

    m_currentCallbackTime.set(m_currentOutTime);
    m_mutexCount++;

    return noErr;
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



void AOBase::setItemStateToCallbackAsApplicable(AudioItem *item)
{
    if(item->state()==AudioItem::e_stateFull || item->state()==AudioItem::e_stateFullEnd)
    {
        if(item->state()==AudioItem::e_stateFull)
        {
            item->setState(AudioItem::e_stateCallback);
        }
        else
        {
            item->setState(AudioItem::e_stateCallbackEnd);
        }
        m_callbackAudioItem = item;
    }
}

class AOBaseSetItemStateToCallbackAsApplicableTest : public AOBase
{
    public:
        MOCK_METHOD1(setCallbackAudioTime,void(AudioItem *item));
        void testSetItemStateToCallbackAsApplicable(AudioItem *item);
};

void AOBaseSetItemStateToCallbackAsApplicableTest::testSetItemStateToCallbackAsApplicable(AudioItem *item)
{
    setItemStateToCallbackAsApplicable(item);
}

TEST(AOBase,setItemStateToCallbackAsApplicableWhenNotFull)
{
    AudioItem item;
    item.setState(AudioItem::e_stateDone);

    AOBaseSetItemStateToCallbackAsApplicableTest audio;

    audio.setItemStateToCallbackAsApplicable(&item);
    EXPECT_EQ(AudioItem::e_stateDone,item.state());
}

TEST(AOBase,setItemStateToCallbackAsApplicableWhenCallbackSet)
{
    AudioItem item;
    item.setState(AudioItem::e_stateCallback);

    AOBaseSetItemStateToCallbackAsApplicableTest audio;

    audio.setItemStateToCallbackAsApplicable(&item);
    EXPECT_EQ(AudioItem::e_stateCallback,item.state());
}

TEST(AOBase,setItemStateToCallbackAsApplicableWhenFull)
{
    AudioItem item;
    item.setState(AudioItem::e_stateFull);

    AOBaseSetItemStateToCallbackAsApplicableTest audio;
    EXPECT_CALL(audio,setCallbackAudioTime(&item)).Times(1);

    audio.setItemStateToCallbackAsApplicable(&item);
    EXPECT_EQ(AudioItem::e_stateCallback,item.state());
}

TEST(AOBase,setItemStateToCallbackAsApplicableWhenFullAndEnd)
{
    AudioItem item;
    item.setState(AudioItem::e_stateFullEnd);

    AOBaseSetItemStateToCallbackAsApplicableTest audio;
    EXPECT_CALL(audio,setCallbackAudioTime(&item)).Times(1);

    audio.setItemStateToCallbackAsApplicable(&item);
    EXPECT_EQ(AudioItem::e_stateCallbackEnd,item.state());
}

//-------------------------------------------------------------------------------------------


void AOCoreAudio::writeToAudioSilenceForRemainder(tint i,tint len)
{
    common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

    j = 0;
    k = 0;
    while(j<m_noChannels)
    {
        tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
        noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
        ::memset(&out[i * noChs],0,(len - i) * noChs * sizeof(tfloat32));
        j += noChs;
        k++;
    }
    m_currentOutTime += dT;
}

const common::TimeStamp& AOBase::getCurrentOutTime() const
{
    return m_currentOutTime;
}

void AOBase::setCurrentOutTime(const common::TimeStamp& t)
{
    m_currentOutTime = t;
}

tint AOBase::remainingSamplesInBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    return pBuffer->bufferLength() - fromIndex;
}

void AOBase::writeToAudioSilenceForRemainder(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    common::TimeStamp dT,newOutTime;

    writeToAudioSilenceToEndOfBuffer(pBuffer,fromIndex);
    dT = static_cast<tfloat64>(remainingSamplesInBuffer(pBuffer,fromIndex));
    newOutTime = getCurrentOutTime() + dT;
    setCurrentOutTime(newOutTime);
}

void AOBase::writeToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    tint j,k;

    j = 0;
    k = 0;
    while(j < pBuffer->numberOfBuffers())
    {
        tbyte *out = pBuffer->buffer(k);
        tint noChs = pBuffer->numberOfChannelsInBuffer(k);
        tint sampleSize = pBuffer->sampleSize(k);
        memset(&out[i * noChs * sampleSize],0,remainingSamplesInBuffer(pBuffer,fromIndex) * noChs * sampleSize);
        j += noChs;
        k++;
    }
}



void AOBase::writeToAudioSilenceForRemainder(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

    for(tint j=0;j<pBuffer->numberOfChannels();j++)
    {
        tint sSize = pBuffer->sampleSize(j);
        tbyte *out = reinterpret_cast<tbyte *>(pBuffer->buffer(j));
        memset(&out[fromIndex * sSize],0,(pBuffer->bufferLength() - fromIndex) * sSize);
    }
    m_currentOutTime += dT;
}

void AOASIO::writeToAudioSilenceForRemainder(tint index,tint i,tint len)
{
    tint j;
    common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

    for(j=0;j<m_noChannels;++j)
    {
        tint sSize = ASIODriverService::instance().getSampleSize(m_channelInfos[j].type);
        tubyte *out = reinterpret_cast<tubyte *>(m_bufferInfos[j].buffers[index]);
        ::memset(&out[i * sSize],0,(len - i) * sSize);
    }
    m_currentOutTime += dT;
}

void AOCoreAudio::writeToAudioSilenceForRemainder(tint i,tint len)
{
    common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));

    j = 0;
    k = 0;
    while(j<m_noChannels)
    {
        tfloat32 *out = reinterpret_cast<tfloat32 *>(ioData->mBuffers[k].mData);
        noChs = static_cast<tint>(ioData->mBuffers[k].mNumberChannels);
        ::memset(&out[i * noChs],0,(len - i) * noChs * sizeof(tfloat32));
        j += noChs;
        k++;
    }
    m_currentOutTime += dT;
}

void AOASIO::writeToAudioSilence(tint index)
{
    for(tint i=0;i<m_noOutChannels;++i)
    {
        ::memset(m_bufferInfos[i].buffers[index],0,m_bufferSizePref * ASIODriverService::getSampleSize(m_channelInfos[i].type));
    }
}

void AOCoreAudio::writeToAudioSilence(AudioBufferList *ioData,UInt32 inNumberFrames)
{
    tint j,k,noChs;

    j = 0;
    k = 0;
    while(j<m_noChannels)
    {
        noChs = ioData->mBuffers[k].mNumberChannels;
        ::memset(ioData->mBuffers[k].mData,0,inNumberFrames * noChs * sizeof(tfloat32));
        j += noChs;
        k++;
    }
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getCurrentOutTime() const
{
    return m_currentOutTime;
}

void AOBase::setCurrentOutTime(const common::TimeStamp& t)
{
    m_currentOutTime = t;
}

tint AOBase::remainingSamplesInBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    return pBuffer->bufferLength() - fromIndex;
}

void AOBase::writeToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    tint j,k;

    j = 0;
    k = 0;
    while(j < pBuffer->numberOfBuffers())
    {
        tbyte *out = pBuffer->buffer(k);
        tint noChs = pBuffer->numberOfChannelsInBuffer(k);
        tint sampleSize = pBuffer->sampleSize(k);
        memset(&out[i * noChs * sampleSize],0,remainingSamplesInBuffer(pBuffer,fromIndex) * noChs * sampleSize);
        j += noChs;
        k++;
    }
}

void AOBase::writeToAudioSilenceForRemainder(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    common::TimeStamp dT(static_cast<tfloat64>(len - i) / static_cast<tfloat64>(m_frequency));
    common::TimeStamp dT,newOutTime;

    writeToAudioSilenceToEndOfBuffer(pBuffer,fromIndex);
    dT = static_cast<tfloat64>(remainingSamplesInBuffer(pBuffer,fromIndex)) / static_cast<tfloat64>(getFrequency());
    newOutTime = getCurrentOutTime() + dT;
    setCurrentOutTime(newOutTime);
}

void AOBase::writeToAudioSilence(AudioBufferList *ioData,UInt32 inNumberFrames)
{
    tint j,k,noChs;

    j = 0;
    k = 0;
    while(j<m_noChannels)
    {
        noChs = ioData->mBuffers[k].mNumberChannels;
        ::memset(ioData->mBuffers[k].mData,0,inNumberFrames * noChs * sizeof(tfloat32));
        j += noChs;
        k++;
    }
}

//-------------------------------------------------------------------------------------------

tint AOBase::remainingSamplesInBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    return pBuffer->bufferLength() - fromIndex;
}

class AOBaseRemainingSamplesInBufferTest : public AOBaseTest
{
    public:
        tint testRemainingSamplesInBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex);
}

TEST(AOBase,remainingSamplesInBuffer)
{
    const tint c_channels[] = {3, 2, 1};
    AudioHardwareBufferTester buffer(c_channels);

    AOBaseRemainingSamplesInBufferTest audio;

    for(tint i=0;i<AudioHardwareBufferTester::c_bufferLength;i++)
    {
        EXPECT_EQ(AudioHardwareBufferTester::c_bufferLength - i,audio.testRemainingSamplesInBuffer(&buffer,i));
    }
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    tint j,k;

    if(remainingSamplesInBuffer(pBuffer,fromIndex) > 0)
    {
        j = 0;
        k = 0;
        while(j < pBuffer->numberOfBuffers())
        {
            tbyte *out = pBuffer->buffer(k);
            tint noChs = pBuffer->numberOfChannelsInBuffer(k);
            tint sampleSize = pBuffer->sampleSize(k);
            memset(&out[i * noChs * sampleSize],0,remainingSamplesInBuffer(pBuffer,fromIndex) * noChs * sampleSize);
            j += noChs;
            k++;
        }
    }
}

class AOBaseWriteToAudioSilenceToEndOfBufferTest : public AOBaseTest
{
    public:
        void testWriteToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex);
};

void AOBaseWriteToAudioSilenceToEndOfBufferTest::testWriteToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    writeToAudioSilenceToEndOfBuffer(pBuffer,fromIndex);
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithNoChannels)
{
    AudioHardwareBufferTester buffer(0,0);

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,3);
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithMono)
{
    const tint c_channels[1] = {1};

    const tfloat32 c_BufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f };

    const tfloat32 c_expectBufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 0.0f, 0.0f };

    AudioHardwareBufferTester buffer(c_channels,1);
    memcpy(buffer.buffer(0),c_BufferA,1 * 5 * sizeof(tfloat32));

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,3);

    EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,1 * 5 * sizeof(tfloat32)));
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithStereo)
{
    const tint c_channels[1] = {2};

    const tfloat32 c_BufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };

    const tfloat32 c_expectBufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    AudioHardwareBufferTester buffer(c_channels,1);
    memcpy(buffer.buffer(0),c_BufferA,2 * 5 * sizeof(tfloat32));

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,3);

    EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,2 * 5 * sizeof(tfloat32)));
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithQuadrophonicInSingleBuffer)
{
    const tint c_channels[1] = {4};

    const tfloat32 c_BufferA[4 * 5] = {
        10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f,
        20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f
    };

    const tfloat32 c_expectBufferA[4 * 5] = {
        10.0f, 11.0f, 12.0f, 13.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    AudioHardwareBufferTester buffer(c_channels,2);
    memcpy(buffer.buffer(0),c_BufferA,4 * 5 * sizeof(tfloat32));

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,1);

    EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,4 * 5 * sizeof(tfloat32)));
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithQuadrophonicInStereoBuffers)
{
    const tint c_channels[2] = {2, 2};

    const tfloat32 c_BufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
    const tfloat32 c_BufferB[2 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f };

    const tfloat32 c_expectBufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 0.0f, 0.0f };
    const tfloat32 c_expectBufferB[2 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 0.0f, 0.0f };

    AudioHardwareBufferTester buffer(c_channels,2);
    memcpy(buffer.buffer(0),c_BufferA,2 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,4);

    EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,2 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithQuadrophonicInMonoBuffers)
{
    const tint c_channels[4] = {1, 1, 1, 1};

    const tfloat32 c_BufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f };
    const tfloat32 c_BufferB[1 * 5] = { 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
    const tfloat32 c_BufferC[1 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };
    const tfloat32 c_BufferD[1 * 5] = { 25.0f, 26.0f, 27.0f, 28.0f, 29.0f };

    const tfloat32 c_expectBufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 0.0f };
    const tfloat32 c_expectBufferB[1 * 5] = { 15.0f, 16.0f, 17.0f, 18.0f, 0.0f };
    const tfloat32 c_expectBufferC[1 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 0.0f };
    const tfloat32 c_expectBufferD[1 * 5] = { 25.0f, 26.0f, 27.0f, 28.0f, 0.0f };

    AudioHardwareBufferTester buffer(c_channels,4);
    memcpy(buffer.buffer(0),c_BufferA,1 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(1),c_BufferB,1 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(3),c_BufferC,1 * 5 * sizeof(tfloat32));

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,4);

    EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,1 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,1 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(3),c_expectBufferD,1 * 5 * sizeof(tfloat32)));
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithHetreogeneousBuffers)
{
    const tint c_channels[3] = {3, 2, 1};

    const tfloat32 c_BufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
    const tfloat32 c_BufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
    const tfloat32 c_BufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

    const tfloat32 c_expectBufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f };
    const tfloat32 c_expectBufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f };
    const tfloat32 c_expectBufferC[1 * 5] = { 26.0f, 27.0f, 0.0f, 0.0f, 0.0f };

    AudioHardwareBufferTester buffer(c_channels,3);
    memcpy(buffer.buffer(0),c_BufferA,3 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,2);

    EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,3 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithHetreogeneousAndNoSilence)
{
    const tint c_channels[3] = {3, 2, 1};

    const tfloat32 c_BufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
    const tfloat32 c_BufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
    const tfloat32 c_BufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

    const tfloat32 c_expectBufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
    const tfloat32 c_expectBufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
    const tfloat32 c_expectBufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

    AudioHardwareBufferTester buffer(c_channels,3);
    memcpy(buffer.buffer(0),c_BufferA,3 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,5);

    EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,3 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
}

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithHetreogeneousAndFullSilence)
{
    const tint c_channels[3] = {3, 2, 1};

    const tfloat32 c_BufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
    const tfloat32 c_BufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
    const tfloat32 c_BufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

    const tfloat32 c_expectBufferA[3 * 5] = { 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f };
    const tfloat32 c_expectBufferB[2 * 5] = { 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f };
    const tfloat32 c_expectBufferC[1 * 5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    AudioHardwareBufferTester buffer(c_channels,3);
    memcpy(buffer.buffer(0),c_BufferA,3 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
    memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));

    AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
    audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,0);

    EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,3 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
    EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
}


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

void AOWin32::processStopTimeMessage()
{
    if(m_stopTimeFlag)
    {
        common::TimeStamp dT,rT;

        dT = const_cast<const common::TimeStamp &>(m_stopTimeClock);
        rT = common::TimeStamp::reference();
        if(dT > rT)
        {
            tint delay;

            dT -= rT;
            delay = (dT.secondsTotal() * 1000) + dT.millisecond();
            QTimer::singleShot(delay,this,SLOT(onStop()));
        }
        else
        {
            stopCodec();
        }
        m_stopTimeFlag = false;
    }
}

AOCoreAudio::processMessages()
{
    if(m_stopTimeFlag)
    {
        if(m_flagStart)
        {
            tint delay;
            common::TimeStamp dT,cT;

            cT = common::TimeStamp::reference();
            dT = const_cast<const common::TimeStamp &>(m_stopTimeClock);
            if(dT > cT)
            {
                dT -= cT;
                delay = (dT.secondsTotal() * 1000) + dT.millisecond();
                if(delay>0)
                {
                    QTimer::singleShot(delay,this,SLOT(onStopProcess()));
                }
                else
                {
                    onStopProcess();
                }
            }
            else
            {
                onStopProcess();
            }
        }
        else
        {
            onStopProcess();
        }
        m_stopTimeFlag = false;
    }
}

void AOLinuxALSA::processMessagesForStop()
{
    if(getStopTimeFlag())
    {
        if(getFlagStart())
        {
            common::TimeStamp dT,cT;

            cT = getReferenceClockTime();
            dT = getStopTimeClock();
            if(dT > cT)
            {
                tint delay;

                dT -= cT;
                delay = (dT.secondsTotal() * 1000) + dT.millisecond();
                if(delay>0)
                {
                    processMessagesForStopSetTimer(delay);
                }
                else
                {
                    stopCodec();
                }
            }
            else
            {
                stopCodec();
            }
        }
        setStopTimeFlag(false);
    }
}

void AOLinuxALSA::processMessagesForStopSetTimer(tint delay)
{
    QTimer::singleShot(delay,this,SLOT(onStopProcess()));
}

class AOLinuxALSAProcessMessagesTest : public AOLinuxALSA
{
    MOCK_CONST_METHOD0(getStopTimeFlag,bool());
    MOCK_METHOD1(setStopTimeFlag,void(bool f));
    MOCK_CONST_METHOD0(getFlagStart,bool());
    MOCK_CONST_METHOD0(getReferenceClockTime,common::TimeStamp());
    MOCK_CONST_METHOD0(getStopTimeClock,common::TimeStamp());
    MOCK_METHOD1(stopCodec,void(bool));
    MOCK_METHOD1(processMessagesForStopSetTimer,void(tint delay));

    void testProcessMessagesForStop();
};

void AOLinuxALSAProcessMessagesTest::testProcessMessagesForStop()
{
    processMessagesForStop();
}

TEST(AOLinuxALSA,processMessagesForStopGivenStopFlagNotSet)
{
    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(false));

    audio.testProcessMessagesForStop();
}


TEST(AOLinuxALSA,processMessagesForStopGivenStopFlagIsSetButAudioIsNotStarted)
{
    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getFlagStart()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(audio,setStopTimeFlag(false)).Times(1);

    audio.testProcessMessagesForStop();
}

TEST(AOLinuxALSA,processMessagesForStopGivenStopAndAudioWithReferenceAfterStopTime)
{
    common::TimeStamp refeT(50.0);
    common::TimeStamp stopT(49.0);

    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getFlagStart()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getReferenceClockTime()).Times(1).WillOnce(Return(refeT));
    EXPECT_CALL(audio,getStopTimeClock()).Times(1).WillOnce(Return(stopT));
    EXPECT_CALL(audio,stopCodec(true)).Times(1);
    EXPECT_CALL(audio,setStopTimeFlag(false)).Times(1);

    audio.testProcessMessagesForStop();
}

TEST(AOLinuxALSA,processMessagesForStopGivenStopAndAudioWithReferenceBeforeStopTimeWithInsignifcantDelay)
{
    common::TimeStamp refeT(50.0);
    common::TimeStamp stopT(50.000001);

    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getFlagStart()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getReferenceClockTime()).Times(1).WillOnce(Return(refeT));
    EXPECT_CALL(audio,getStopTimeClock()).Times(1).WillOnce(Return(stopT));
    EXPECT_CALL(audio,stopCodec(true)).Times(1);
    EXPECT_CALL(audio,setStopTimeFlag(false)).Times(1);

    audio.testProcessMessagesForStop();
}

TEST(AOLinuxALSA,processMessagesForStopGivenStopAndAudioWithReferenceBeforeStopTimeWithDelay)
{
    common::TimeStamp refeT(50.0);
    common::TimeStamp stopT(53.2);

    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getFlagStart()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getReferenceClockTime()).Times(1).WillOnce(Return(refeT));
    EXPECT_CALL(audio,getStopTimeClock()).Times(1).WillOnce(Return(stopT));
    EXPECT_CALL(audio,processMessagesForStopSetTimer(3200)).Times(1);
    EXPECT_CALL(audio,setStopTimeFlag(false)).Times(1);

    audio.testProcessMessagesForStop();
}
