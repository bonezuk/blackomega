//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

void AOBase::initCyclicBuffer()
{
    tint i;
    AudioItem *item = 0,*pItem=0;

    freeCyclicBuffer();

    for(i=0;i<m_itemNumber;++i)
    {
        item = new AudioItem(this,m_frameNumber,m_frameLength,m_noInChannels,m_noOutChannels);

        if(pItem==0)
        {
            m_callbackAudioItem = item;
        }
        else
        {
            pItem->setNext(item);
        }
        item->setPrev(pItem);
        pItem = item;
    }
    if(item!=0)
    {
        item->setNext(m_callbackAudioItem);
        m_callbackAudioItem->setPrev(item);
    }
    m_codecAudioItem = m_callbackAudioItem;

    m_crossFadeItem = new AudioItem(this,m_frameNumber,m_frameLength,m_noInChannels,m_noOutChannels);

    m_crossASample = new tfloat64 [m_frameNumber * m_frameLength * m_noOutChannels];
    m_crossBSample = new tfloat64 [m_frameNumber * m_frameLength * m_noOutChannels];

    m_mergeAudioItem = new AudioItem(this,m_frameNumber,m_frameLength,m_noInChannels,m_noOutChannels);
}

void AOBase::freeCyclicBuffer()
{
    AudioItem *nItem,*item = m_callbackAudioItem;

    if(item!=0)
    {
        do
        {
            nItem = item->next();
            delete item;
            item = nItem;
        } while(item!=m_callbackAudioItem);
    }
    m_callbackAudioItem = 0;
    m_codecAudioItem = 0;

    if(m_crossFadeItem!=0)
    {
        delete m_crossFadeItem;
        m_crossFadeItem = 0;
    }
    if(m_crossASample!=0)
    {
        delete [] m_crossASample;
        m_crossASample = 0;
    }
    if(m_crossBSample!=0)
    {
        delete [] m_crossBSample;
        m_crossBSample = 0;
    }
    if(m_mergeAudioItem!=0)
    {
        delete m_mergeAudioItem;
        m_mergeAudioItem = 0;
    }
}

void AOBase::flushCyclicBuffer()
{
    AudioItem *item = m_callbackAudioItem;

    do
    {
        item->setState(AudioItem::e_stateEmpty);
        item->setDone(0);
        item->data()->reset();
        item = item->prev();
    } while(item!=m_callbackAudioItem);

    m_callbackAudioItem = item;
    m_codecAudioItem = item;

    if(m_crossFadeItem!=0)
    {
        m_crossFadeItem->setDone(0);
        m_crossFadeItem->data()->reset();
    }
}

