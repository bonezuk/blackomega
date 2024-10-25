//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_ALACSEQUENCE_H
#define __OMEGA_ENGINE_REDOMEGA_ALACSEQUENCE_H
//-------------------------------------------------------------------------------------------

#include "engine/redomega/inc/RedOmegaDLL.h"
#include "common/inc/Log.h"
#include "engine/inc/SequenceMemory.h"
#include "engine/inc/AData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT ALACSequence : public engine::SequenceMemory
{
	public:
		ALACSequence(QSharedPointer<common::Array<tubyte,tubyte> >& array);
		ALACSequence(QSharedPointer<common::Array<tubyte,tubyte> >& array,tint bitLength);
		virtual ~ALACSequence();

		tuint32 readBERInteger();

		tfloat32 readFloat();
		tfloat64 readDouble();

	protected:

};

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
