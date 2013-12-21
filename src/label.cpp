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

#include <ivl/ivl>
#include <ivl/system>
#include "lib/ivl.hpp"
#include "io/info.hpp"
#include "io/files.hpp"
#include "search.hpp"
#include "label.hpp"

namespace qc {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

template <bool DIST>
void label(const label_options& opt = label_options())
{
	typedef float T;
	typedef typename tree <T>::data data;  // data type
	timer t;

//-----------------------------------------------------------------------------

	// codebook input
	msg::nl(info);
	msg::in_line(info, "loading codebooks...");
	root <T>* book = root <T>::load(opt.book);
	size_t C = book->children();
	msg::require(check, book, "empty codebooks");
	msg::done(info);
	msg::book(info, book->dims(), C, book->side(), book->bins());
	if (opt.method == label_options::approx)
		for (size_t c = 0; c < C; c++)
		{
			msg::in_line(info, "codebook ", c, ": ");
			msg::edge(info, book->weights(c), opt.range);
		}
	msg::nl(info);

	// parameters
	msg::param(info, opt);
	msg::nl(info);

	// label
	msg::in_line(info, "loading + labeling data");
	if (!opt.brief()) msg::nl(info);
	labeler <T, DIST> label(book, opt, t);
	size_t F = label.files(), N = label.points();
	opt.brief() ? msg::done(info) : msg::nl(info);
	double time = t.toc();
	msg::data(info, F, N);
	msg::time(info, "total labeling time", time);
	msg::avg_time(info, "average labeling time", time / F, time / N);
	msg::nl(info);

	// output
	msg::in_line(info, "saving labels...");
	label.save(opt.label);
	msg::done(info);
	msg::nl(info);
}

//-----------------------------------------------------------------------------

}  // namespace qc

int main(int argc, char* argv[])
{
	qc::label_args opt(argc, argv);
	opt.distortion ? qc::label <true> (opt) : qc::label <false> (opt);
	return 0;
}
