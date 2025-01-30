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

#include <dsdpcm_decoder.h>
#include <dsdpcm_engine.h>
//#include <dsdpcm_engine_stl.h>
//#include <dsdpcm_engine_tbb.h>

class dsdpcm_decoder_t::ctx_t : public dsdpcm_engine_t {
};

dsdpcm_decoder_t::dsdpcm_decoder_t() : ctx(nullptr) {
}

dsdpcm_decoder_t::~dsdpcm_decoder_t() {
	delete ctx;
}

double dsdpcm_decoder_t::get_delay() {
	if (!ctx) {
		return 0.0;
	}
	return ctx->get_delay();
}

int dsdpcm_decoder_t::init(size_t channels, size_t framerate, size_t dsd_samplerate, size_t pcm_samplerate, conv_type_e conv_type, bool conv_fp64, double* fir_data, size_t fir_size, size_t fir_decimation) {
	if (!ctx) {
		ctx = new ctx_t();
	}
	if (!ctx) {
		return -1;
	}
	return ctx->init(channels, framerate, dsd_samplerate, pcm_samplerate, conv_type, conv_fp64, fir_data, fir_size, fir_decimation);
}

void dsdpcm_decoder_t::free() {
	if (!ctx) {
		return;
	}
	return ctx->free();
}

size_t dsdpcm_decoder_t::convert(const unsigned char* dsd_data, const size_t dsd_size, audio_sample* pcm_data) {
	if (!ctx) {
		return 0;
	}
	return ctx->convert(dsd_data, dsd_size, pcm_data);
}
