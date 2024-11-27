//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_FORMATSSUPPORTED_H
#define __OMEGA_AUDIOIO_FORMATSSUPPORTED_H
//-------------------------------------------------------------------------------------------

#include <QSet>

#include "audioio/inc/FormatDescription.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT FormatsSupported
{
	public:
		FormatsSupported();
		FormatsSupported(const FormatsSupported& rhs);
		
		const FormatsSupported& operator = (const FormatsSupported& rhs);
		
		bool isEmpty() const;
        bool isSupported(const FormatDescription& desc) const;
		
		void add(const FormatDescription& desc);
		void clear();
		
		void print() const;
				
	protected:
	
		QSet<tuint32> m_formats;
		
        tuint32 toKey(const FormatDescription& desc) const;
        FormatDescription fromKey(tuint32 key) const;
		
		void copy(const FormatsSupported& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
