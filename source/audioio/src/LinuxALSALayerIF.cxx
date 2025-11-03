//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

#include "audioio/inc/LinuxALSALayerIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(LinuxALSAIFFactory,LinuxALSAIF, \
                            LinuxALSALayerIFFactory,LinuxALSALayerIF, \
                            "alsa",false)

//-------------------------------------------------------------------------------------------

LinuxALSALayerIF::LinuxALSALayerIF() : LinuxALSAIF()
{}

//-------------------------------------------------------------------------------------------

LinuxALSALayerIF::~LinuxALSALayerIF()
{}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_card_next(int *rcard)
{
    return ::snd_card_next(rcard);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_card_get_name(int card, char **name)
{
    return ::snd_card_get_name(card,name);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_card_get_longname(int card, char **name)
{
    return ::snd_card_get_longname(card,name);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_open(snd_pcm_t **pcm, const char *name, snd_pcm_stream_t stream, int mode)
{
    return ::snd_pcm_open(pcm,name,stream,mode);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_close(snd_pcm_t *pcm)
{
    return ::snd_pcm_close(pcm);
}

//-------------------------------------------------------------------------------------------

const char *LinuxALSALayerIF::snd_strerror(int errnum)
{
    return ::snd_strerror(errnum);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_malloc(snd_pcm_hw_params_t **ptr)
{
    return ::snd_pcm_hw_params_malloc(ptr);
}

//-------------------------------------------------------------------------------------------

void LinuxALSALayerIF::snd_pcm_hw_params_free(snd_pcm_hw_params_t *obj)
{
    return ::snd_pcm_hw_params_free(obj);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_any(snd_pcm_t *pcm, snd_pcm_hw_params_t *params)
{
    return ::snd_pcm_hw_params_any(pcm,params);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_set_rate_resample(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val)
{
    return ::snd_pcm_hw_params_set_rate_resample(pcm,params,val);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_set_access(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_access_t _access)
{
    return ::snd_pcm_hw_params_set_access(pcm,params,_access);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_set_channels(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val)
{
    return ::snd_pcm_hw_params_set_channels(pcm,params,val);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_set_format(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_format_t val)
{
    return ::snd_pcm_hw_params_set_format(pcm,params,val);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_set_rate(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val, int dir)
{
    return ::snd_pcm_hw_params_set_rate(pcm,params,val,dir);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t *params)
{
    return ::snd_pcm_hw_params(pcm,params);
}

//-------------------------------------------------------------------------------------------

void *LinuxALSALayerIF::snd_async_handler_get_callback_private(snd_async_handler_t *handler)
{
    return ::snd_async_handler_get_callback_private(handler);
}

//-------------------------------------------------------------------------------------------

snd_pcm_sframes_t LinuxALSALayerIF::snd_pcm_avail_update(snd_pcm_t *pcm)
{
    return ::snd_pcm_avail_update(pcm);
}

//-------------------------------------------------------------------------------------------

snd_pcm_sframes_t LinuxALSALayerIF::snd_pcm_writei(snd_pcm_t *pcm,const void *buffer,snd_pcm_uframes_t size)
{
    return ::snd_pcm_writei(pcm,buffer,size);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_htimestamp(snd_pcm_t *pcm,snd_pcm_uframes_t *avail,snd_htimestamp_t *tstamp)
{
    return ::snd_pcm_htimestamp(pcm,avail,tstamp);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_get_buffer_time_max(const snd_pcm_hw_params_t *params,unsigned int *val,int *dir)
{
    return ::snd_pcm_hw_params_get_buffer_time_max(params,val,dir);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_set_period_time_near(snd_pcm_t *pcm,snd_pcm_hw_params_t *params,unsigned int *val,int *dir)
{
    return ::snd_pcm_hw_params_set_period_time_near(pcm,params,val,dir);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_set_buffer_time_near(snd_pcm_t *pcm,snd_pcm_hw_params_t *params,unsigned int *val,int *dir)
{
    return ::snd_pcm_hw_params_set_buffer_time_near(pcm,params,val,dir);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_get_period_size(const snd_pcm_hw_params_t *params,snd_pcm_uframes_t *val,int *dir)
{
    return ::snd_pcm_hw_params_get_period_size(params,val,dir);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_hw_params_get_buffer_size(const snd_pcm_hw_params_t *params,snd_pcm_uframes_t *val)
{
    return ::snd_pcm_hw_params_get_buffer_size(params,val);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_sw_params_malloc(snd_pcm_sw_params_t **ptr)
{
    return ::snd_pcm_sw_params_malloc(ptr);
}

//-------------------------------------------------------------------------------------------

void LinuxALSALayerIF::snd_pcm_sw_params_free(snd_pcm_sw_params_t *obj)
{
    ::snd_pcm_sw_params_free(obj);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_sw_params_current(snd_pcm_t *pcm,snd_pcm_sw_params_t *params)
{
    return ::snd_pcm_sw_params_current(pcm,params);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_sw_params_set_start_threshold(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_uframes_t val)
{
    return ::snd_pcm_sw_params_set_start_threshold(pcm,params,val);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_sw_params_set_avail_min(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_uframes_t val)
{
    return ::snd_pcm_sw_params_set_avail_min(pcm,params,val);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_sw_params_set_tstamp_mode(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_tstamp_t val)
{
    return ::snd_pcm_sw_params_set_tstamp_mode(pcm,params,val);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_sw_params(snd_pcm_t *pcm,snd_pcm_sw_params_t *params)
{
    return ::snd_pcm_sw_params(pcm,params);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_async_add_pcm_handler(snd_async_handler_t **handler,snd_pcm_t *pcm,snd_async_callback_t callback,void *private_data)
{
    return ::snd_async_add_pcm_handler(handler,pcm,callback,private_data);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_async_del_handler(snd_async_handler_t *handler)
{
    return ::snd_async_del_handler(handler);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_prepare(snd_pcm_t *pcm)
{
    return ::snd_pcm_prepare(pcm);
}

//-------------------------------------------------------------------------------------------

int LinuxALSALayerIF::snd_pcm_drop(snd_pcm_t *pcm)
{
    return ::snd_pcm_drop(pcm);
}

//-------------------------------------------------------------------------------------------

snd_pcm_t *LinuxALSALayerIF::snd_async_handler_get_pcm(snd_async_handler_t *handler)
{
    return ::snd_async_handler_get_pcm(handler);
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
