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

#ifndef TRAIN_PROP_HPP
#define TRAIN_PROP_HPP

#include "queue.hpp"

namespace drvq {

using namespace ivl;
using namespace std;

using ivl::max;
using ivl::min;
using ivl::sort;

//-----------------------------------------------------------------------------

template <
	typename T, typename lab, typename pos, typename count,
	typename HASH = types::t_false
>
class prop
{
//-----------------------------------------------------------------------------

	typedef array_2d <pos> REF;
	typedef queue <pos, T, REF, HASH::value> QUEUE;
	typedef sample_search_generator <count> generator;

	// output
	array_2d <lab>& source;      // nearest centroid label on grid
	array_2d <T>& distance;      // distance from centroid on grid
	array <array <lab> >& edge;  // edges between neighboring centroids
	array <array <T> >& weight;  // edge weights

	// input (or i/o)
	const size_t K;                     // number of centroids
	const size_t J;                     // number of children centroids
	array <lab> & code0;                // centroids quantized in child0
	array <lab> & code1;                // centroids quantized in child1
	const array_2d <T>& dist0;          // centroid distances to child0 centroids
	const array_2d <T>& dist1;          // centroid distances to child0 centroids
	const array <array <lab> >& edge0;  // child0 edges
	const array <array <lab> >& edge1;  // child1 edges
	const array <array <T> >& weight0;  // child0 edge weights
	const array <array <T> >& weight1;  // child1 edge weights
	const array_2d <count>& target;     // target domain to visit

	// options
	const train_options& opt;

	// temporary
	array_2d <pos> order;      // position in queue
	array_2d <pos>& state;     // alias of position as state
	QUEUE front;               // propagating front
	array_2d <char> adjacent;  // centroid adjacency matrix  // TODO: char -> bool
	array <T> far;             // mean distance per centroid

	// state
	const pos alive, burnt;

public:

//-----------------------------------------------------------------------------

	prop
	(
		// output
		array_2d <lab>& source,
		array_2d <T>& distance,
		array <array <lab> >& edge,
		array <array <T> >& weight,

		// input (or i/o)
		array <lab>& code0,
		array <lab>& code1,
		const array_2d <T>& dist0,
		const array_2d <T>& dist1,
		const array <array <lab> >& edge0,
		const array <array <lab> >& edge1,
		const array <array <T> >& weight0,
		const array <array <T> >& weight1,
		const array_2d <count>& target,

		// options
		const train_options& opt
	) :

		// output
		source(source), distance(distance), edge(edge), weight(weight),

		// input
		K(dist0.rows()), J(dist0.columns()),
		code0(code0), code1(code1),
		dist0(dist0), dist1(dist1),
		edge0(edge0), edge1(edge1),
		weight0(weight0), weight1(weight1),
		target(target),

		// options
		opt(opt),

		// temporary
		state(order), front(order),
		alive(-1), burnt(-2)
		{ }

//-----------------------------------------------------------------------------

