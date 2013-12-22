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

#ifndef NN_HPP
#define NN_HPP

#include "options/nn.hpp"

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

template <typename T>
class nner
{
	typedef typename tree <T>::lab lab;    // label type
	typedef typename tree <T>::pos pos;    // position type
	typedef typename tree <T>::data data;  // data type

//-----------------------------------------------------------------------------

public:
	array <array_2d <T> > dist;

//-----------------------------------------------------------------------------

	void verify(const array <array_2d <T> >& d)
	{
		msg::in_line(info, "verifying correctness");

		bool ok = true;
		size_t C = dist.length(), N = dist[0].rows();
		for (size_t n = 0; n < N; n++)
		{
			msg::progress(info, n, N);
			for (size_t c = 0; c < C; c++)
			{
				size_t a = arg_min(dist[c].as_rows()[n]),
				       b = arg_min(d   [c].as_rows()[n]);
				ok &= (a == b);
			}
		}

		msg::test(info, ok);
	}

//-----------------------------------------------------------------------------

	void eval(const array <pos>& nn, const nn_options& opt)
	{
		msg::in_line(info, "evaluating");

		const size_array at = (0, _, opt.at_step, _, opt.at_max);
		array <T> recall(at.length(), T());
		T R = .95;
		array <T> rat = (R, _, opt.rat_step, _, opt.rat_max);
		array <T> freq(rat.length());
		size_t FL = freq.length();
		size_t N = nn.length(), C = dist.length(), J = dist[0].columns();
		T rank = 0, lower = 0, upper = 0, approx = 0;

		for (size_t n = 0; n < N; n++)
		{
			msg::progress(info, n, N);
			pos quot = nn[n];

			for (size_t c = 0; c < C; c++)
			{
				array <T> d = dist[c].as_rows()[n];
				T dq = d[quot % J], dl = min(d);
				T r = find(d < dq).length();
				rank   += r;
				recall[r > at]++;
				freq[(min(dq / dl, opt.rat_max) - R) / opt.rat_step]++;
				lower  += dl;
				upper  += max(d);
				approx += dq;
				quot   /= J;
			}
		}
		T S = C * N;
		msg::done(info);
		msg::rank(info, rank / S, J);
		msg::bounds(info, lower / S, approx / S, upper / S);

		if (opt.recall)
			msg::recall(info, at, 1 - recall / S);

		if (opt.ratio)
			msg::ratio(info, rat, freq / S);
	}

};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // NN_HPP
