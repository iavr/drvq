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

#ifndef OPTIONS_FLAT_HPP
#define OPTIONS_FLAT_HPP

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

struct flat_options
{
	enum method_type { fast, approx, exact };

	// paths / files
	string output;  // output flat codebook file name
	string book;    // codebook file name

	flat_options() :
		output("../out/centroids.bin"),
		book("../out/codebook.bin")
		{ }

	virtual void display() const { }
};

//-----------------------------------------------------------------------------

struct flat_args : public cmd_args <flat_options, flat_args>
{
	typedef cmd_args <flat_options, flat_args> base;

	template <typename C>
	void args(C cmd)
	{
		set(cmd, "output",     output,     "o",  "output flat codebook file name");
		set(cmd, "book",       book,       "b",  "codebook file name");
	}

	flat_args(int argc, char* argv[]) : base(argc, argv) { done(); }
};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // OPTIONS_FLAT_HPP
