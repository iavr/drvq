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

#ifndef TRAIN_QUEUE_HPP
#define TRAIN_QUEUE_HPP

namespace qc {

using namespace ivl;
using namespace std;

using ivl::find;
using ivl::max;

//-----------------------------------------------------------------------------

template <
	typename D, typename T,
	typename REF,
	bool HASH = false,
	bool IS_FIBO = false
>
class queue
{
protected:

	typedef typename types::t_expr <IS_FIBO>::type FIBO;

	struct job
	{
		D data;
		T time;
		job() { }
		job(const D& d, const T& t) : data(d), time(t) { }
	};

	struct compare
	{
		bool operator()(const job& j1, const job& j2)
		{
			return types::r_if <FIBO> (j1.time < j2.time, j1.time > j2.time);
		}
	};

//-----------------------------------------------------------------------------

	struct SWAP
	{
		REF ref_def;
		REF& ref;

		SWAP() : ref(ref_def) { }
		SWAP(REF& r) : ref(r) { }

		void operator()(job& j1, job& j2)
		{
			std::swap(j1, j2);
			std::swap(ref[j1.data], ref[j2.data]);
		}
	};

	template <typename H, typename S>
	void heap_swap(H& h, S& s, types::t_true) { }

	template <typename H, typename S>
	void heap_swap(H& h, S& s, types::t_false) { h.set_swap(s); }

	typedef typename types::t_if <
		FIBO,
		fibonacci_heap <job, compare>,
		binary_heap <job, array <job>, compare, SWAP>
	>::type HEAP;

//-----------------------------------------------------------------------------

	SWAP swap;
	HEAP heap;

//-----------------------------------------------------------------------------

public:

	queue() { }

	queue(REF& r) : swap(r) { heap_swap(heap, swap, FIBO()); }

	void init(T, size_t) { }  // only for hash queue

//-----------------------------------------------------------------------------

	bool empty() { return heap.empty(); }

	size_t size() { return heap.size(); }

	size_t length() { return heap.size(); }

	size_t level() { return 0; }                // only for hash queue

	size_t heap_size() { return heap.size(); }  // only for hash queue

	D pop() { return heap.pop().data; }

	size_t up(const size_t p, const D& data, const T& time)
	{
		return heap.up(p, job(data, time));
	}

	size_t push(const D& data, const T& time)
	{
		return heap.push(job(data, time));
	}

	size_t seek(const T& time)
	{
		return heap.size();
	}
};

//-----------------------------------------------------------------------------

template <
	typename D, typename T,
	typename REF, bool IS_FIBO
>
class queue <D, T, REF, true, IS_FIBO> :
	public queue <D, T, REF, false, IS_FIBO>
{
	typedef queue <D, T, REF, false, IS_FIBO> base;
	typedef typename base::job job;
	using base::heap;
	using base::swap;

//-----------------------------------------------------------------------------

	class entry
	{
		array <job> a;
		array <bool> in;
		size_t entry_size;

	public:

		entry() : entry_size(0) { }

		void init() { a.init(); in.init(); entry_size = 0; }

		bool empty() { return entry_size == 0; }

		size_t size() { return entry_size; }

		size_t add(const job& j)
		{
			a.push_back(j);
			in.push_back(true);
			entry_size++;
			return a.length() - 1;
		}

		size_t replace(const size_t p, const job& j) { a[p] = j; return p; }

		void remove(const size_t p) { in[p] = false; entry_size--; }

		size_array pos() { return find(in); }

		array <job> yield() { return a[in]; }

	};

//-----------------------------------------------------------------------------

	T bucket;            // hash interval
	size_t h, H;         // current and maximum hash entry
	array <entry> hash;  // hash table
	size_t queue_size;   // total queue length

//-----------------------------------------------------------------------------

	void scan()
	{
		if (!heap.empty())
			return;

		do h++;
		while (h < H && hash[h].empty());
		if (h == H)
			return;

		size_array p = hash[h].pos();
		array <job> a = hash[h].yield();
		for (size_t i = 0; i < p.length(); i++)
			swap.ref[a[i].data] = h + H * i;
		heap.init(a);
	}

//-----------------------------------------------------------------------------

public:

	queue() { init(); }

	queue(REF& r) : base(r) { init(); }

	void init(T bucket = 1, size_t H = 1)
	{
		this->bucket = bucket;
		hash.init(this->H = H);
		h = 0;
		queue_size = 0;
	}

//-----------------------------------------------------------------------------

	bool empty() { return queue_size == 0; }

	size_t size() { return queue_size; }

	size_t length() { return queue_size; }

	size_t level() { return h; }

	size_t heap_size() { return heap.size(); }

	D pop()
	{
		scan();
		CHECK(h < H, erange());
		queue_size--;
		return heap.pop().data;
	}

	size_t up(const size_t p, const D& data, const T& time)
	{
		job j(data, time);
		size_t b = max(h, time / bucket), q = p / H, r = p % H;
		if (b == r)
			return b + H * (b == h ? heap.up(q, j) : hash[r].replace(q, j));
		else
		{
			CHECK(r != h, ecomp());
			hash[r].remove(q);
			return b + H * (b == h ? heap.push(j) : hash[b].add(j));
		}
	}

	size_t push(const D& data, const T& time)
	{
		queue_size++;
		job j(data, time);
		size_t b = max(h, time / bucket);
		return b + H * (b == h ? heap.push(j) : hash[b].add(j));
	}

	size_t seek(const T& time)
	{
		size_t b = time / bucket;
		return b + H * (b == h ? heap.size() : hash[b].size());
	}
};

//-----------------------------------------------------------------------------

}  // namespace qc

#endif // TRAIN_QUEUE_HPP
