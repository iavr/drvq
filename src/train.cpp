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
#include "train.hpp"

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

void train(const train_options& opt = train_options())
{
	typedef float T;
	typedef tree <T>::data data;  // data type
	timer t;

//-----------------------------------------------------------------------------

	// parameters
	msg::nl(info);
	msg::param(info, opt);
	msg::nl(info);
	msg::capacity(info, opt.cap);
	msg::nl(info);

	// data input
	msg::in_line(info, "loading training data");
	t.tic();
	data X = load_data <T>(opt);
	msg::require(check, X.length(), "empty training data");
	msg::done(info);
	msg::time(info, "data loaded in", t.toc());
	msg::data(info, load_lines(opt.list).length(), X);  // TODO: correct #files in case of -f
	msg::nl(info);

	msg::in_line(info, "normalizing...");
	t.tic();
	normalize(X, opt);
	msg::done(info);
	msg::nl(info);

	// train
	msg::disp(info, "training codebooks");
	msg::nl(info);
	t.tic();
	train_root <T>* book = new train_root <T>(X, opt);
	msg::time(info, "total training time", t.toc());
	msg::nl(info);

	// output
	msg::in_line(info, "saving codebooks...");
	book->save(opt.book);
	msg::done(info);
	msg::nl(info);
}

//-----------------------------------------------------------------------------

}  // namespace drvq

int main(int argc, char* argv[])
{
	drvq::train_args opt(argc, argv);
	drvq::train(opt);
	return 0;
}
