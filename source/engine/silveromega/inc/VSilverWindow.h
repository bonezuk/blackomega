//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERWINDOW_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERWINDOW_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverContainer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverWindow
{
    public:
        VSilverWindow(VSilverContainer *info);
        ~VSilverWindow();
        
        static void start();
        static void stop();
        
        bool setup(engine::Sequence *seq);
        
        tint getPCM(sample_t *mem,tint len);
        
        void window();
        
        void synthesis();
        
        tint m_mode;
        tint m_winLength;
        tfloat32 *m_window;
        
        tint m_leftStart;
        tint m_leftEnd;
        tint m_rightStart;
        tint m_rightEnd;
        
        tint m_centerW;
        tint m_prevBlockMode;
        tint m_currentBlockMode;
        tint m_outCurrent;
        tint m_outReturn;
        
        tuint m_pcmEnd;
        tfloat32 **m_pcm;
        
        tint m_outPosition;
        tint m_outSize;
        tfloat32 **m_out;
        
    protected:
    
        static tfloat32 *m_windowLookup;
        
        VSilverContainer *m_info;
        
        common::Allocation m_alloc;
        
        void printError(const tchar *strR,const tchar *strE) const;
        
        bool setupWindow(engine::Sequence *seq);
        bool setupPCM();
        
        void calculateLeftWindow(tfloat32 *x,const tint shift);
        void calculateRightWindow(tfloat32 *x,const tint shift);
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
