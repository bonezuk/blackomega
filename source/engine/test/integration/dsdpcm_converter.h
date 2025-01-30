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

#include "dsdpcm_filter_setup.h"
#include <array>
#include <vector>

#ifndef _USE_IPP
#include "dsdpcm_fir.h"
#include "pcmpcm_fir.h"
#else
#include "dsdpcm_fir_ipp.h"
#include "pcmpcm_fir_ipp.h"
#endif

template<typename real_t>
class dsdpcm_converter_t {
protected:
	dsdpcm_fir_t<real_t>               dsd_filter;
	std::vector<pcmpcm_fir_t<real_t>*> pcm_filters;
	std::array<std::vector<real_t>, 2> pcm_buffers;
	size_t framerate;
	size_t dsd_samplerate;
	size_t pcm_samplerate;
	bool   is_48k;
	size_t dsd_to_pcm_ratio;
public:
	dsdpcm_converter_t(size_t p_framerate, size_t p_dsd_samplerate, size_t p_pcm_samplerate) {
		framerate = p_framerate;
		dsd_samplerate = p_dsd_samplerate;
		pcm_samplerate = p_pcm_samplerate;
		is_48k = p_pcm_samplerate % 48000 == 0;
		dsd_to_pcm_ratio = dsd_samplerate / (is_48k ? (pcm_samplerate / 48000) * 44100 : pcm_samplerate);
	}
	~dsdpcm_converter_t() {
		for (auto pcm_filter : pcm_filters) {
			delete pcm_filter;
		}
	}
	double get_delay() {
		auto delay = dsd_filter.get_delay() / dsd_filter.get_downsample_ratio();
		for (auto& pcm_filter : pcm_filters) {
			delay = (delay + pcm_filter->get_delay()) / pcm_filter->get_downsample_ratio();
		}
		return delay;
	}
	size_t convert(uint8_t* inp_data, size_t inp_size, real_t* out_data) {
		size_t pcm_samples;
		if (pcm_filters.size() > 0) {
			size_t stage{ 0 };
			pcm_samples = dsd_filter.run(inp_data, pcm_buffers[stage % 2].data(), inp_size);
			while (stage + 1 < pcm_filters.size()) {
				pcm_samples = pcm_filters[stage]->run(pcm_buffers[stage % 2].data(), pcm_buffers[(stage + 1) % 2].data(), pcm_samples);
				stage++;
			}
			pcm_samples = pcm_filters[stage]->run(pcm_buffers[stage % 2].data(), out_data, pcm_samples);
			stage++;
		}
		else {
			pcm_samples = dsd_filter.run(inp_data, out_data, inp_size);
		}
		return pcm_samples;
	};
	void set_buffers(size_t pcm_samples) {
		if (pcm_filters.size() > 0) {
			pcm_buffers[0].resize(pcm_samples);
			if (pcm_filters.size() > 1) {
				pcm_buffers[1].resize(pcm_samples / 2);
			}
		}
	}
};
