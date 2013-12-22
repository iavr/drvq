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
#include "options/flat.hpp"

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

void flatten(const flat_options& opt = flat_options())
{
	typedef float T;
	typedef tree <T>::data data;  // data type

//-----------------------------------------------------------------------------

	// codebook input
	msg::nl(info);
	msg::in_line(info, "loading codebooks...");
	root <T>* book = root <T>::load(opt.book);
	size_t C = book->children();
	msg::require(check, book, "empty codebooks");
	msg::done(info);
	msg::book(info, book->dims(), C, book->side(), book->bins());
	msg::nl(info);

	// parameters
	msg::param(info, opt);
	msg::nl(info);

	// flatten
	msg::in_line(info, "flattening centroids...");
	data cen = book->flat();
	msg::done(info);

	// output
	msg::in_line(info, "saving centroids...");
	save_data(cen, opt.output);
	msg::done(info);
	msg::nl(info);
}

//-----------------------------------------------------------------------------

}  // namespace drvq

int main(int argc, char* argv[])
{
	drvq::flat_args opt(argc, argv);
	drvq::flatten(opt);
	return 0;
}
