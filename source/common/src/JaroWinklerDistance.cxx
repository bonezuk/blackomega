#include "common/inc/JaroWinklerDistance.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

JaroWinklerDistance::JaroWinklerDistance(const QString& s2) : m_s2(s2),
    m_s2Lower(s2.toLower()),
    m_lookupIndexMap(),
    m_lookupLowerIndexMap()
{}

//-------------------------------------------------------------------------------------------

JaroWinklerDistance::~JaroWinklerDistance()
{}

//-------------------------------------------------------------------------------------------

void JaroWinklerDistance::buildIndexMap(const QString& s2,QVector<QMultiHash<QChar,int> >& iMapList,int noEntries)
{
    int i,j;
    int s1Len,s2Len,searchWindow;
    
    iMapList.clear();
    
    s1Len = noEntries;
    s2Len = s2.length();
    searchWindow = window(s1Len,s2Len);
    
    for(i=0;i<noEntries;i++)
    {
        QMultiHash<QChar,int> indexMap;
        
        int sPos = maxLength(0,i - searchWindow);
        int sEnd = minLength(i + searchWindow + 1,s2Len);
        
        for(j=sPos;j<sEnd;j++)
        {
            indexMap.insert(s2.at(j),j);
        }
        iMapList.append(indexMap);
    }
}

//-------------------------------------------------------------------------------------------

const QString& JaroWinklerDistance::getComparisonString(bool caseSensitive) const
{
    return (caseSensitive) ? m_s2 : m_s2Lower;
}

//-------------------------------------------------------------------------------------------

QMap<int,QVector<QMultiHash<QChar,int> > >& JaroWinklerDistance::getLookupIndexMap(bool caseSensitive)
{
    return (caseSensitive) ? m_lookupIndexMap : m_lookupLowerIndexMap;
}

//-------------------------------------------------------------------------------------------

const QVector<QMultiHash<QChar,int> >& JaroWinklerDistance::getIndexMap(const QString& s1,bool caseSensitive)
{
    int len = s1.length();
    
    if(getLookupIndexMap(caseSensitive).find(len)==getLookupIndexMap(caseSensitive).end())
    {
        QVector<QMultiHash<QChar,int> > indexMap;
        getLookupIndexMap(caseSensitive).insert(len,indexMap);
        buildIndexMap(getComparisonString(caseSensitive),getLookupIndexMap(caseSensitive).find(len).value(),len);    
    }
    return getLookupIndexMap(caseSensitive).find(len).value();
}

