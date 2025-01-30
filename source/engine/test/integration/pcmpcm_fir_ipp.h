/*
* SACD Decoder plugin
* Copyright (c) 2011-2022 Maxim V.Anisiutkin <maxim.anisiutkin@gmail.com>
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
#include <ipp.h>
#include <ipp/ipps.h>

template<typename real_t>
class pcmpcm_fir_t {
	typedef typename std::conditional<std::is_same_v<real_t, float>, IppsFIRSpec_32f, IppsFIRSpec_64f>::type IppsFIRSpec;
	static constexpr bool is_fp32 = std::is_same_v<real_t, float>;
protected:
	size_t  decimation;
	size_t  interpolation;
	real_t* fir_coefs;
	size_t  fir_order;
	size_t  fir_length;

	real_t*      fir_dly;
	Ipp8u*       fir_buf;
	IppsFIRSpec* fir_spec;
	IppStatus    fir_status;
public:
	pcmpcm_fir_t() {
		decimation = 1;
		interpolation = 1;
		fir_coefs = nullptr;
		fir_order = 0;
		fir_length = 0;

		fir_dly = nullptr;
		fir_buf = nullptr;
		fir_spec = nullptr;
	}
	pcmpcm_fir_t(real_t* p_fir_coefs, size_t p_fir_length, size_t p_decimation, size_t p_interpolation = 1) {
		init(p_fir_coefs, p_fir_length, p_decimation, p_interpolation);
	}
	~pcmpcm_fir_t() {
		free();
	}
	double get_downsample_ratio() {
		return (double)decimation / interpolation;
	}
	double get_delay() {
		return (double)fir_order / 2 / interpolation;
	}
	void init(real_t* p_fir_coefs, size_t p_fir_length, size_t p_decimation, size_t p_interpolation = 1) {
		decimation = p_decimation;
		interpolation = p_interpolation;
		fir_coefs = p_fir_coefs;
		fir_length = p_fir_length;
		fir_order = p_fir_length - 1;
		auto dly_length = (fir_length + interpolation - 1) / interpolation;
		if constexpr(is_fp32) {
			fir_dly = static_cast<real_t*>(ippsMalloc_32f(dly_length));
			ippsZero_32f(fir_dly, dly_length);
		}
		else {
			fir_dly = static_cast<real_t*>(ippsMalloc_64f(dly_length));
			ippsZero_64f(fir_dly, dly_length);
		}
		int specSize{ 0 };
		int bufSize{ 0 };
		fir_status = ippsFIRMRGetSize(fir_length, interpolation, decimation, is_fp32 ? ipp32f : ipp64f, &specSize, &bufSize);
		fir_buf = ippsMalloc_8u(bufSize);
		fir_spec = reinterpret_cast<IppsFIRSpec*>(ippsMalloc_8u(specSize));
		if constexpr(is_fp32) {
			fir_status = ippsFIRMRInit_32f(fir_coefs, fir_length, interpolation, 0, decimation, 0, fir_spec);
		}
		else {
			fir_status = ippsFIRMRInit_64f(fir_coefs, fir_length, interpolation, 0, decimation, 0, fir_spec);
		}
 	}
	void free() {
		if (fir_dly) {
			ippsFree(fir_dly);
			fir_dly = nullptr;
		}
		if (fir_buf) {
			ippsFree(fir_buf);
			fir_buf = nullptr;
		}
		if (fir_spec) {
			ippsFree(fir_spec);
			fir_spec = nullptr;
		}
	}
	int run(real_t* p_pcm_data, real_t* p_out_data, int p_pcm_samples) {
		auto iters = p_pcm_samples / decimation;
		if constexpr(is_fp32) {
			fir_status = ippsFIRMR_32f(p_pcm_data, p_out_data, iters , fir_spec, fir_dly, fir_dly, fir_buf);
		}
		else {
			fir_status = ippsFIRMR_64f(p_pcm_data, p_out_data, iters, fir_spec, fir_dly, fir_dly, fir_buf);
		}
		return iters * interpolation;
	}
};
