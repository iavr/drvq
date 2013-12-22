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

#ifndef SEARCH_ROOT_HPP
#define SEARCH_ROOT_HPP

namespace drvq {

using namespace ivl;
using namespace std;

using ivl::exp;
using ivl::find;

//-----------------------------------------------------------------------------

template <typename T>
class assignment;

//-----------------------------------------------------------------------------

template <typename T>
class root : public ifile <root <T> >
{
	typedef types::t_true yes;
	typedef types::t_false no;

protected:

	typedef typename tree <T>::lab lab;    // label type
	typedef typename tree <T>::pos pos;    // position type
	typedef typename tree <T>::data data;  // data type

	const size_t C;                // number of children
	const size_t J;                // children capacity
	const array <size_array> dim;  // child dimension ranges
	array <tree <T>*> child;       // children

//-----------------------------------------------------------------------------

	ret <array <array <lab> >, array <array <T> > >
	knn(const array_2d <T>& dist, size_t k, T cutoff) const
	{
		size_t N = dist.rows();
		k = min(k, dist.columns());
		array <array <lab> > nn(N);
		array <array <T> > d(N);
		for (size_t n = 0; n < N; n++)
		{
			array <lab> keep = find(dist.as_rows()[n] < cutoff);
			(_, d[n], nn[n]) = head_(dist.as_rows()[n][keep], k);
			nn[n] = keep[nn[n]];
		}
		return (_, nn, d);
	}

//-----------------------------------------------------------------------------

	void read(tree <T>*& t, std::istream& s)
	{
		if (ivl::read <char>(s)) t = new node <T>(s);
		else                     t = new leaf <T>(s);
	}

//-----------------------------------------------------------------------------

public:

	root(const size_t C, const size_t J, const array <size_array>& d) :
		C(C), J(J), dim(d), child(C)
		{ }

//-----------------------------------------------------------------------------

	root(std::istream& s) :
		C(ivl::read <size_t>(s)),
		J(ivl::read <size_t>(s)),
		dim(read_array <size_array>()(s)),
		child(C)
	{
		for (size_t c = 0; c < C; c++)
			read(child[c], s);
	}

//-----------------------------------------------------------------------------

	size_t dims()           const { return max(dim[C - 1]) + 1; }
	size_t children()       const { return C; }
	size_t side()           const { return J; }
	size_t bins()           const { return _[T(J)] ->* T(C); }

	const array <array <T> >& weights(size_t c) const
	{
		return child[c]->weights();
	}

//-----------------------------------------------------------------------------

	array <pos> quant(const data& X) const { return quant(no(), X); }

	array <pos> quant(no, const data& X) const
	{
		if (X.empty()) return array <pos>();
		array <pos> l(X[0].length(), size_t(0));
		for (size_t c = 0, stride = 1; c < C; c++, stride *= J)
			l += stride * child[c] -> quant(X, dim[c]);
		return l;
	}

//-----------------------------------------------------------------------------

	ret <array <pos>, array <T> >
	quant(yes, const data& X) const
	{
		if (X.empty()) return array <pos>();
		size_t N = X[0].length();
		array <lab> q;
		array <pos> l(N, size_t(0));
		array <T> d(N, T());
		for (size_t c = 0, stride = 1; c < C; c++, stride *= J)
		{
			q = child[c] -> quant(X, dim[c]);
			l += stride * q;
			d += child[c] -> dist2(X, dim[c], q);
		}
		return (_, l, q);
	}

//-----------------------------------------------------------------------------

	array <pos> quant(const data& X, T range) const
	{
		return quant(no(), X, range);
	}

	array <pos> quant(no, const data& X, T range) const
	{
		if (X.empty()) return array <pos>();
		array <pos> l(X[0].length(), size_t(0));
		for (size_t c = 0, stride = 1; c < C; c++, stride *= J)
			l += stride * child[c] -> quant(X, dim[c], range);
		return l;
	}

//-----------------------------------------------------------------------------

