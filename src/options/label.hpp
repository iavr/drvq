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

#ifndef OPTIONS_LABEL_HPP
#define OPTIONS_LABEL_HPP

#include "data.hpp"

namespace qc {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

struct label_options : public descriptor_options, public offline_options
{
	enum method_type { fast, approx, exact };

	// paths / files
	string book;   // codebook file name
	string label;  // output label file name

	// parameters
	bool distortion;           // use distortion (distances to labels)?
	int_<method_type> method;  // labeling method
	double range;              // range of edge weights to explore in method 1 (> 0)

	label_options() :
		book  ("../out/codebook.bin"),
		label ("../out/labels.bin"),
		distortion(true), method(exact), range(.45)
		{ }

	bool brief() const { return method == fast; }
	virtual void display() const { }
};

//-----------------------------------------------------------------------------

struct label_args : public cmd_args <label_options, label_args>
{
	typedef label_options base;

	template <typename C>
	void args(C cmd)
	{
		set(cmd, "output",     label,      "o",  "output label file name");
		set(cmd, "book",       book,       "b",  "codebook file name");
		data_options::args(this, cmd, "input data");
		descriptor_options::args(this, cmd);

		set(cmd, "distortion", distortion, "ds", "use distortion (distances to labels)?");
		set(cmd, "method",     method(),   "m",  "labeling method [0: fast, 1: approx, 2: exact]");
		set(cmd, "range",      range,      "r",  "range of edge weights to explore in method 1 (> 0)");
	}

	label_args(int argc, char* argv[]) : cmd_args(argc, argv) { done(); }
};

//-----------------------------------------------------------------------------

}  // namespace qc

#endif  // OPTIONS_LABEL_HPP
