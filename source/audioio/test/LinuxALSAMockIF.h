//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_TEST_LINUXALSAMOCKIF_H
#define __OMEGA_AUDIOIO_TEST_LINUXALSAMOCKIF_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/LinuxALSAIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class LinuxALSAMockIF : public LinuxALSAIF
{
    public:
        LinuxALSAMockIF();
        virtual ~LinuxALSAMockIF();
        
        MOCK_METHOD1(snd_strerror,const char *(int errnum));

        MOCK_METHOD1(snd_card_next,int(int *rcard));
        MOCK_METHOD2(snd_card_get_name,int(int card, char **name));
        MOCK_METHOD2(snd_card_get_longname,int(int card, char **name));
        
        MOCK_METHOD4(snd_pcm_open,int(snd_pcm_t **pcm, const char *name, snd_pcm_stream_t stream, int mode));
        MOCK_METHOD1(snd_pcm_close,int(snd_pcm_t *pcm));
        
        MOCK_METHOD1(snd_pcm_hw_params_malloc,int(snd_pcm_hw_params_t **ptr));
        MOCK_METHOD1(snd_pcm_hw_params_free,void(snd_pcm_hw_params_t *obj));
        MOCK_METHOD2(snd_pcm_hw_params_any,int(snd_pcm_t *pcm, snd_pcm_hw_params_t *params));
        MOCK_METHOD3(snd_pcm_hw_params_set_rate_resample,int(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val));
        MOCK_METHOD3(snd_pcm_hw_params_set_access,int(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_access_t _access));
        MOCK_METHOD3(snd_pcm_hw_params_set_channels,int(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val));
        MOCK_METHOD3(snd_pcm_hw_params_set_format,int(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, snd_pcm_format_t val));
        MOCK_METHOD4(snd_pcm_hw_params_set_rate,int(snd_pcm_t *pcm, snd_pcm_hw_params_t *params, unsigned int val, int dir));
        MOCK_METHOD2(snd_pcm_hw_params,int(snd_pcm_t *pcm, snd_pcm_hw_params_t *params));
        
        MOCK_METHOD1(snd_async_handler_get_callback_private,void *(snd_async_handler_t *handler));
        MOCK_METHOD1(snd_pcm_avail_update,snd_pcm_sframes_t(snd_pcm_t *pcm));
        MOCK_METHOD3(snd_pcm_writei,snd_pcm_sframes_t(snd_pcm_t *pcm,const void *buffer,snd_pcm_uframes_t size));
        MOCK_METHOD3(snd_pcm_htimestamp,int(snd_pcm_t *pcm,snd_pcm_uframes_t *avail,snd_htimestamp_t *tstamp));
        
        MOCK_METHOD3(snd_pcm_hw_params_get_buffer_time_max,int(const snd_pcm_hw_params_t *params,unsigned int *val,int *dir));
        MOCK_METHOD4(snd_pcm_hw_params_set_period_time_near,int(snd_pcm_t *pcm,snd_pcm_hw_params_t *params,unsigned int *val,int *dir));
        MOCK_METHOD4(snd_pcm_hw_params_set_buffer_time_near,int(snd_pcm_t *pcm,snd_pcm_hw_params_t *params,unsigned int *val,int *dir));
        MOCK_METHOD3(snd_pcm_hw_params_get_period_size,int(const snd_pcm_hw_params_t *params,snd_pcm_uframes_t *val,int *dir));
        MOCK_METHOD2(snd_pcm_hw_params_get_buffer_size,int(const snd_pcm_hw_params_t *params,snd_pcm_uframes_t *val));

        MOCK_METHOD1(snd_pcm_sw_params_malloc,int(snd_pcm_sw_params_t **ptr));
        MOCK_METHOD1(snd_pcm_sw_params_free,void(snd_pcm_sw_params_t *obj));
        MOCK_METHOD2(snd_pcm_sw_params_current,int(snd_pcm_t *pcm,snd_pcm_sw_params_t *params));
        MOCK_METHOD3(snd_pcm_sw_params_set_start_threshold,int(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_uframes_t val));
        MOCK_METHOD3(snd_pcm_sw_params_set_avail_min,int(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_uframes_t val));
        MOCK_METHOD3(snd_pcm_sw_params_set_tstamp_mode,int(snd_pcm_t *pcm,snd_pcm_sw_params_t *params,snd_pcm_tstamp_t val));
        MOCK_METHOD2(snd_pcm_sw_params,int(snd_pcm_t *pcm,snd_pcm_sw_params_t *params));

        MOCK_METHOD4(snd_async_add_pcm_handler,int(snd_async_handler_t **handler,snd_pcm_t *pcm,snd_async_callback_t callback,void *private_data));
        MOCK_METHOD1(snd_async_del_handler,int(snd_async_handler_t *handler));
        
        MOCK_METHOD1(snd_pcm_prepare,int(snd_pcm_t *pcm));
        MOCK_METHOD1(snd_pcm_drop,int(snd_pcm_t *pcm));
        MOCK_METHOD1(snd_async_handler_get_pcm,snd_pcm_t *(snd_async_handler_t *handler));
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
