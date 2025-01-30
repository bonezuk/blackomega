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
class dsdpcm_filter_setup_t	{
	using ctable_t = std::array<real_t, 256>;
	std::vector<ctable_t> dsd_fir1_8_ctables;
	std::vector<ctable_t> dsd_fir1_16_ctables;
	std::vector<ctable_t> dsd_fir1_64_ctables;
	std::vector<ctable_t> dsd_fir1_user_ctables;
	std::vector<real_t>   pcm_fir2_2_coefs;
	std::vector<real_t>   pcm_fir3_2_coefs;
	std::vector<real_t>   pcm_fir4_147_160_coefs;
	std::vector<real_t>   pcm_fir4_147_80_coefs;
	double*               dsd_fir1_user_coefs;
	size_t                dsd_fir1_user_length;
	size_t                dsd_fir1_user_decimation;
	bool                  dsd_fir1_user_modified;
public:
	dsdpcm_filter_setup_t() {
		dsd_fir1_user_coefs = nullptr;
		dsd_fir1_user_length = 0;
		dsd_fir1_user_decimation = 0;
		dsd_fir1_user_modified = false;
	}
	~dsdpcm_filter_setup_t() {
	}
	void flush_fir1_ctables() {
		dsd_fir1_8_ctables.clear();
		dsd_fir1_16_ctables.clear();
		dsd_fir1_64_ctables.clear();
		dsd_fir1_user_ctables.clear();
	}
	static double NORM_I(const int scale = 0) {
		return (double)1 / (double)((unsigned int)1 << (31 - scale));
	}
	ctable_t* get_fir1_8_ctables() {
		if (dsd_fir1_8_ctables.empty()) {
			dsd_fir1_8_ctables.resize(CTABLES(DSDFIR1_8_LENGTH));
			set_ctables(DSDFIR1_8_COEFS, DSDFIR1_8_LENGTH, NORM_I(3), dsd_fir1_8_ctables);
		}
		return dsd_fir1_8_ctables.data();
	}
	size_t get_fir1_8_length() {
		return DSDFIR1_8_LENGTH;
	}
	ctable_t* get_fir1_16_ctables() {
		if (dsd_fir1_16_ctables.empty()) {
			dsd_fir1_16_ctables.resize(CTABLES(DSDFIR1_16_LENGTH));
			set_ctables(DSDFIR1_16_COEFS, DSDFIR1_16_LENGTH, NORM_I(3), dsd_fir1_16_ctables);
		}
		return dsd_fir1_16_ctables.data();
	}
	size_t get_fir1_16_length() {
		return DSDFIR1_16_LENGTH;
	}
	ctable_t* get_fir1_64_ctables() {
		if (dsd_fir1_64_ctables.empty()) {
			dsd_fir1_64_ctables.resize(CTABLES(DSDFIR1_64_LENGTH));
			set_ctables(DSDFIR1_64_COEFS, DSDFIR1_64_LENGTH, NORM_I(), dsd_fir1_64_ctables);
		}
		return dsd_fir1_64_ctables.data();
	}
	size_t get_fir1_64_length() {
		return DSDFIR1_64_LENGTH;
	}
	ctable_t* get_fir1_user_ctables() {
		if (dsd_fir1_user_modified && dsd_fir1_user_coefs && dsd_fir1_user_length > 0) {
			dsd_fir1_user_ctables.resize(CTABLES(dsd_fir1_user_length));
			set_ctables(dsd_fir1_user_coefs, dsd_fir1_user_length, 1.0, dsd_fir1_user_ctables);
			dsd_fir1_user_modified = false;
		}
		return dsd_fir1_user_ctables.data();
	}
	size_t get_fir1_user_length() {
		return dsd_fir1_user_length;
	}
	size_t get_fir1_user_decimation() {
		return dsd_fir1_user_decimation;
	}
	real_t* get_fir2_2_coefs() {
		if (pcm_fir2_2_coefs.empty()) {
			pcm_fir2_2_coefs.resize(PCMFIR2_2_LENGTH);
			set_coefs(PCMFIR2_2_COEFS, PCMFIR2_2_LENGTH, NORM_I(), pcm_fir2_2_coefs.data());
		}
		return pcm_fir2_2_coefs.data();
	}
	size_t get_fir2_2_length() {
		return PCMFIR2_2_LENGTH;
	}
	real_t* get_fir3_2_coefs() {
		if (pcm_fir3_2_coefs.empty()) {
			pcm_fir3_2_coefs.resize(PCMFIR3_2_LENGTH);
			set_coefs(PCMFIR3_2_COEFS, PCMFIR3_2_LENGTH, NORM_I(), pcm_fir3_2_coefs.data());
		}
		return pcm_fir3_2_coefs.data();
	}
	size_t get_fir3_2_length() {
		return PCMFIR3_2_LENGTH;
	}
	real_t* get_fir4_147_160_coefs() {
		if (pcm_fir4_147_160_coefs.empty()) {
			pcm_fir4_147_160_coefs.resize(PCMFIR4_147_160_LENGTH);
			set_coefs(PCMFIR4_147_160_COEFS, PCMFIR4_147_160_LENGTH, 160, pcm_fir4_147_160_coefs.data());
		}
		return pcm_fir4_147_160_coefs.data();
	}
	size_t get_fir4_147_160_length() {
		return PCMFIR4_147_160_LENGTH;
	}
	real_t* get_fir4_147_80_coefs() {
		if (pcm_fir4_147_80_coefs.empty()) {
			pcm_fir4_147_80_coefs.resize(PCMFIR4_147_160_LENGTH);
			set_coefs(PCMFIR4_147_160_COEFS, PCMFIR4_147_160_LENGTH, 80, pcm_fir4_147_80_coefs.data());
		}
		return pcm_fir4_147_80_coefs.data();
	}
	size_t get_fir4_147_80_length() {
		return PCMFIR4_147_160_LENGTH;
	}
	void set_fir1_user_coefs(double* fir_coefs, size_t fir_length) {
		dsd_fir1_user_modified = dsd_fir1_user_coefs || fir_coefs;
		dsd_fir1_user_coefs = fir_coefs;
		dsd_fir1_user_length = fir_length;
	}
	void set_fir1_user_decimation(size_t fir_decimation) {
		dsd_fir1_user_decimation = fir_decimation;
	}
private:
	size_t set_ctables(const double* fir_coefs, const size_t fir_length, const double fir_gain, std::vector<ctable_t>& out_ctables) {
		auto ctables = CTABLES(fir_length);
		for (auto ct = 0u; ct < ctables; ct++) {
			auto k = fir_length - ct * 8;
			if (k > 8) {
				k = 8;
			}
			if (k < 0) {
				k = 0;
			}
			for (auto i = 0; i < 256; i++) {
				double cvalue{ 0.0 };
				for (auto j = 0u; j < k; j++) {
					cvalue += (((i >> (7 - j)) & 1) * 2 - 1) * fir_coefs[fir_length - 1 - (ct * 8 + j)];
				}
				out_ctables[ct][i] = (real_t)(cvalue * fir_gain);
			}
		}
		return ctables;
	}
	void set_coefs(const double* fir_coefs, const int fir_length, const double fir_gain, real_t* out_coefs) {
		for (auto i = 0; i < fir_length; i++) {
			out_coefs[i] = (real_t)(fir_coefs[fir_length - 1 - i] * fir_gain);
		}
	}
};
