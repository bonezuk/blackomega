//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_ATOMINFO_H
#define __OMEGA_TRACK_INFO_ATOMINFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"
#include "engine/inc/IOFile.h"
#include "engine/whiteomega/inc/Atom.h"

#include <QList>
#include <QPair>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT AtomInfo : public Info
{
    public:
        AtomInfo();
        virtual ~AtomInfo();
        
        virtual bool read(common::BIOStream *input);

        virtual bool isImage() const;
        virtual ImageInfoArray *getImageData(ImageFormat& format) const;
        virtual ImageInfoArray *getImageData(IDTagImageType type,ImageFormat& format) const;

        virtual bool isChildren() const;
        virtual tint noChildren() const;
        virtual const ChildInfo& child(tint idx) const;

    protected:

        QList<ChildInfo> m_chapters;
        
        ImageFormat m_coverFormat;
        ImageInfoArray *m_coverArray;
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        QString getAtomData(const tchar *name,engine::whiteomega::Atom *a);
        QString getAtomData(const QString& name,engine::whiteomega::Atom *a);
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

