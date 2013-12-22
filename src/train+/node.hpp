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

#ifndef TRAIN_NODE_HPP
#define TRAIN_NODE_HPP

#include "prop.hpp"

namespace drvq {

using namespace ivl;
using namespace std;
using ivl::min;
using ivl::max;
using ivl::find;
using ivl::rand;

//-----------------------------------------------------------------------------

template <typename T>
class train_node : public node <T>, public train_tree <T>
{
	typedef typename tree <T>::lab lab;                 // label type
	typedef typename tree <T>::pos pos;                 // position type
	typedef typename tree <T>::data data;               // data type
	typedef typename train_tree <T>::count count;       // count type
	typedef typename train_tree <T>::book book;         // codeword type
	typedef sample_search_generator <count> generator;  // random generator type

	const size_t D, D0, D1, L;        // dimensions, child dimensions, level
	using node <T>::K;                // number of centroids
	using node <T>::J;                // number of children centroids
	using node <T>::dim0;             // left child dimension ranges
	using node <T>::dim1;             // right child dimension ranges
	train_tree <T> *child0, *child1;  // children
	array <array <T> > cen;           // centroids (vectors)
	using node <T>::code0;            // positions of left centroids on grid
	using node <T>::code1;            // positions of right centroids on grid
	using node <T>::source;           // centroid label per bin
	using node <T>::edge;             // edges between neighboring centroids
	using node <T>::weight;           // edge weights
	array <lab> label;                // centroid label per data point

	// temporary
	array_2d <count> b_pop;           // population per bin
	array_2d <T> distance;            // distance from centroid on grid
	generator gen;                    // random sample generator
	array_2d <char> chosen;           // is each bin chosen as a sample?  // TODO: char -> bool

//-----------------------------------------------------------------------------

	void train
	(
		const data& X,          // data points
		const size_array& at,   // dimension range
		const train_options& opt  // training options
	)
	{
		// trivial cases
		if(K < 2) return;

		// recurse
		// TODO: X, at[dim_] -> X[dim_]
		child0 = drvq::train(X, at[dim0], opt);
		child1 = drvq::train(X, at[dim1], opt);
		node <T>::child0 = static_cast <tree <T>*> (child0);
		node <T>::child1 = static_cast <tree <T>*> (child1);

		// children centroids
		const array <array <T> > &cen0 = child0 -> centroids(),
		                         &cen1 = child1 -> centroids();

		bool detail = J >= 512;                // detailed messages
		msg::level(info, L, at, K, J, detail);
		msg::nl(info, detail);

		// initialize
		if (detail) msg::in_line(info, "initializing");
		const size_t B = J * J;                // number of bins on grid
		const size_array grid = idx(J, J);     // grid dimensions
		size_array zero, nz;                   // empty and non-empty bins

		// quantize data points into bins
		array <count> pop(K, 1);               // population per centroid
		b_pop.init(grid, count(0));            // population per bin
		array <pos> code =                     // nearest bin per data point
			(child0 -> labels()) + J * (child1 -> labels());
		b_pop[code]++;
		size_array dom = find(b_pop);          // domain to visit

		// initialize centroids
		cen.init(D);                           // centroids (vectors)
		for (size_t d = 0; d < D; d++)
			cen[d].init(K, T());
		gen = sample_search(b_pop);            // generator on bin distribution
		chosen.init(grid, false);              // is each bin chosen as a sample?
		sample();

		// initialize assignments
		assign(true, types::t_false(), opt, detail);
		array_2d <lab> prev = source;          // bin labels @ previous iteration

		for (size_t it = 0; ; it++)
		{
			if (detail) msg::iter(info, it);

			// update populations
			pop[_] = 0;
			pop[source[dom]] += b_pop[dom];
			(_, zero, nz) = split++(pop == count(0));

			// update centroids
			for (size_t d = 0; d < D; d++)
			{
				cen[d][nz] = T();
				if (d < D0) cen[d][source[dom]] += b_pop[dom] * cen0[d]     [dom % J];
				else        cen[d][source[dom]] += b_pop[dom] * cen1[d - D0][dom / J];
			}
			for (size_t d = 0; d < D; d++)
				cen[d][nz] /= pop[nz];

			// re-sample empty centroids
			sample(zero);

			// update assignments
			assign(false, types::t_false(), opt, detail);

			// OPTIONAL: total distortion (sum of square residuals)
			// T distortion = sum(b_pop * distance);

			// terminate?
			double ratio = opt.theta * _[it * .01] ->* 2;
			if (find((source != prev)[dom]).length() < dom.length() * ratio)
			{
				if (!detail) msg::term(info, it);
				break;
			}
			prev = source;
		}

		if (detail) msg::in_line(info, "terminating");

		// re-sample empty centroids
		sample(zero);

		// re-assign, generating edges, and check for empty
		assign(true, types::t_true(), opt, detail);
		msg::zero(check, K, source[dom], b_pop[dom]);
		if (detail) msg::edge(check, weight, opt.range);

		// store data labels, release children
		label = source[code];
		child0 -> release();
		child1 -> release();

		// clear temporaries
		b_pop.init(idx(0, 0));     // TODO: init()
		distance.init(idx(0, 0));  // TODO: init()
		gen.clear();
		chosen.init(idx(0, 0));    // TODO: init()
	}

//-----------------------------------------------------------------------------

