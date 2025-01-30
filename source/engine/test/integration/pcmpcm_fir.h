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
#include <vector>

template<typename real_t>
class pcmpcm_fir_t {
protected:
	size_t              decimation;
	size_t              interpolation;
	real_t*             fir_coefs;
	size_t              fir_order;
	size_t              fir_length;
	std::vector<real_t> fir_buffer;
	size_t              buf_length;
	size_t              buf_index;
	size_t              out_index;
public:
	pcmpcm_fir_t() {
		decimation = 1;
		interpolation = 1;
		fir_coefs = nullptr;
		fir_order = 0;
		fir_length = 0;
		buf_length = 0;
		buf_index = 0;
		out_index = 0;
	}
	pcmpcm_fir_t(real_t* p_fir_coefs, size_t p_fir_length, size_t p_decimation, size_t p_interpolation = 1) {
		init(p_fir_coefs, p_fir_length, p_decimation, p_interpolation);
	}
	~pcmpcm_fir_t() {
	}
	void init(real_t* p_fir_coefs, size_t p_fir_length, size_t p_decimation, size_t p_interpolation = 1) {
		decimation = p_decimation;
		interpolation = p_interpolation;
		fir_coefs = p_fir_coefs;
		fir_order = p_fir_length - 1;
		fir_length = p_fir_length;
		buf_length = (interpolation > 1) ? (fir_length + interpolation) / interpolation : fir_length;
		buf_index = 0;
		out_index = 0;
		fir_buffer.resize(buf_length, real_t(0));
	}
	double get_downsample_ratio() {
		return (double)decimation / interpolation;
	}
	double get_delay() {
		return (double)fir_order / 2 / interpolation;
	}
	size_t run(real_t* p_pcm_data, real_t* p_out_data, size_t p_pcm_samples) {
		size_t out_samples;
		if (interpolation > 1) {
			out_samples = (p_pcm_samples * interpolation) / decimation;
			for (auto sample = 0u; sample < out_samples; sample++) {
				out_index += decimation;
				while (out_index >= interpolation) {
					fir_buffer[buf_index] = *(p_pcm_data++);
					buf_index = (buf_index + 1) % buf_length;
					out_index -= interpolation;
				}
				p_out_data[sample] = real_t(0);
				size_t k{ 0 };
				for (auto i = out_index; i < fir_length; i += interpolation) {
					p_out_data[sample] += fir_coefs[i] * fir_buffer[(buf_index + k++) % buf_length];
				}
			}
		}
		else {
			out_samples = p_pcm_samples / decimation;
			for (auto sample = 0u; sample < out_samples; sample++) {
				for (auto i = 0u; i < decimation; i++) {
					fir_buffer[buf_index] = *(p_pcm_data++);
					buf_index = (buf_index + 1) % buf_length;
				}
				p_out_data[sample] = real_t(0);
				for (auto i = 0u; i < fir_length; i++) {
					p_out_data[sample] += fir_coefs[i] * fir_buffer[(buf_index + i) % buf_length];
				}
			}
		}
		return out_samples;
	}
};