	ret <array <pos>, array <T> >
	quant(yes, const data& X, T range) const
	{
		if (X.empty()) return array <pos>();
		size_t N = X[0].length();
		array <lab> q;
		array <pos> l(N, size_t(0));
		array <T> d(N, T());
		for (size_t c = 0, stride = 1; c < C; c++, stride *= J)
		{
			q = child[c] -> quant(X, dim[c], range);
			l += stride * q;
			d += child[c] -> dist2(X, dim[c], q);
		}
		return (_, l, d);
	}

//-----------------------------------------------------------------------------

	array <pos> exact(const data& X) const { return exact(no(), X); }

	array <pos>
	exact(no, const data& X) const
	{
		if (X.empty()) return array <pos>();
		array <array_2d <T> > dist(C);
		for (size_t c = 0; c < C; c++)
			dist[c] = child[c] -> dist2(X, dim[c]);
		size_t N = X[0].length();
		array <pos> l(N, size_t(0));  // TODO: l = arg_min[1](dist[0]) + J * arg_min[1](dist[1]) + ...;
		for (size_t n = 0; n < N; n++)
			for (size_t c = 0, stride = 1; c < C; c++, stride *= J)
				l[n] += stride * arg_min(dist[c].as_rows()[n]);
		return l;
	}

//-----------------------------------------------------------------------------

	ret <array <pos>, array <T> >
	exact(yes, const data& X) const
	{
		if (X.empty()) return array <pos>();
		array <array_2d <T> > dist(C);
		for (size_t c = 0; c < C; c++)
			dist[c] = child[c] -> dist2(X, dim[c]);
		size_t N = X[0].length();
		size_t q;
		array <pos> l(N, size_t(0));  // TODO: l = arg_min[1](dist[0]) + J * arg_min[1](dist[1]) + ...;
		array <T> d(N, T());
		for (size_t n = 0; n < N; n++)
			for (size_t c = 0, stride = 1; c < C; c++, stride *= J)
			{
				(_, d[n], q) = min++(dist[c].as_rows()[n]);  // TODO: dist[c].as_rows()[n][...] not compiling
				l[n] += stride * q;
			}
		return (_, l, d);
	}

//-----------------------------------------------------------------------------

	array <array_2d <T> >
	dist2(const data& X) const
	{
		if (X.empty()) return array <array_2d <T> >();
		array <array_2d <T> > dist(C);
		for (size_t c = 0; c < C; c++)
			dist[c] = child[c] -> dist2(X, dim[c]);
		return dist;
	}

//-----------------------------------------------------------------------------

	void assign(assignment <T>& a, const data& X, size_t k, T cutoff) const
	{
		if (X.empty()) { a.init(); return; }
		a.init(C);
		array <array_2d <T> > dist(C);
		for (size_t c = 0; c < C; c++)
			(_, a.nn[c], a.dist[c]) =
				knn(child[c] -> dist2(X, dim[c]), k, cutoff);
	}

//-----------------------------------------------------------------------------

	data flat()
	{
		size_t D = dims();
		data cen(D);
		for (size_t d = 0; d < D; d++)
			cen[d].init(J);

		for (size_t c = 0; c < C; c++)
			child[c] -> flat(cen, dim[c], (0, _, J - 1));

		return cen;
	}

//-----------------------------------------------------------------------------

	array <array_2d <T> >
	naive(const data& X)
	{
		data cen = flat();
		size_t N = X[0].length();
		array <array_2d <T> > dist(C);
		for (size_t c = 0; c < C; c++)
		{
			dist[c].init(idx(N, J));
			const size_array& dc = dim[c];
			for (size_t i = 0; i < dc.length(); i++)
				// TODO: += causes segmentation fault
				// dist[c] += outer_minus(X[dc[i]], cen[dc[i]]) ->* 2;
				dist[c] = dist[c] + outer_minus(X[dc[i]], cen[dc[i]]) ->* 2;
		}
		return dist;
	}

};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // SEARCH_ROOT_HPP
