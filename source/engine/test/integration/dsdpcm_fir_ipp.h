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
#include <ipp.h>
#include <ipp/ipps.h>

template<typename real_t>
class dsdpcm_fir_t {
	static constexpr bool is_fp32 = std::is_same_v<real_t, float>;
protected:
	using ctable_t = std::array<real_t, 256>;
	ctable_t* fir_ctables;
	size_t    fir_order;
	size_t    fir_length;
	size_t    decimation;

	Ipp8u*  fir_dly;
	real_t* fir_out;
public:
	dsdpcm_fir_t() {
		fir_ctables = nullptr;
		fir_order = 0;
		fir_length = 0;
		decimation = 1;
		fir_dly = nullptr;
	}
	~dsdpcm_fir_t() {
		free();
	}
	double get_downsample_ratio() {
		return double(decimation) * 8;
	}
	double get_delay() {
		return double(fir_order) / 2;
	}
	void init(ctable_t* p_fir_ctables, size_t p_fir_length, size_t p_decimation) {
		fir_ctables = p_fir_ctables;
		fir_order = p_fir_length - 1;
		fir_length = CTABLES(p_fir_length);
		decimation = p_decimation / 8;
		fir_dly = ippsMalloc_8u(fir_length);
		memset(fir_dly, DSD_SILENCE_BYTE, fir_length);
		if constexpr(is_fp32) {
			fir_out = static_cast<real_t*>(ippsMalloc_32f(fir_length));
		}
		else {
			fir_out = static_cast<real_t*>(ippsMalloc_64f(fir_length));
		}
	}
	void free() {
		if (fir_dly) {
			ippsFree(fir_dly);
			fir_dly = nullptr;
		}
		if (fir_out) {
			ippsFree(fir_out);
			fir_out = nullptr;
		}
	}
	size_t run(uint8_t* p_dsd_data, real_t* p_pcm_data, size_t p_dsd_samples) {
		auto pcm_samples = p_dsd_samples / decimation;
		auto buf_index = 0u;
		for (auto sample = 0u; sample < pcm_samples; sample++) {
			for (auto i = 0u; buf_index + i < fir_length; i++) {
				fir_out[i] = fir_ctables[i][fir_dly[buf_index + i]];
			}
			for (auto i = (fir_length > buf_index) ? fir_length - buf_index : 0u; i < fir_length; i++) {
				fir_out[i] = fir_ctables[i][p_dsd_data[buf_index + i - fir_length]];
			}
			buf_index += decimation;
			if constexpr(is_fp32) {
				ippsSum_32f(fir_out, fir_length, &p_pcm_data[sample], ippAlgHintNone);
			}
			else {
				ippsSum_64f(fir_out, fir_length, &p_pcm_data[sample]);
			}
		}
		ippsCopy_8u(&p_dsd_data[p_dsd_samples - fir_length], fir_dly, fir_length);
		return pcm_samples;
	}
};
