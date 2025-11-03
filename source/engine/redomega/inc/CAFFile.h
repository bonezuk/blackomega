//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_CAFFILE_H
#define __OMEGA_ENGINE_REDOMEGA_CAFFILE_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/File.h"
#include "engine/whiteomega/inc/Atom.h"
#include "engine/redomega/inc/ALACSequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

#define CAFF_CADD_ID ATOM_ID('c','a','f','f')
#define CAFF_DESC_ID ATOM_ID('d','e','s','c')
#define CAFF_ALAC_ID ATOM_ID('a','l','a','c')
#define CAFF_KUKI_ID ATOM_ID('k','u','k','i')
#define CAFF_PAKT_ID ATOM_ID('p','a','k','t')
#define CAFF_DATA_ID ATOM_ID('d','a','t','a')

//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT CAFFile
{
    public:
        CAFFile();
        virtual ~CAFFile();

        bool init(engine::File *f);
        void close();

        ALACSequence *getSequence(tuint32 id);

        tint blockOffset(tuint32 id) const;
        tint blockLength(tuint32 id) const;

    protected:

        engine::File *m_file;
        QMap<tuint32,QPair<tint,tint> > m_blockMap; // <blockID, <block length,block offset> >

        void printError(const tchar *strR,const tchar *strE) const;

        tuint32 readUInt32(tubyte *y) const;
};

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