//-------------------------------------------------------------------------------------------
// The given string expects to have an index map with at least the same length
//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::findMatches(const QString& s,int *match,bool caseSensitive)
{
    int m,s1Len;
    QString s1 = (caseSensitive) ? s : s.toLower();
    const QVector<QMultiHash<QChar,int> >& indexMap = getIndexMap(s1,caseSensitive);

    m = 0;
    s1Len = s1.length();
    for(int i=0;i<s1Len;i++)
    {
        int pos,iCount;
        const QChar& a = s1.at(i);
        
        const QMultiHash<QChar,int>& indexEntry = indexMap.at(i);
    
        pos = -1;
        iCount = indexEntry.count(a);
        if(iCount > 1)
        {
            QList<int> positions = indexEntry.values(a);

            for(QList<int>::iterator ppI=positions.begin();ppI!=positions.end();++ppI)
            {
                if((pos<0 || *ppI<pos) && !(match[*ppI] & 0x00000002))
                {
                    pos = *ppI;
                }
            }
        }
        else if(iCount==1)
        {
            pos = indexEntry.value(a);
            if(match[pos] & 0x00000002)
            {
                pos = -1;
            }
        }
    
        if(pos>=0 && !(match[pos] & 0x00000002))
        {
            match[i] |= 0x00000001;
            match[pos] |= 0x00000002;
            m++;
        }
    }
    return m;
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::minLength(int s1Len,int s2Len)
{
    return (s1Len < s2Len) ? s1Len : s2Len;
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::maxLength(int s1Len,int s2Len)
{
    return (s1Len > s2Len) ? s1Len : s2Len;
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::window(int s1Len,int s2Len)
{
    int len = (maxLength(s1Len,s2Len) >> 1) - 1;
    return (len > 0) ? len : 0;
}

//-------------------------------------------------------------------------------------------

tfloat64 JaroWinklerDistance::distance(const QString& s1,const QString& s2,bool caseSensitive)
{
    int s1Len,s2Len,sLMax;
    int m,t,l;
    int *match;

    if(s1.isEmpty() || s2.isEmpty())
    {
        if(s1.isEmpty() && s2.isEmpty())
        {
            return 1.0;
        }
        return 0.0;
    }
    
    s1Len = s1.length();
    s2Len = s2.length();
    sLMax = maxLength(s1Len,s2Len);
    
    match = new int [sLMax];
    memset(match,0,sLMax * sizeof(int));
    
    if(caseSensitive)
    {
        m = findMatchesBothCaseSensitive(s1,s2,match);
        t = transpositionsCaseSensitive(s1,s2,match);
        l = winklerScalarCaseSensitive(s1,s2);
    }
    else
    {
        m = findMatchesBothCaseInsensitive(s1,s2,match);
        t = transpositionsCaseInsensitive(s1,s2,match);
        l = winklerScalarCaseInsensitive(s1,s2);
    }
    
    delete [] match;
    
    return calculateDistance(m,t,l,s1Len,s2Len);
}

//-------------------------------------------------------------------------------------------

tfloat64 JaroWinklerDistance::calculateDistance(int m,int t,int l,int s1Len,int s2Len)
{
    tfloat64 dW;
    
    if(m > 0)
    {
        tfloat64 mD = static_cast<tfloat64>(m);
        tfloat64 tD = static_cast<tfloat64>(t);
        tfloat64 s1L = static_cast<tfloat64>(s1Len);
        tfloat64 s2L = static_cast<tfloat64>(s2Len);
        tfloat64 dJ = ((mD / s1L) + (mD / s2L) + ((mD - tD) / mD)) / 3.0;
        tfloat64 lD = static_cast<tfloat64>(l);
        dW = dJ + (lD * 0.1 * (1.0 - dJ));
    }
    else
    {
        dW = 0.0;
    }
    return dW;
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::findMatchesBothCaseSensitive(const QString& s1,const QString& s2,int *match)
{
    int i,j;
    int m,searchWindow;
    int s1Len,s2Len;
    
    s1Len = s1.length();
    s2Len = s2.length();

    m = 0;
    searchWindow = window(s1Len,s2Len);
    
    for(i=0;i<s1Len;i++)
    {
        const QChar& a = s1.at(i);
    
        int sPos = maxLength(0,i - searchWindow);
        int sEnd = minLength(i + searchWindow + 1,s2Len);
        
        for(j=sPos;j<sEnd;j++)
        {
            if(!(match[j] & 0x00000002) && a==s2.at(j))
            {
                match[i] |= 0x00000001;
                match[j] |= 0x00000002;
                m++;
                break;
            }
        }
    }
    return m;
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::findMatchesBothCaseInsensitive(const QString& s1,const QString& s2,int *match)
{
    int i,j;
    int m,searchWindow;
    int s1Len,s2Len;
    
    s1Len = s1.length();
    s2Len = s2.length();

    m = 0;
    searchWindow = window(s1Len,s2Len);
    
    for(i=0;i<s1Len;i++)
    {
        QChar a = s1.at(i).toLower();
    
        int sPos = maxLength(0,i - searchWindow);
        int sEnd = minLength(i + searchWindow + 1,s2Len);
        
        for(j=sPos;j<sEnd;j++)
        {
            if(!(match[j] & 0x00000002) && a==s2.at(j).toLower())
            {
                match[i] |= 0x00000001;
                match[j] |= 0x00000002;
                m++;
                break;
            }
        }
    }
    return m;
}

//-------------------------------------------------------------------------------------------
// For each s1-0x01 match there is a corrisponding s2-0x02 match. If these do not corrispond
// then the given matches have been generated else where than the findMatches methods.
// So when a match for s1 is flagged a match for s2 can also be found.
//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::transpositionsCaseSensitive(const QString& s1,const QString& s2,int *match)
{
    int i,j,t;
    int s1Len;
    
    s1Len = s1.length();
    
    t = 0;
    j = 0;
    for(i=0;i<s1Len;i++)
    {
        const QChar& a = s1.at(i);
        
        if(match[i] & 0x00000001)
        {
            while(!(match[j] & 0x00000002))
            {
                j++;
            }
            if(a != s2.at(j))
            {
                t++;
            }
            j++;
        }
    }
    t /= 2;
    
    return t;
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::transpositionsCaseInsensitive(const QString& s1,const QString& s2,int *match)
{
    int i,j,t;
    int s1Len;
    
    s1Len = s1.length();
    
    t = 0;
    j = 0;
    for(i=0;i<s1Len;i++)
    {
        QChar a = s1.at(i).toLower();
        
        if(match[i] & 0x00000001)
        {
            while(!(match[j] & 0x00000002))
            {
                j++;
            }
            if(a != s2.at(j).toLower())
            {
                t++;
            }
            j++;
        }
    }
    t /= 2;
    
    return t;
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::winklerScalarCaseSensitive(const QString& s1,const QString& s2)
{
    int l,lMax,sLen;
    
    sLen = minLength(s1.length(),s2.length());
    lMax = (sLen < 4) ? sLen : 4;
    for(l=0;l<lMax;l++)
    {
        if(s1.at(l)!=s2.at(l))
        {
            break;
        }
    }
    return l;
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistance::winklerScalarCaseInsensitive(const QString& s1,const QString& s2)
{
    int l,lMax,sLen;
    
    sLen = minLength(s1.length(),s2.length());
    lMax = (sLen < 4) ? sLen : 4;
    for(l=0;l<lMax;l++)
    {
        if(s1.at(l).toLower()!=s2.at(l).toLower())
        {
            break;
        }
    }
    return l;
}

//-------------------------------------------------------------------------------------------

tfloat64 JaroWinklerDistance::distance(const QString& s1,bool caseSensitive)
{
    int s1Len,s2Len,sLMax;
    int m,t,l;
    int *match;
    const QString& s2 = getComparisonString(caseSensitive);

    if(s1.isEmpty() || s2.isEmpty())
    {
        if(s1.isEmpty() && s2.isEmpty())
        {
            return 1.0;
        }
        return 0.0;
    }
    
    s1Len = s1.length();
    s2Len = s2.length();
    sLMax = maxLength(s1Len,s2Len);
    
    match = new int [sLMax];
    memset(match,0,sLMax * sizeof(int));
    
    m = findMatches(s1,match,caseSensitive);
    if(caseSensitive)
    {
        t = transpositionsCaseSensitive(s1,s2,match);
        l = winklerScalarCaseSensitive(s1,s2);
    }
    else
    {
        t = transpositionsCaseInsensitive(s1,s2,match);
        l = winklerScalarCaseInsensitive(s1,s2);
    }
    
    delete [] match;
    
    return calculateDistance(m,t,l,s1Len,s2Len);
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

