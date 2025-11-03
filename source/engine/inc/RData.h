//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_RDATA_H
#define __OMEGA_ENGINE_RDATA_H
//-------------------------------------------------------------------------------------------

#include <QVector>

#include "common/inc/TimeStamp.h"
#include "engine/inc/AData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT RData : public AData
{
    public:
        class Part;
    
    public:
        RData();
        RData(tint len,tint inChannel,tint outChannel);
        RData(const AData& rhs);
        virtual ~RData();
        
        virtual Part& nextPart();

        virtual tint rOffset() const;
        virtual tint rLength() const;
        
        virtual tint noParts() const;
        
        virtual Part& part(tint i);
        virtual const Part& partConst(tint i) const;

        virtual sample_t *partData(tint i);
        virtual const sample_t *partDataConst(tint i) const;
        
        virtual sample_t *partDataOut(tint i);
        virtual const sample_t *partDataOutConst(tint i) const;        

        virtual sample_t *currentPartData();
        virtual const sample_t *currentPartDataConst() const;
        
        virtual sample_t *partFilterData(tint i, tint filterIndex);
        virtual const sample_t *partFilterDataConst(tint i, tint filterIndex) const;

        virtual sample_t *partDataCenter(tint i);
        virtual const sample_t *partDataCenterConst(tint i) const;

        virtual void reset();
        
        virtual void clipToTime(const common::TimeStamp& clipT);
        
        virtual sample_t *center();
        
        virtual CodecDataType partGetDataType(tint i) const;
        
    protected:
        
        QVector<Part> m_parts;
        
        virtual void copy(const AData& rhs);
};

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT RData::Part
{
    public:
        Part();
        Part(const Part& rhs);
        virtual ~Part();
        
        const Part& operator = (const Part& rhs);
    
        virtual tint& offset();
        virtual const tint& offsetConst() const;
        virtual tint& length();
        virtual const tint& lengthConst() const;
        virtual bool& done();
        virtual const bool& doneConst() const;

        virtual common::TimeStamp& start();
        virtual const common::TimeStamp& startConst() const;
        virtual common::TimeStamp& end();
        virtual const common::TimeStamp& endConst() const;
        
        virtual common::TimeStamp& startNext();
        virtual const common::TimeStamp& startNextConst() const;
        virtual common::TimeStamp& endNext();
        virtual const common::TimeStamp& endNextConst() const;
        
        virtual common::TimeStamp& refStartTime();
        virtual const common::TimeStamp& refStartTimeConst() const;

        virtual bool isNext() const;
        virtual void setNext(bool flag);

        virtual CodecDataType getDataType() const;
        virtual void setDataType(CodecDataType dType);

        friend ENGINE_EXPORT bool operator == (const Part& a,const Part& b);
        friend ENGINE_EXPORT bool operator != (const Part& a,const Part& b);
    
    protected:

        tint m_offset;
        tint m_length;
        common::TimeStamp m_start;
        common::TimeStamp m_end;
        bool m_done;
        
        bool m_nextFlag;
        common::TimeStamp m_startNext;
        common::TimeStamp m_endNext;
        
        common::TimeStamp m_refStartTime;
        
        CodecDataType m_dataType;

        void copy(const Part& rhs);
        
        virtual bool isEqual(const Part& rhs) const;
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

