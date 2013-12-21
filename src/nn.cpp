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
#include "data/norm.hpp"
#include "search.hpp"
#include "nn.hpp"

namespace qc {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

void nn(const nn_options& opt = nn_options())
{
	typedef float T;
	typedef tree <T>::lab lab;    // label type
	typedef tree <T>::pos pos;    // position type
	typedef tree <T>::data data;  // data type
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
	for (size_t c = 0; c < C; c++)
	{
		msg::in_line(info, "codebook ", c, " degrees: ");
		msg::edge(info, book->weights(c), opt.range);
	}
	msg::nl(info);

	// query input
	msg::in_line(info, "loading query data");
	array <string> queries = load_lines(opt.list);
	bool single = opt.query >=0 && opt.query < queries.length();
	data X = single ?
		load_data <T>(opt, queries[opt.query]) :
		load_data <T>(opt);
	normalize(X, opt);
	msg::require(check, X.length(), "empty query data");
	size_t F = (single ? 1 : queries.length()), N = X[0].length();
	msg::done(info);
	msg::data(info, F, X);
	msg::nl(info);

	// parameters
	msg::param(info, opt);
	msg::nl(info);

	// naive
	msg::in_line(info, "naive exact distances (slowest)...");
	t.tic();
	array <array_2d <T> > naive = book->naive(X);
	double time = t.toc();
	msg::done(info);
	msg::time(info, "total time", time);
	msg::avg_time(info, "average time", time / F, time / N);
	nner <T> nn;
	nn.dist = naive;
	msg::nl(info);

	// exact
	msg::in_line(info, "exact distances (slow)...");
	t.tic();
	array <array_2d <T> > exact = book->dist2(X);
	time = t.toc();
	msg::done(info);
	msg::time(info, "total time", time);
	msg::avg_time(info, "average time", time / F, time / N);
	nn.verify(exact);
	msg::nl(info);

	// fast
	msg::in_line(info, "quant (fast)...");
	t.tic();
	array <pos> q = book->quant(X);
	time = t.toc();
	msg::done(info);
	msg::time(info, "total search time", time);
	msg::avg_time(info, "average search time", time / F, time / N);
	nn.eval(q, opt);
	msg::nl(info);

	// approximate
	msg::in_line(info, "quant (approximate)...");
	t.tic();
	array <pos> l = book->quant(X, opt.range);
	time = t.toc();
	msg::done(info);
	msg::time(info, "total search time", time);
	msg::avg_time(info, "average search time", time / F, time / N);
	nn.eval(l, opt);
	msg::nl(info);
}

//-----------------------------------------------------------------------------

}  // namespace qc

int main(int argc, char* argv[])
{
	qc::nn_args opt(argc, argv);
	qc::nn(opt);
	return 0;
}
