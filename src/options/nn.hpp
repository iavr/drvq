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

#ifndef OPTIONS_NN_HPP
#define OPTIONS_NN_HPP

#include "data.hpp"

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

struct nn_options : public descriptor_options, public online_options
{
	// paths / files
	string book;   // codebook file name
	int query;     // query file id

	// parameters
	double range;    // range of edge weights to explore (> 0)
	bool   recall;   // measure recall@R
	size_t at_max;   // maximum recall@ position
	size_t at_step;  // recall@ position step
	bool   ratio;    // measure distance ratio
	double rat_max;  // maximum ratio
	double rat_step; // ratio step

	nn_options() :
		book("../out/codebook.bin"),
		query(-1), range(.45),
		recall(true), at_max(120), at_step(10),
		ratio(false), rat_max(2), rat_step(.05)
		{ }

	virtual void display() const { }
};

//-----------------------------------------------------------------------------

struct nn_args : public cmd_args <nn_options, nn_args>
{
	template <typename C>
	void args(C cmd)
	{
		set(cmd, "book",       book,       "b", "codebook file name");
		data_options::args(this, cmd, "query data");
		descriptor_options::args(this, cmd);

		set(cmd, "query",        query,      "q",  "query file id");
		set(cmd, "range",        range,      "r",  "range of edge weights to explore (> 0)");
		set(cmd, "recall",       recall,     "R",  "measure recall@R");
		set(cmd, "recall_max",   at_max,     "Rm", "maximum recall@ position");
		set(cmd, "recall_step",  at_step,    "Rs", "recall@ position step");
		set(cmd, "ratio",        ratio,      "H",  "measure distance ratio histogram");
		set(cmd, "rat_max",      rat_max,    "Hm", "maximum distance ratio");
		set(cmd, "rat_step",     rat_step,   "Hs", "distance ratio step");
	}

	nn_args(int argc, char* argv[]) : cmd_args(argc, argv) { done(); }
};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // OPTIONS_NN_HPP
