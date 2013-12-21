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

#ifndef OPTIONS_SEARCH_HPP
#define OPTIONS_SEARCH_HPP

namespace qc {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

struct search_options
{
	// paths / files
	string book;    // codebook file name
	string index;   // index file name
	string assign;  // assignments file name
	string rank;    // output ranks file name

	//parameters
	size_t knn;     // k nearest neighbor codeword of query points
	double cutoff;  // maximum distance between query point + codeword
	double sigma2;  // similarity score variance
	double weight;  // minimum file weight
	size_t stop;    // maximum number of entries per bin (stop list)
	size_t top;     // number of top files to re-rank

	search_options() :

		book   ("../out/codebook.bin"),
		index  ("../out/index.bin"),
		assign ("../out/assign.bin"),
		rank   ("../out/ranks.bin"),

		knn(10), cutoff(.25), sigma2(.05), weight(30), stop(20), top(200)

		{ }

	virtual void display() const { }
};

//-----------------------------------------------------------------------------

struct search_args : public cmd_args <search_options, search_args>
{
	template <typename C>
	void args(C cmd)
	{
		set(cmd, "output", rank,   "o",  "output ranks file name");
		set(cmd, "book",   book,   "b",  "codebook file name");
		set(cmd, "index",  index,  "i",  "index file name");
		set(cmd, "assign", assign, "a",  "assignments file name");
		set(cmd, "knn",    knn,    "k",  "k nearest neighbor codeword of query points");
		set(cmd, "cutoff", cutoff, "c",  "maximum distance between query point + codeword");
		set(cmd, "var",    sigma2, "v",  "similarity score variance");
		set(cmd, "weight", weight, "w",  "minimum file weight");
		set(cmd, "stop",   stop,   "s",  "maximum number of entries per bin (stop list)");
		set(cmd, "top",    top,    "t",  "number of top files to re-rank");
	}

	search_args(int argc, char* argv[]) : cmd_args(argc, argv) { done(); }
};

//-----------------------------------------------------------------------------

}  // namespace qc

#endif  // OPTIONS_SEARCH_HPP
