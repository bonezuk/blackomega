/*
* SACD Decoder plugin
* Copyright (c) 2011-2023 Maxim V.Anisiutkin <maxim.anisiutkin@gmail.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with FFmpeg; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "dsdpcm_converter_multistage.h"
#include "dsdpcm_converter_direct.h"
#include "dsdpcm_converter_user.h"
#include "dsdpcm_engine.h"
#include <algorithm>
#include <math.h>
#include <stdio.h>

#define LOG_ERROR   ("Error: ")
#define LOG_WARNING ("Warning: ")
#define LOG_INFO    ("Info: ")
#define LOG(p1, p2) log_printf("%s%s", p1, p2)

dsdpcm_engine_t::dsdpcm_engine_t() {
	channels = 0;
	framerate = 0;
	dsd_samplerate = 0;
	pcm_samplerate = 0;
	conv_delay = 0.0;
	conv_type = conv_type_e::UNKNOWN;
	conv_fp64 = false;
	run_threads = false;
}

dsdpcm_engine_t::~dsdpcm_engine_t() {
	free();
}

double dsdpcm_engine_t::get_delay() {
	return conv_delay;
}

int dsdpcm_engine_t::init(size_t p_channels, size_t p_framerate, size_t p_dsd_samplerate, size_t p_pcm_samplerate, conv_type_e p_conv_type, bool p_conv_fp64, double* p_fir_data, size_t p_fir_size, size_t p_fir_decimation) {
	if (p_conv_type == conv_type_e::USER) {
		if (!(p_fir_data && p_fir_size > 0 && p_fir_decimation > 0)) {
			return -2;
		}
	}
	channels = p_channels;
	framerate = p_framerate;
	dsd_samplerate = p_dsd_samplerate;
	pcm_samplerate = p_pcm_samplerate;
	conv_type = p_conv_type;
	conv_fp64 = p_conv_fp64;
	fir_data = p_fir_data;
	fir_size = p_fir_size;
	fir_decimation = p_fir_decimation;
	reinit();
	return 0;
}

void dsdpcm_engine_t::reinit() {
	free();
	if (conv_fp64) {
		if (conv_type == conv_type_e::USER) {
			fltSetup_fp64.set_fir1_user_coefs(fir_data, fir_size);
			fltSetup_fp64.set_fir1_user_decimation(fir_decimation);
		}
		init_slots(convSlots_fp64, fltSetup_fp64);
		conv_delay = convSlots_fp64[0].codec->get_delay();
	}
	else {
		if (conv_type == conv_type_e::USER) {
			fltSetup_fp32.set_fir1_user_coefs(fir_data, fir_size);
			fltSetup_fp32.set_fir1_user_decimation(fir_decimation);
		}
		init_slots(convSlots_fp32, fltSetup_fp32);
		conv_delay = convSlots_fp32[0].codec->get_delay();
	}
}

void dsdpcm_engine_t::free() {
	conv_fp64 ? free_slots(convSlots_fp64) : free_slots(convSlots_fp32);
}

size_t dsdpcm_engine_t::convert(const uint8_t* p_dsd_data, const size_t p_dsd_size, audio_sample* p_pcm_data) {
	return conv_fp64 ? convert(convSlots_fp64, p_dsd_data, p_dsd_size, p_pcm_data) : convert(convSlots_fp32, p_dsd_data, p_dsd_size, p_pcm_data);
}

template<typename real_t>
bool dsdpcm_engine_t::init_slots(std::vector<dsdpcm_slot_t<real_t>>& slots, dsdpcm_filter_setup_t<real_t>& fltSetup) {
	slots.resize(channels);
	auto dsd_samples = dsd_samplerate / 8 / framerate;
	auto pcm_samples = pcm_samplerate / framerate;
	for (auto&& slot : slots) {
		slot.inp_data.resize(dsd_samples);
		slot.out_data.resize(pcm_samples);
		switch (conv_type) {
		case conv_type_e::MULTISTAGE:
			slot.codec = new dsdpcm_converter_multistage_t<real_t>(fltSetup, framerate, dsd_samplerate, pcm_samplerate);
			break;
		case conv_type_e::DIRECT:
			slot.codec = new dsdpcm_converter_direct_t<real_t>(fltSetup, framerate, dsd_samplerate, pcm_samplerate);
			break;
		case conv_type_e::USER:
			slot.codec = new dsdpcm_converter_user_t<real_t>(fltSetup, framerate, dsd_samplerate, pcm_samplerate);
			break;
		default:
			break;
		}
		if (!slot.codec) {
			LOG(LOG_ERROR, ("Could not instantiate DSD to PCM converter"));
			return false;
		}
		run_threads = true;
		std::thread t([this, &slot]() { slot.run(run_threads); });
		if (!t.joinable()) {
			LOG(LOG_ERROR, ("Could not start DSD to PCM converter thread"));
			return false;
		}
		slot.thread = std::move(t);
	}
	return true;
}

template<typename real_t>
void dsdpcm_engine_t::free_slots(std::vector<dsdpcm_slot_t<real_t>>& slots) {
	run_threads = false;
	for (auto&& slot : slots) {
		slot.inp_semaphore.release(); // Release worker (decoding) thread for exit
		slot.thread.join(); // Wait until worker (decoding) thread exit
		delete slot.codec;
		slot.codec = nullptr;
		slot.inp_data.clear();
		slot.out_data.clear();
	}
	slots.clear();
}

template<typename real_t>
size_t dsdpcm_engine_t::convert(std::vector<dsdpcm_slot_t<real_t>>& slots, const uint8_t* inp_data, const size_t inp_size, audio_sample* out_data) {
	size_t pcm_samples{ 0 };
	size_t ch{ 0 };
	for (auto&& slot : slots) {
		for (auto sample = 0u; sample < inp_size / channels; sample++) {
			slot.inp_data[sample] = inp_data[sample * channels + ch];
		}
		slot.inp_semaphore.release(); // Release worker (decoding) thread on the loaded slot
#ifndef _USE_ST
		ch++;
	}
	ch = 0;
	for (auto&& slot : slots) {
#endif
		slot.out_semaphore.acquire();	// Wait until worker (decoding) thread is complete
		for (auto sample = 0u; sample < slot.out_data.size(); sample++) {
			out_data[sample * channels + ch] = (float)slot.out_data[sample];
		}
		pcm_samples += slot.out_data.size();
		ch++;
	}
	return pcm_samples;
}

void log_printf(const char* fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vfprintf(stdout, fmt, argptr);
    va_end(argptr);
}
