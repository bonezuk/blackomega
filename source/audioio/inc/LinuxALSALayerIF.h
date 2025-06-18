//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_LINUXALSALAYERIF_H
#define __OMEGA_AUDIOIO_LINUXALSALAYERIF_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/LinuxALSAIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT LinuxALSALayerIF : public LinuxALSAIF
{
    public:
        LinuxALSALayerIF();
        virtual ~LinuxALSALayerIF();

        virtual const char *snd_strerror(int errnum);

        virtual int snd_card_next(int *rcard);
        virtual int snd_card_get_name(int card, char **name);
        virtual int snd_card_get_longname(int card, char **name);

        virtual int snd_pcm_open(snd_pcm_t **pcm, const char *name, snd_pcm_stream_t stream, int mode);
        virtual int snd_pcm_close(snd_pcm_t *pcm);

        virtual int snd_pcm_hw_params_malloc(snd_pcm_hw_params_t **ptr);
        virtual void snd_pcm_hw_params_free(snd_pcm_hw_params_t *obj);
        virtual int snd_pcm_hw_params_any(snd_pcm_t *pcm, snd_pcm_hw_params_t *params);
        virtual int snd_pcm_hw_params_set_rate_resample(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val);
        virtual int snd_pcm_hw_params_set_access(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_access_t _access);
        virtual int snd_pcm_hw_params_set_channels(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val);
        virtual int snd_pcm_hw_params_set_format(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_format_t val);
        virtual int snd_pcm_hw_params_set_rate(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val, int dir);
        virtual int snd_pcm_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t *params);

        virtual void *snd_async_handler_get_callback_private(snd_async_handler_t *handler);
        virtual snd_pcm_sframes_t snd_pcm_avail_update(snd_pcm_t *pcm);
        virtual snd_pcm_sframes_t snd_pcm_writei(snd_pcm_t *pcm,const void *buffer,snd_pcm_uframes_t size);
        virtual int snd_pcm_htimestamp(snd_pcm_t *pcm,snd_pcm_uframes_t *avail,snd_htimestamp_t *tstamp);

        virtual int snd_pcm_hw_params_get_buffer_time_max(const snd_pcm_hw_params_t *params,unsigned int *val,int *dir);
        virtual int snd_pcm_hw_params_set_period_time_near(snd_pcm_t *pcm,snd_pcm_hw_params_t *params,unsigned int *val,int *dir);
        virtual int snd_pcm_hw_params_set_buffer_time_near(snd_pcm_t *pcm,snd_pcm_hw_params_t *params,unsigned int *val,int *dir);
        virtual int snd_pcm_hw_params_get_period_size(const snd_pcm_hw_params_t *params,snd_pcm_uframes_t *val,int *dir);
        virtual int snd_pcm_hw_params_get_buffer_size(const snd_pcm_hw_params_t *params,snd_pcm_uframes_t *val);

        virtual int snd_pcm_sw_params_malloc(snd_pcm_sw_params_t **ptr);
        virtual void snd_pcm_sw_params_free(snd_pcm_sw_params_t *obj);
        virtual int snd_pcm_sw_params_current(snd_pcm_t *pcm,snd_pcm_sw_params_t *params);
        virtual int snd_pcm_sw_params_set_start_threshold(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_uframes_t val);
        virtual int snd_pcm_sw_params_set_avail_min(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_uframes_t val);
        virtual int snd_pcm_sw_params_set_tstamp_mode(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_tstamp_t val);
        virtual int snd_pcm_sw_params(snd_pcm_t *pcm,snd_pcm_sw_params_t *params);

        virtual int snd_async_add_pcm_handler(snd_async_handler_t **handler,snd_pcm_t *pcm,snd_async_callback_t callback,void *private_data);
        virtual int snd_async_del_handler(snd_async_handler_t *handler);

        virtual int snd_pcm_prepare(snd_pcm_t *pcm);
        virtual int snd_pcm_drop(snd_pcm_t *pcm);
        virtual snd_pcm_t *snd_async_handler_get_pcm(snd_async_handler_t *handler);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
