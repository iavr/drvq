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

#ifndef DATA_NORM_HPP
#define DATA_NORM_HPP

namespace qc {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

template <typename T>
void norm_l2(array <array <T> >& X)
{
	const array <T> L2 = ivl::norm(X);
	const size_array nz = find(L2 != 0);  // TODO: array <bool>
	const size_t D = X.length();
	for (size_t d = 0; d < D; d++)
	{
		if (X[d].length() > 100000) msg::progress(info, d, D);
		X[d][nz] /= L2[nz];
	}
}

//-----------------------------------------------------------------------------

template <typename T>
void norm_root(array <array <T> >& X)
{
	if (X.empty()) return;

	array <T> L1(X[0].length(), T());
	for (size_t d = 0; d < X.length(); d++)
		L1 += abs(X[d]);  // TODO: array <T> L1 = sum(abs(X));

	const size_array nz = find(L1 != 0);  // TODO: array <bool>
	const size_t D = X.length();
	for (size_t d = 0; d < X.length(); d++)
	{
		if (X[d].length() > 100000) msg::progress(info, d, D);
		X[d][nz] /= L1[nz];
		force(X[d]) = sqrt(abs(X[d])) * sign(X[d]);  // TODO: force(X) = ...
	}
}

//-----------------------------------------------------------------------------

template <typename T, typename O>
void normalize(array <array <T> >& X, const O& opt)
{
	switch (opt.norm)
	{
		case O::l2:   norm_l2(X);   return;
		case O::root: norm_root(X); return;
	}
}

//-----------------------------------------------------------------------------

}  // namespace qc

#endif  // DATA_NORM_HPP
