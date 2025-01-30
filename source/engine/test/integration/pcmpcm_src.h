/*
* SACD Decoder plugin
* Copyright (c) 2011-2021 Maxim V.Anisiutkin <maxim.anisiutkin@gmail.com>
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
#include "pcmpcm_fir.h"

template<typename real_t>
class pcmpcm_src_t : public pcmpcm_fir_t<real_t> {
	size_t             interpolation;
public:
	pcmpcm_src_t() {
		interpolation = 1;
		fir_coefs = nullptr;		 
		fir_order = 0;
		fir_length = 0;
		decimation = 1;
		fir_index = 0;
	}
	~pcmpcm_src_t() {
	}
	void init(real_t* p_fir_coefs, size_t p_fir_length, size_t p_decimation) {
		fir_coefs = p_fir_coefs;
		fir_order = p_fir_length - 1;
		fir_length = p_fir_length;
		decimation = p_decimation;
		fir_buffer.resize(2 * fir_length, (real_t)0);
		fir_index = 0;
	}
	int get_decimation() {
		return decimation;
	}
	double get_delay() {
		return (double)fir_order / 2;
	}
	size_t run(real_t* p_pcm_data, real_t* p_out_data, size_t p_pcm_samples) {
		auto out_samples = p_pcm_samples / decimation;
		for (auto sample = 0u; sample < out_samples; sample++) {
			for (auto i = 0u; i < decimation; i++) {
				fir_buffer[fir_index + fir_length] = fir_buffer[fir_index] = *(p_pcm_data++);
				fir_index = (++fir_index) % fir_length;
			}
			p_out_data[sample] = (real_t)0;
			for (auto j = 0u; j < fir_length; j++) {
				p_out_data[sample] += fir_coefs[j] * fir_buffer[fir_index + j];
			}
		}
		return out_samples;
	}
};