	void sample()
	{
		array <pos> samp = gen(K, chosen);  // samples with replacement
		code0 = samp % J;                   // positions of centroids on grid
		code1 = samp / J;
	}

	void sample(const size_array& which)
	{
		size_t k = which.length();
		array <pos> samp = gen(k, chosen);  // samples with replacement
		code0[which] = samp % J;            // positions of centroids on grid
		code1[which] = samp / J;
	}

//-----------------------------------------------------------------------------

	template <typename TERM>
	void assign(bool quantized, const TERM term,
					const train_options& opt, bool detail)
	{
		if (opt.hash)
			assign(quantized, term, types::t_true(), opt, detail);
		else
			assign(quantized, term, types::t_false(), opt, detail);
	}

	template <typename TERM, typename HASH>
	void assign(bool quantized, TERM term, HASH,
					const train_options& opt, bool detail)
	{
		// find distances between centroids and child centroids
		array_2d <T> dist0 = quantized ? child0 -> dist2(code0) :
		                                 child0 -> dist2(cen, dim0);
		array_2d <T> dist1 = quantized ? child1 -> dist2(code1) :
		                                 child1 -> dist2(cen, dim1);

		// quantize centroids
		if (!quantized)
		{
			for (size_t k = 0; k < K; k++)
			{
				code0[k] = arg_min(dist0.as_rows()[k]);  // TODO: code0 = arg_min[1](dist0);
				code1[k] = arg_min(dist1.as_rows()[k]);
			}
			chosen[code0 + J * code1] = true;
		}

		// get child edges
		const array <array <lab> > &edge0 = child0 -> edges(),
		                           &edge1 = child1 -> edges();
		const array <array <T> > &weight0 = child0 -> weights(),
		                         &weight1 = child1 -> weights();

		// propagate
		prop <T, lab, pos, count, HASH>
			P(source, distance, edge, weight,
		     code0, code1, dist0, dist1,
		     edge0, edge1, weight0, weight1,
		     b_pop, opt);

		P(gen, chosen, term, detail);
	}

//-----------------------------------------------------------------------------

public:

	train_node
	(
		const data& X,            // data points
		const size_array& at,     // dimension range
		const train_options& opt  // training options
	) :
		node <T>
		(
			// TODO: at -> X
			opt.cap[min(opt.cap.length() - 1, log2_(at.length()))],      // K
			opt.cap[min(opt.cap.length() - 1, log2_(at.length()) - 1)],  // J
			(0, _, at.length() / 2 - 1),                                 // dim0
			(at.length() / 2, _, at.length() - 1)                        // dim1
		),
		// TODO: at -> X
		D(at.length()), D0(D / 2), D1(D - D0), L(log2_(D))
	{
		// TODO: X, at -> X
		this->train(X, at, opt);
	}

//-----------------------------------------------------------------------------

	virtual void write(std::ostream& s) const
	{
		char type = 1;
		ivl::write(type, s);
		ivl::write(K, s);
		ivl::write(J, s);
		write_array(dim0, s);
		write_array(dim1, s);
		write_array(code0, s);
		write_array(code1, s);
		write_array_2d(source, s);
		write_array(edge, s);
		write_array(weight, s);

		child0 -> write(s);
		child1 -> write(s);
	}

//-----------------------------------------------------------------------------

	virtual size_t size() const { return K; }

	virtual const array <lab>&
	labels() const { return label; }

	virtual const array <array <T> >&
	centroids() const { return cen; }

	virtual const array <array <lab> >&
	edges() const { return edge; }

	virtual const array <array <T> >&
	weights() const { return weight; }

	virtual void release() { cen.init(); label.init(); }

//-----------------------------------------------------------------------------

	virtual array <lab> quant(const data& X, const size_array& at) const
	{
		return node <T>::quant(X, at);
	}

	virtual array <lab> quant(const data& X, const size_array& at,
									  const T range) const
	{
		return node <T>::quant(X, at, range);
	}

	virtual array_2d <T> dist2(const data& X, const size_array& at) const
	{
		return node <T>::dist2(X, at);
	}

	virtual array <T> dist2(const data& X, const size_array& at,
									const array <lab>& c) const
	{
		return node <T>::dist2(X, at, c);
	}

	virtual array <array <T> > dist2(const data& X, const size_array& at,
												const array <array <lab> >& c) const
	{
		return node <T>::dist2(X, at, c);
	}

	virtual array_2d <T> dist2(const size_array& c) const
	{
		return (child0 -> dist2(code0[c])) (_, cast <size_t> (code0)) +
		       (child1 -> dist2(code1[c])) (_, cast <size_t> (code1));
	}

//-----------------------------------------------------------------------------

	virtual void flat(data& C, const size_array& at, const array <lab>& c)
	{
		node <T>::flat(C, at, c);
	}

};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // TRAIN_NODE_HPP
