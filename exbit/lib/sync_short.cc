/*
 * Copyright (C) 2013, 2016 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ieee802_11/sync_short.h>
#include <gnuradio/io_signature.h>
#include "utils.h"

#include <iostream>

using namespace gr::ieee802_11;

//static const int MIN_GAP = 480; // 160 short +160 long + 160 of sig and data field
static const int MIN_GAP = 960; // for ac short+long+sig+data
//static const int MAX_SAMPLES = 540 * 80;
static const int MAX_SAMPLES = 540 * 160; // for ac i think 80 will be 160; not sure yet

class sync_short_impl : public sync_short {

public:
sync_short_impl(double threshold, unsigned int min_plateau, bool log, bool debug) :
		block("sync_short",
			gr::io_signature::make3(3, 3, sizeof(gr_complex), sizeof(gr_complex), sizeof(float)),
			gr::io_signature::make(1, 1, sizeof(gr_complex))),
		d_log(log),
		d_debug(debug),
		d_state(SEARCH),
		d_plateau(0),
		d_freq_offset(0),
		d_copied(0),
		MIN_PLATEAU(min_plateau),
		d_threshold(threshold) {

	set_tag_propagation_policy(block::TPP_DONT);
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];
	const gr_complex *in_abs = (const gr_complex*)input_items[1];
	const float *in_cor = (const float*)input_items[2];
	gr_complex *out = (gr_complex*)output_items[0];

	int noutput = noutput_items;
	int ninput = std::min(std::min(ninput_items[0], ninput_items[1]), ninput_items[2]);

	// dout << "SHORT noutput : " << noutput << " ninput: " << ninput_items[0] << std::endl;
	 //cout << "Hello World!Hello World!Hello World!";

	switch(d_state) {

	case SEARCH: {
		int i;

		for(i = 0; i < ninput; i++) {
			if(in_cor[i] > d_threshold) {
				if(d_plateau < MIN_PLATEAU) {
					d_plateau++;

				} else {
					d_state = COPY;
					d_copied = 0;
					d_freq_offset = arg(in_abs[i]) / 32;
					d_plateau = 0;
					insert_tag(nitems_written(0), d_freq_offset, nitems_read(0) + i);
					dout << "SHORT Frame!" << std::endl;
					break;
				}
			} else {
				d_plateau = 0;
			}
		}

		//consume_each(i); 
		//consume_each(std::max(0, i-int(MIN_PLATEAU)-80)); // Dr. Hanif
		consume_each(std::max(0, i-int(MIN_PLATEAU)-160));// for ac
		return 0;
	}

	case COPY: {

		int o = 0;
		//int counter = 0;
		//int s = 0, e = 0;
		//int z = 1;
		while( o < ninput && o < noutput && d_copied < MAX_SAMPLES) {
			if(in_cor[o] > d_threshold) {
				//counter ++;
				//if (counter == 1){
				//std::cout << "first correlation exceeds threshold " << in_cor[o] << "at "<< o << std::endl;
				//s = o; // first short preamble's index
				//e = o+160; // end of preamble
				//std::cout << "e value : " << e << std::endl; 
				
				if(d_plateau < MIN_PLATEAU) {
					d_plateau++;
				} 
				// there's another frame
				else if(d_copied > MIN_GAP) {
					d_copied = 0;
					d_plateau = 0;
					//d_freq_offset = arg(in_abs[o]) / 16;
					d_freq_offset = arg(in_abs[o]) / 32; //for ac its 32, each STS takes 32 samples
					insert_tag(nitems_written(0) + o, d_freq_offset, nitems_read(0) + o);
					dout << "SHORT Frame!" << std::endl;
					//std::cout << "Hi"<< std::endl;
					break;
				}

			} 
			else {
				d_plateau = 0;
			}
				
			out[o] = in[o] * exp(gr_complex(0, -d_freq_offset * d_copied));
			//out[o] = in[o] * exp(gr_complex(0, -0 * d_copied));
			
			
			
			//if(o >= 0 && o < 3) {
				//std::cout << "Sample " << o << ": "<< std::endl;
				//std::cout << "short: d_freq_offset at "<< o <<" : " << d_freq_offset << std::endl;
				//std::cout << "short: exp at "<< o <<" : " << exp(gr_complex(0, -0 * d_copied)) << std::endl;
			//}

			// printing STSs
			//if(o >= 0 && o < 640) {
				//std::cout << "RX Preamble Sample from sync_short_in" << o << " : "<< in[o] << std::endl;
				//std::cout << "RX Preamble Sample from sync_short_out" << o << " : "<< out[o] << std::endl;
			//}
			
			
			
			o++;
			d_copied++;
		}

		if(d_copied == MAX_SAMPLES) {
			d_state = SEARCH;
		}

		dout << "SHORT copied " << o << std::endl;

		consume_each(o);
		return o;
	}
	}

	throw std::runtime_error("sync short: unknown state");
	return 0;
}

void insert_tag(uint64_t item, double freq_offset, uint64_t input_item) {
	mylog(boost::format("frame start at in: %2% out: %1%") % item % input_item);

	const pmt::pmt_t key = pmt::string_to_symbol("wifi_start");
	const pmt::pmt_t value = pmt::from_double(freq_offset);
	const pmt::pmt_t srcid = pmt::string_to_symbol(name());
	add_item_tag(0, item, key, value, srcid);
}

private:
	enum {SEARCH, COPY} d_state;
	int d_copied;
	int d_plateau;
	float d_freq_offset;
	const double d_threshold;
	const bool d_log;
	const bool d_debug;
	const unsigned int MIN_PLATEAU;
};

sync_short::sptr
sync_short::make(double threshold, unsigned int min_plateau, bool log, bool debug) {
	return gnuradio::get_initial_sptr(new sync_short_impl(threshold, min_plateau, log, debug));
}
