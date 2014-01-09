/* This file is part of drvq library <http://image.ntua.gr/iva/tools/drvq>.
   A C++ library for dimensionality-recursive vector quantization.

   Copyright (c) 2013, Yannis Avrithis <iavr@image.ntua.gr>.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

//-----------------------------------------------------------------------------

#ifndef OPTIONS_TRAIN_HPP
#define OPTIONS_TRAIN_HPP

#include "data.hpp"

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

class train_options : public descriptor_options, public offline_options
{
protected:

	size_t cap_id;   // capacity id

//-----------------------------------------------------------------------------

	void set_capacity()
	{
		switch (descriptor)
		{
			case sift: switch (books)
			{
				case 4: switch(cap_id)
				{
					case 7: cap = 1 << idx(6, 7, 8, 9, 11, 16); return;
					case 6: cap = 1 << idx(6, 7, 8, 9, 11, 15); return;
					case 5: cap = 1 << idx(6, 7, 8, 9, 11, 14); return;
					case 4: cap = 1 << idx(6, 7, 8, 9, 11, 13); return;
					case 3: cap = 1 << idx(6, 7, 8, 9, 10, 12); return;
					case 2: cap = 1 << idx(5, 6, 7, 8,  9, 11); return;
					case 1: cap = 1 << idx(5, 6, 7, 8,  9, 10); return;
					case 0: cap = 1 << idx(4, 5, 6, 7,  8,  9); return;
					default: msg::error(check, "unsupported capacity");
				}

				case 2: switch(cap_id)
				{
					case 6: cap = 1 << idx(6, 7, 8, 9, 10, 12, 16); return;
					case 5: cap = 1 << idx(6, 7, 8, 9, 10, 12, 15); return;
					case 4: cap = 1 << idx(6, 7, 8, 9, 10, 12, 14); return;
					case 3: cap = 1 << idx(6, 7, 8, 9, 10, 11, 13); return;
					case 2: cap = 1 << idx(5, 6, 7, 8,  9, 10, 12); return;
					case 1: cap = 1 << idx(5, 6, 7, 8,  9, 10, 11); return;
					case 0: cap = 1 << idx(4, 5, 6, 7,  8,  9, 10); return;
					default: msg::error(check, "unsupported capacity");
				}

				case 1: switch(cap_id)
				{
					case 6: cap = 1 << idx(6, 7, 8, 9, 10, 11, 12, 16); return;
					case 5: cap = 1 << idx(6, 7, 8, 9, 10, 11, 12, 15); return;
					case 4: cap = 1 << idx(6, 7, 8, 9, 10, 11, 12, 14); return;
					case 3: cap = 1 << idx(6, 7, 8, 9, 10, 11, 12, 13); return;
					case 2: cap = 1 << idx(5, 6, 7, 8,  9, 10, 11, 13); return;
					case 1: cap = 1 << idx(5, 6, 7, 8,  9, 10, 11, 12); return;
					case 0: cap = 1 << idx(4, 5, 6, 7,  8,  9, 10, 11); return;
					default: msg::error(check, "unsupported capacity");
				}

				default: msg::error(check, "unsupported capacity");
			}

			case surf: switch(cap_id)
			{
				case 3: cap = 1 << idx(5, 6, 7, 8, 10, 12); return;
				case 2: cap = 1 << idx(5, 6, 7, 8, 9, 11);  return;
				case 1: cap = 1 << idx(4, 5, 6, 7, 8, 10);  return;
				case 0: cap = 1 << idx(4, 5, 6, 7, 8, 9);   return;
				default: msg::error(check, "unsupported capacity");
			}
		}
	}

//-----------------------------------------------------------------------------

public:

	// paths / files
	string book;     // output codebook file name

	// parameters
	size_t books;    // number of codebooks
	size_array cap;  // capacity per level; should be > 1
	double theta;    // termination parameter
	double range;    // maximum range of edges in propagation
	bool hash;       // use hash queue?
	size_t bucket;   // bucket size in hash queue

	train_options() :
		book("../out/codebook.bin"),
		books(4), cap_id(2), theta(5), range(.35),
		hash(false), bucket(20)
	{
		set_capacity();
	}

	virtual void display() const { }
};

//-----------------------------------------------------------------------------

struct train_args : public cmd_args <train_options, train_args>
{
	typedef cmd_args <train_options, train_args> base;

	template <typename C>
	void args(C cmd)
	{
		set(cmd, "output",     book,       "o", "output codebook file name");
		data_options::args(this, cmd, "training data");
		descriptor_options::args(this, cmd);

		set(cmd, "books",      books,      "b", "number of codebooks");
		set(cmd, "capacity",   cap_id,     "c", "capacity per level [SIFT: 0..7; SURF: 0..3]");
		set(cmd, "theta",      theta,      "t", "termination parameter");
		set(cmd, "range",      range,      "r", "maximum range of edges in propagation");
		set(cmd, "hash",       hash,       "H", "use hash queue?");
		set(cmd, "bucket",     bucket,     "B", "bucket size in hash queue");
		set_capacity();
	}

	train_args(int argc, char* argv[]) : base(argc, argv) { done(); }
};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // OPTIONS_TRAIN_HPP
