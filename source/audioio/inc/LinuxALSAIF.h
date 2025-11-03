//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_LINUXALSAIF_H
#define __OMEGA_AUDIOIO_LINUXALSAIF_H
//-------------------------------------------------------------------------------------------

#include <alsa/asoundlib.h>

#include "common/inc/Factory.h"
#include "audioio/inc/AOQueryDevice.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT LinuxALSAIF
{
    public:
        LinuxALSAIF();
        virtual ~LinuxALSAIF();
        
        static QSharedPointer<LinuxALSAIF> instance(const tchar *factoryName);
        static QSharedPointer<LinuxALSAIF> instance();
        static void release();
        
        virtual const char *snd_strerror(int errnum) = 0;
        
        virtual int snd_card_next(int *rcard) = 0;
        virtual int snd_card_get_name(int card, char **name) = 0;
        virtual int snd_card_get_longname(int card, char **name) = 0;

        virtual int snd_pcm_open(snd_pcm_t **pcm, const char *name, snd_pcm_stream_t stream, int mode) = 0;
        virtual int snd_pcm_close(snd_pcm_t *pcm) = 0;

        virtual int snd_pcm_hw_params_malloc(snd_pcm_hw_params_t **ptr) = 0;
        virtual void snd_pcm_hw_params_free(snd_pcm_hw_params_t *obj) = 0;
        virtual int snd_pcm_hw_params_any(snd_pcm_t *pcm, snd_pcm_hw_params_t *params) = 0;
        virtual int snd_pcm_hw_params_set_rate_resample(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val) = 0;
        virtual int snd_pcm_hw_params_set_access(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_access_t _access) = 0;
        virtual int snd_pcm_hw_params_set_channels(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val) = 0;
        virtual int snd_pcm_hw_params_set_format(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_format_t val) = 0;
        virtual int snd_pcm_hw_params_set_rate(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val, int dir) = 0;
        virtual int snd_pcm_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t *params) = 0;

        virtual int snd_pcm_hw_params_get_buffer_time_max(const snd_pcm_hw_params_t *params,unsigned int *val,int *dir) = 0;
        virtual int snd_pcm_hw_params_set_period_time_near(snd_pcm_t *pcm,snd_pcm_hw_params_t *params,unsigned int *val,int *dir) = 0;
        virtual int snd_pcm_hw_params_set_buffer_time_near(snd_pcm_t *pcm,snd_pcm_hw_params_t *params,unsigned int *val,int *dir) = 0;
        virtual int snd_pcm_hw_params_get_period_size(const snd_pcm_hw_params_t *params,snd_pcm_uframes_t *val,int *dir) = 0;
        virtual int snd_pcm_hw_params_get_buffer_size(const snd_pcm_hw_params_t *params,snd_pcm_uframes_t *val) = 0;

        virtual int snd_pcm_sw_params_malloc(snd_pcm_sw_params_t **ptr) = 0;
        virtual void snd_pcm_sw_params_free(snd_pcm_sw_params_t *obj) = 0;
        virtual int snd_pcm_sw_params_current(snd_pcm_t *pcm,snd_pcm_sw_params_t *params) = 0;
        virtual int snd_pcm_sw_params_set_start_threshold(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_uframes_t val) = 0;
        virtual int snd_pcm_sw_params_set_avail_min(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_uframes_t val) = 0;
        virtual int snd_pcm_sw_params_set_tstamp_mode(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_tstamp_t val) = 0;
        virtual int snd_pcm_sw_params(snd_pcm_t *pcm,snd_pcm_sw_params_t *params) = 0;

        virtual int snd_async_add_pcm_handler(snd_async_handler_t **handler,snd_pcm_t *pcm,snd_async_callback_t callback,void *private_data) = 0;
        virtual int snd_async_del_handler(snd_async_handler_t *handler) = 0;
        
        virtual int snd_pcm_prepare(snd_pcm_t *pcm) = 0;
        virtual int snd_pcm_drop(snd_pcm_t *pcm) = 0;

        virtual void *snd_async_handler_get_callback_private(snd_async_handler_t *handler) = 0;
        virtual snd_pcm_sframes_t snd_pcm_avail_update(snd_pcm_t *pcm) = 0;
        virtual snd_pcm_sframes_t snd_pcm_writei(snd_pcm_t *pcm,const void *buffer,snd_pcm_uframes_t size) = 0;    
        virtual int snd_pcm_htimestamp(snd_pcm_t *pcm,snd_pcm_uframes_t *avail,snd_htimestamp_t *tstamp) = 0;
        virtual snd_pcm_t *snd_async_handler_get_pcm(snd_async_handler_t *handler) = 0;
    private:
    
        static QSharedPointer<LinuxALSAIF> m_instance;
};

typedef QSharedPointer<LinuxALSAIF> LinuxALSAIFSPtr;

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(AUDIOIO_EXPORT,LinuxALSAIFFactory,LinuxALSAIF)

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
