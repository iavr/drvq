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

#ifndef SEARCH_NODE_HPP
#define SEARCH_NODE_HPP

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

template <typename T>
class node : public tree <T>
{
protected:

	typedef typename tree <T>::lab lab;    // label type
	typedef typename tree <T>::pos pos;    // position type
	typedef typename tree <T>::data data;  // data type

	const size_t K, J;            // capacity (centroids), children capacity
	const size_array dim0, dim1;  // child dimension ranges
	array<lab> code0, code1;      // positions of centroids on grid
	array_2d <lab> source;        // centroid label per bin
	array <array <lab> > edge;    // edges between neighboring centroids
	array <array <T> > weight;    // edge weights
	tree <T> *child0, *child1;    // children

//-----------------------------------------------------------------------------

	static void read(tree <T>*& t, std::istream& s)
	{
		if (ivl::read <char>(s)) t = new node <T>(s);
		else                     t = new leaf <T>(s);
	}

//-----------------------------------------------------------------------------

public:

	node(const size_t K, const size_t J,
		  const size_array& d0, const size_array& d1) :
		K(K), J(J), dim0(d0), dim1(d1), source(J, J), edge(K), weight(K)
		{ }

//-----------------------------------------------------------------------------

	node(std::istream& s) :
		K(ivl::read <size_t>(s)),
		J(ivl::read <size_t>(s)),
		dim0(read_array <size_t>()(s)),
		dim1(read_array <size_t>()(s)),
		code0(read_array <lab>()(s)),
		code1(read_array <lab>()(s)),
		source(read_array_2d <array <lab> >()(s)),
		edge(read_array <array <lab> >()(s)),
		weight(read_array <array <T> >()(s))
	{
		read(child0, s);
		read(child1, s);
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
		// TODO: (X, at[dim_]) -> (X[dim_])
		return source[child0 -> quant(X, at[dim0]) + J *
		              child1 -> quant(X, at[dim1])];
	}

	virtual array_2d <T> dist2(const data& X, const size_array& at) const
	{
		// TODO: (X, at[dim_]) -> (X[dim_])
		return (child0 -> dist2(X, at[dim0])) (_, cast <pos>(code0)) +
		       (child1 -> dist2(X, at[dim1])) (_, cast <pos>(code1));
	}

	virtual array <T> dist2(const data& X, const size_array& at,
									const array <lab>& c) const
	{
		// TODO: (X, at[dim_], ...) -> (X[dim_], ...)
		return child0 -> dist2(X, at[dim0], array <lab>(code0[c])) +
		       child1 -> dist2(X, at[dim1], array <lab>(code1[c]));   // TODO: remove array <lab> copy (ambiguity)
	}

	virtual array <array <T> > dist2(const data& X, const size_array& at,
												const array <array <lab> >& c) const
	{
		// TODO: X[at[0]] -> X[0]
		size_t N = X[at[0]].length();
		array <array <lab> > c0(N), c1(N);
		for (size_t n = 0; n < N; n++)
		{
			c0[n] = code0[c[n]];
			c1[n] = code1[c[n]];
		}
		// TODO: (X, at[dim_], ...) -> (X[dim_], ...)
		return child0 -> dist2(X, at[dim0], c0) +
		       child1 -> dist2(X, at[dim1], c1);
	}

//-----------------------------------------------------------------------------

	array <lab> quant(const data& X, const size_array& at,
							const T range) const
	{
		// quickly quantize each point x in X to q
		const array <lab> q0 = child0 -> quant(X, at[dim0]),
		                  q1 = child1 -> quant(X, at[dim1]);

		const array <array <lab> > &edge0 = child0 -> edges(),
		                           &edge1 = child1 -> edges();
		const array <array <T> > &weight0 = child0 -> weights(),
		                         &weight1 = child1 -> weights();

		// for each quantized point q, collect its neighbors in the two children
		// and keep all of their unique sources as candidates
		const size_t N = X[0].length();
		array <array <lab> > candidate(N);
		for (size_t n = 0; n < N; n++)
		{
			const array <lab> &e0 = edge0[q0[n]],
			                  &e1 = edge1[q1[n]];
			const array <T>  &w0 = weight0[q0[n]],
			                 &w1 = weight1[q1[n]];

			candidate[n] = unique_(source[join(
				cast <pos>(e0[w0 < range]) + J * pos(q1[n]),
				pos(q0[n]) + J * cast <pos>(e1[w1 < range])
			)]);
		}

		// for each point in X, find its distances to all its candidates...
		const array <array <T> > dist = dist2(X, at, candidate);

		// ... and quantize it to the candidate with the minimum distance
		array <lab> label(N);
		for (size_t n = 0; n < N; n++)
			label[n] = candidate[n][arg_min(dist[n])];
		return label;
	}

//-----------------------------------------------------------------------------

	virtual void flat(data& C, const size_array& at, const array <lab>& c)
	{
		child0 -> flat(C, at[dim0], code0[c]);
		child1 -> flat(C, at[dim1], code1[c]);
	}

};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // SEARCH_NODE_HPP
