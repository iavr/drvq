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

#ifndef SEARCH_LEAF_HPP
#define SEARCH_LEAF_HPP

namespace drvq {

using namespace ivl;
using namespace std;

using ivl::max;
using ivl::min;

//-----------------------------------------------------------------------------

template <typename T>
class leaf : public tree <T>
{
protected:

	typedef typename tree <T>::lab lab;    // label type
	typedef typename tree <T>::data data;  // data type

	const T base;               // data interval minimum
	const T length;             // data interval length + eps
	const T bin;                // bin size
	const size_t K;             // number of centroids
	array <array <T> > cen_nd;  // centroids as nd
	array <T>& cen;             // centroids
	array <lab> source;         // centroid label per bin
	array <array <lab> > edge;    // edges between neighboring centroids
	array <array <T> > weight;  // edge weights

//-----------------------------------------------------------------------------

public:

	leaf
	(
		const T min,     // data interval minimum
		const T max,     // data interval maximum
		const size_t B,  // number of bins
		const size_t K   // number of centroids; should be K > 1
	) :
		base(min), length(max - base + eps_<T>()), bin(length / B),
		K(K), cen_nd(1), cen(cen_nd[0]), source(B), edge(K), weight(K)
		{ }

//-----------------------------------------------------------------------------

	leaf(std::istream& s) :
		base(read <T>(s)),
		length(),
		bin(read <T>(s)),
		K(read <size_t>(s)),
		cen_nd(1), cen(cen_nd[0])
	{
		cen = read_array <T>()(s);
		source = read_array <lab>()(s);
		edge = read_array <array <lab> >()(s);
		weight = read_array <array <T> >()(s);
	}

//-----------------------------------------------------------------------------

	virtual size_t size() const { return K; }

	virtual const array <array <lab> >&
	edges() const { return edge; }

	virtual const array <array <T> >&
	weights() const { return weight; }

//-----------------------------------------------------------------------------

	virtual array <lab> quant(const data& X, const size_array& at) const
	{
		// TODO: X[at[0]] -> X[0]
		return source[(X[at[0]] - base) / bin];
		// return source[max(0, min(source.length() - 1, (X[at[0]] - base) / bin))];  // TODO: fixed ranges
	}

	// !!! DUMMY - only supported by nodes !!!
	virtual array <lab> quant(const data&, const size_array&, const T) const
	{
		return array <lab>();
	}

	virtual array_2d <T> dist2(const data& X, const size_array& at) const
	{
		// TODO: x[at[0]] -> x[0]
		return outer_minus(X[at[0]], cen) ->* 2;
	}

	virtual array <T> dist2(const data& X, const size_array& at,
									const array <lab>& c) const
	{
		// TODO: x[at[0]] -> x[0]
		return (X[at[0]] - cen[c]) ->* 2;
	}

	virtual array <array <T> > dist2(const data& X, const size_array& at,
												const array <array <lab> >& c) const
	{
		// TODO: x[at[0]] -> x[0]
		size_t N = X[at[0]].length();
		array <array <T> > dist(N);
		for (size_t n = 0; n < N; n++)
			dist[n] = (X[at[0]][n] - cen[c[n]]) ->* 2;
		return dist;
	}

//-----------------------------------------------------------------------------

	virtual void flat(data& C, const size_array& at, const array <lab>& c)
	{
		C[at[0]] = cen[c];
	}

};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // SEARCH_LEAF_HPP
