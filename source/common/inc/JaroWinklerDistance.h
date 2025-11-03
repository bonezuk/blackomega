//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_JAROWINKLERDISTANCE_H
#define __OMEGA_COMMON_JAROWINKLERDISTANCE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QHash>
#include <QtCore/QMultiHash>
#include <QtCore/QVector>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT JaroWinklerDistance
{
    public:
        JaroWinklerDistance(const QString& s2);
        virtual ~JaroWinklerDistance();

        static tfloat64 distance(const QString& s1,const QString& s2,bool caseSensitive = true);

        tfloat64 distance(const QString& s1,bool caseSensitive = true);

    protected:

        QString m_s2;
        QString m_s2Lower;
        QMap<int,QVector<QMultiHash<QChar,int> > > m_lookupIndexMap;
        QMap<int,QVector<QMultiHash<QChar,int> > > m_lookupLowerIndexMap;

        virtual const QString& getComparisonString(bool caseSensitive) const;
        virtual QMap<int,QVector<QMultiHash<QChar,int> > >& getLookupIndexMap(bool caseSensitive);

        virtual void buildIndexMap(const QString& s2,QVector<QMultiHash<QChar,int> >& iMapList,int noEntries);
        virtual const QVector<QMultiHash<QChar,int> >& getIndexMap(const QString& s1,bool caseSensitive);
        virtual int findMatches(const QString& s,int *match,bool caseSensitive);

        static int minLength(int s1Len,int s2Len);
        static int maxLength(int s1Len,int s2Len);
        static int window(int s1Len,int s2Len);

        static int findMatchesBothCaseSensitive(const QString& s1,const QString& s2,int *match);
        static int findMatchesBothCaseInsensitive(const QString& s1,const QString& s2,int *match);

        static int transpositionsCaseSensitive(const QString& s1,const QString& s2,int *match);
        static int transpositionsCaseInsensitive(const QString& s1,const QString& s2,int *match);

        static int winklerScalarCaseSensitive(const QString& s1,const QString& s2);
        static int winklerScalarCaseInsensitive(const QString& s1,const QString& s2);

        static tfloat64 calculateDistance(int m,int t,int l,int s1Len,int s2Len);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

