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

#pragma once

#include <cstddef>

#if defined(_M_X64) || defined(_M_ARM64)
using audio_sample = double;
#else
using audio_sample = float;
#endif

enum class conv_type_e {
	UNKNOWN = -1,
	MULTISTAGE = 0,
	DIRECT = 1,
	USER = 2
};

class dsdpcm_decoder_t final {
	class ctx_t;
	ctx_t* ctx;
public:
	dsdpcm_decoder_t();
	~dsdpcm_decoder_t();
	double get_delay();
	int init(size_t channels, size_t framerate, size_t dsd_samplerate, size_t pcm_samplerate, conv_type_e conv_type, bool conv_fp64, double* fir_data = nullptr, size_t fir_size = 0, size_t fir_decimation = 0);
	void free();
	size_t convert(const unsigned char* dsd_data, const size_t dsd_size, audio_sample* pcm_data);
};
