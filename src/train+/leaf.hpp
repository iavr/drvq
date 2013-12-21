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

#ifndef TRAIN_LEAF_HPP
#define TRAIN_LEAF_HPP

namespace qc {

using namespace ivl;
using namespace std;

using ivl::min;
using ivl::max;
using ivl::find;
using ivl::rand;
using ivl::sort;

//-----------------------------------------------------------------------------

template <typename T>
class train_leaf : public leaf <T>, public train_tree <T>
{
	typedef typename tree <T>::lab lab;                 // label type
	typedef typename tree <T>::data data;               // data type
	typedef typename train_tree <T>::count count;       // count type
	typedef typename train_tree <T>::book book;         // codeword type
	typedef sample_search_generator <count> generator;  // random generator type

	enum { R = 64 };          // resolution (number of bins per centroid)
	using leaf <T>::base;     // training interval minimum
	using leaf <T>::length;   // training interval length + eps
	using leaf <T>::bin;      // bin size
	using leaf <T>::K;        // number of centroids
	using leaf <T>::cen_nd;   // centroids as nd
	using leaf <T>::cen;      // centroids
	using leaf <T>::source;   // centroid label per bin
	using leaf <T>::edge;     // edges between neighboring centroids
	using leaf <T>::weight;   // edge weights
	array <lab> label;        // centroid label per data point

	// temporary
	generator gen;            // random sample generator
	array <bool> chosen;      // is each bin chosen as a sample?

//-----------------------------------------------------------------------------

	void train
	(
		const array <T>& X,       // data points
		const train_options& opt  // training options
	)
	{
		// trivial cases
		if (X.empty() || K < 2) return;

		// initialize
		const size_t B = K * R;         // number of bins
		size_array zero, nz;            // empty and non-empty bins
		const array <T> val =
			base + (0, _, B - 1) * bin;  // values at all bins

		// quantize data points into bins
		array <T> b_mass(B, T());                  // mass per bin
		array <count> pop(K), b_pop(B, count(0));  // population per centroid / bin
		array <lab> code = (X - base) / bin;       // nearest bin per data point
		b_pop[code]++;
		b_mass[code] += X;

		// initialize centroids & assignments
		gen = sample_search(b_pop);               // generator on bin distribution
		chosen.init(B, false);                    // is each bin chosen as a sample?
		sample(val);
		assign();
 		array <lab> prev = source;                // label @ previous iteration

		for (size_t it = 0; ; it++)
		{
			// update populations
			pop[_] = 0;
			pop[source] += b_pop;
			(_, zero, nz) = split++(pop == count(0));

			// update centroids
			cen[_] = T();
			cen[source] += b_mass;
			cen[nz] /= pop[nz];

			// re-sample empty centroids
			sample(val, zero);

			// update assignments
			assign();

			// OPTIONAL: total distortion (sum of square residuals)
			// T distortion = sum(b_pop * (val - cen[source]) ->* 2);

			// terminate?
			double ratio = opt.theta * _[it * .01] ->* 2;
			if (find(source != prev).length() < B * ratio)
				break;
			prev = source;
		}

		// re-sample empty centroids
		sample(val, zero);

		// re-assign, and check for empty centroids
		assign();
		msg::zero(check, K, source, b_pop);

		// generate centroid edges
		generate_edges();

		// store data labels
		label = source[code];

		// clear temporaries
		gen.clear();
		chosen.init();
	}

//-----------------------------------------------------------------------------

	void sample(const array <T>& val)
	{
		cen = sort(val[gen(K, chosen)]);  // samples with replacement
	}

	void sample(const array <T>& val, const size_array& which)
	{
		cen[which] = val[gen(which.length(), chosen)];
		cen = sort(cen);
	}

//-----------------------------------------------------------------------------

	void assign()
	{
		const size_t B = K * R;
		size_array mid =
			((cen[0, _, K - 2] + cen[1, _, K - 1]) / 2 - base) / bin;
		source[0, _, mid[0] - 1] = 0;
		source[mid[K - 2], _, B - 1] = K - 1;
		for (size_t k = 1; k < K - 1; k++)
			source[mid[k - 1], _, mid[k] - 1] = k;
	}

//-----------------------------------------------------------------------------

	void generate_edges()
	{
		edge[0]       = arr(0, 1);
		edge[K - 1]   = arr(K - 1, K - 2);
		weight[0]     = arr(T(), _[cen[1] - cen[0]] ->* 2);
		weight[K - 1] = arr(T(), _[cen[K - 1] - cen[K - 2]] ->* 2);

		for (size_t k = 1; k < K - 1; k++)
		{
			edge[k] = arr(k, k - 1, k + 1);
			weight[k] = arr(T(), _[cen[k] - cen[k - 1]] ->* 2,
								      _[cen[k] - cen[k + 1]] ->* 2);
		}
	}

//-----------------------------------------------------------------------------

public:

	train_leaf
	(
		const array <T>& X,       // data points
		const train_options& opt  // training options
	) :
	leaf <T>
	(
		min(X),          // min
		max(X),          // max
		opt.cap[0] * R,  // B
		opt.cap[0]       // K
	)
	{
		train(X, opt);
	}

//-----------------------------------------------------------------------------

	virtual void write(std::ostream& s) const
	{
		char type = 0;
		ivl::write(type, s);
		ivl::write(base, s);
		ivl::write(bin, s);
		ivl::write(K, s);
		write_array(cen, s);
		write_array(source, s);
		write_array(edge, s);
		write_array(weight, s);
	}

//-----------------------------------------------------------------------------

	virtual size_t size() const { return K; }

	virtual const array <lab>&
	labels() const { return label; }

	virtual const array <array <T> >&
	centroids() const { return cen_nd; }

	virtual const array <array <lab> >&
	edges() const { return edge; }

	virtual const array <array <T> >&
	weights() const { return weight; }

	virtual void release() { label.init(); }

//-----------------------------------------------------------------------------

	virtual array <lab> quant(const data& X, const size_array& at) const
	{
		return leaf <T>::quant(X, at);
	}

	virtual array <lab> quant(const data& X, const size_array& at,
									 const T range) const
	{
		return leaf <T>::quant(X, at, range);
	}

	virtual array_2d <T> dist2(const data& X, const size_array& at) const
	{
		return leaf <T>::dist2(X, at);
	}

	virtual array <T> dist2(const data& X, const size_array& at,
									const array <lab>& c) const
	{
		return leaf <T>::dist2(X, at, c);
	}

	virtual array <array <T> > dist2(const data& X, const size_array& at,
												const array <array <lab> >& c) const
	{
		return leaf <T>::dist2(X, at, c);
	}

	virtual array_2d <T> dist2(const size_array& c) const
	{
		return outer_minus(cen[c], cen) ->* 2;
	}

//-----------------------------------------------------------------------------

	virtual void flat(data& C, const size_array& at, const array <lab>& c)
	{
		leaf <T>::flat(C, at, c);
	}

};

//-----------------------------------------------------------------------------

}  // namespace qc

#endif  // TRAIN_LEAF_HPP