	template <typename TERM>
	void operator()
	(
		const generator& gen,     // random sample generator
		array_2d <char>& chosen,  // is each bin chosen as a sample?  // TODO: char -> bool
		const TERM term,          // terminate (compute edges)
		const bool detail         // detailed messages
	)
	{
		// initialize edges
		if (term)
		{
			adjacent.init(idx(K, K), false);  // TODO: init(K, K, false)
			for (size_t k = 0; k < K; k++)  // loops always first
			{
				edge[k].push_back(k);
				weight[k].push_back(0);
				adjacent(k, k) = true;
			}
		}

		// initialize grid
		const size_t B = J * J;             // number of bins on grid
		const size_array grid = idx(J, J);  // grid size
		distance.init(grid, T());           // distance on grid  // TODO: remove T()
		state.init(grid, alive);            // state on grid
		size_t total =
			find(target).length();           // total number of bins to visit
		size_t queue_length = 0;            // maximum queue length (info only)
		size_t heap_length = 0;             // maximum heap length (info only)
		pos p, n;                           // current point; neighbor point
		T dist;                             // current distance
		lab src;                            // p's source (nearest centroid)
		lab c0, c1;                         // p's coordinates

		// mean distances per centroid
		array <T> far0(K), far1(K);
		for (size_t k = 0; k < K; k++)
		{
			far0[k] = mean(dist0.as_rows()[k]);  // TODO: array <T> _far0 = mean[1](dist0);
			far1[k] = mean(dist1.as_rows()[k]);  // TODO: array <T> _far1 = mean[1](dist1);
		}
		far = far0 + far1;

		// initialize hash queue
		init_queue(B, HASH());

		// initiate front, re-sampling when sources coincide
		for (size_t k = 0; k < K; k++)
		{
			p = offset(code0[k], code1[k]);
			if (state[p] != alive)
				(_, code0[k], code1[k]) = coord(p = gen(chosen));
			msg::collide(check, state, p, alive, k);
			push(p, 0, k);
			// push(p, dist0(k, code0[k]) + dist1(k, code1[k]), k);
		}

		for (size_t visited = 0, b = 0; !front.empty(); b++)
		{
			if(detail)
			{
				msg::max_update(info, queue_length, front.length());
				if (HASH::value && front.level() > 0)
					msg::max_update(info, heap_length, front.heap_size());
				msg::progress(info, b, B);
			}

			// select nearest point
			p = front.pop();
			state[p] = burnt;
			src = source[p];
			(_, c0, c1) = coord(p);

			// propagate to c0's neighbors
			const array <lab>& e0 = edge0[c0];
			const array <T>& w0 = weight0[c0];
			for (size_t i = 1; i < e0.length(); i++)  // skip self (loop)
			{
				if (w0[i] >= opt.range) continue;
				(_, n, dist) = span(e0[i], c1, src);
				move(n, dist, src, term);
			}

			// propagate to c1's neighbors
			const array <lab>& e1 = edge1[c1];
			const array <T>& w1 = weight1[c1];
			for (size_t i = 1; i < e1.length(); i++)  // skip self (loop)
			{
				if (w1[i] >= opt.range) continue;
				(_, n, dist) = span(c0, e1[i], src);
				move(n, dist, src, term);
			}

			// terminate if all target bins are visited
			if (term) continue;
			if (target[p]) visited++;
			if (visited == total) break;
		}

		if(detail)
		{
			msg::queue(info, queue_length, heap_length);
			msg::visit(check, state, burnt, target, term);
		}
	}

//-----------------------------------------------------------------------------

	// coordinates to linear offset
	pos offset(const lab row, const lab col) { return row + J * col; }

	// linear offset to coordinates
	ret <lab, lab>
	coord(const pos off) { return (_, off % J, off / J); }

	// coordinates to linear offset and distance
	ret <pos, T>
	span(const lab c0, const lab c1, const lab src)
	{
		return (_, offset(c0, c1), dist0(src, c0) + dist1(src, c1));
	}

//-----------------------------------------------------------------------------

	template <typename TERM>
	void move(const pos p, const T dist, const lab src, TERM term)
	{
		if (state[p] == burnt)
			join(p, dist, src, term);
		else if (state[p] == alive)
			push(p, dist, src);
		else
			relax(p, dist, src);
	}

//-----------------------------------------------------------------------------

	void push(const pos p, const T dist, const lab src)
	{
		order[p] = front.seek(dist);
		order[p] = front.push(p, distance[p] = dist);
		source[p] = src;
	}

	void relax(const pos p, const T dist, const lab src)
	{
		if (dist < distance[p])
		{
			order[p] = front.up(order[p], p, distance[p] = dist);
			source[p] = src;
		}
	}

//-----------------------------------------------------------------------------

	void join(const pos p, const T dist, const lab src,
				 types::t_false) { }

	void join(const pos p, const T dist, const lab src,
				 types::t_true)
	{
		const lab from = src, to = source[p];
		if (adjacent(from, to)) return;

		const T w = (dist + distance[p]) / max(far[from], far[to]);
		if (w >= 1.5 * opt.range) return;

		adjacent(from, to) = adjacent(to, from) = true;
		edge[from].push_back(to);
		edge[to].push_back(from);
		weight[from].push_back(w);
		weight[to].push_back(w);
	}

//-----------------------------------------------------------------------------

	void init_queue(size_t B, types::t_false) { }

	void init_queue(size_t B, types::t_true)
	{
		array <T> bound0(K), bound1(K);
		for (size_t k = 0; k < K; k++)
		{
			bound0[k] = max(dist0.as_rows()[k]);
			bound1[k] = max(dist1.as_rows()[k]);
		}
		T bound = max(bound0 + bound1) + eps_<T>();
		size_t H = B / opt.bucket;
		front.init(bound / H, H);
	}

};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // TRAIN_PROP_HPP
