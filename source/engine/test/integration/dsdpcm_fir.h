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

#include "dsdpcm_constants.h"
#include <array>
#include <vector>

template<typename real_t>
class dsdpcm_fir_t {
protected:
	using ctable_t = std::array<real_t, 256>;
	size_t               decimation;
	ctable_t*            fir_ctables;
	size_t               fir_order;
	size_t               fir_length;
	std::vector<uint8_t> fir_buffer;
	size_t               buf_index;
public:
	dsdpcm_fir_t() {
		decimation = 1;
		fir_ctables = nullptr;
		fir_order = 0;
		fir_length = 0;
		buf_index = 0;
	}
	~dsdpcm_fir_t() {
	}
	void init(ctable_t* p_fir_ctables, size_t p_fir_length, size_t p_decimation) {
		decimation = p_decimation / 8;
		fir_ctables = p_fir_ctables;
		fir_order = p_fir_length - 1;
		fir_length = CTABLES(p_fir_length);
		buf_index = 0;
		fir_buffer.resize(fir_length, DSD_SILENCE_BYTE);
	}
	double get_downsample_ratio() {
		return double(decimation) * 8;
	}
	double get_delay() {
		return double(fir_order) / 2;
	}
	size_t run(uint8_t* p_dsd_data, real_t* p_pcm_data, size_t p_dsd_samples) {
		auto pcm_samples = p_dsd_samples / decimation;
		for (auto sample = 0u; sample < pcm_samples; sample++) {
			for (auto i = 0u; i < decimation; i++) {
				fir_buffer[buf_index] = *(p_dsd_data++);
				buf_index = (buf_index + 1) % fir_length;
			}
			p_pcm_data[sample] = real_t(0);
			for (auto i = 0u; i < fir_length; i++) {
				p_pcm_data[sample] += fir_ctables[i][fir_buffer[(buf_index + i) % fir_length]];
			}
		}
		return pcm_samples;
	}
};
