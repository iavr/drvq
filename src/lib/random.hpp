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

#ifndef LIB_RANDOM_HPP
#define LIB_RANDOM_HPP

//-----------------------------------------------------------------------------

namespace ivl {

//-----------------------------------------------------------------------------

template <typename T, typename K = size_t>
class sample_search_generator;

//-----------------------------------------------------------------------------

template <typename T>
class sample_search_generator <T, size_t>
{
	typedef sample_search_generator <T, size_t> generator;

	size_t D;
	array <T> cum;
	T total;

//-----------------------------------------------------------------------------

	size_t sample() const
	{
		return first[ord](cum >= rand(T(0), total));
	}

	template <typename S>
	array <size_t, S>& sample(array <size_t, S>& X) const
	{
		for(size_t n = 0; n < X.length(); n++)
			X[n] = sample();
		return X;
	}

//-----------------------------------------------------------------------------

	template <typename C>
	size_t repeat(const C& chosen) const
	{
		size_t x;
		do x = sample();
		while (chosen[x]);
		return x;
	}

	template <typename S, typename C>
	array <size_t, S>& repeat(array <size_t, S>& X, const C& chosen) const
	{
		for(size_t n = 0; n < X.length(); n++)
			X[n] = repeat(chosen);
		return X;
	}

//-----------------------------------------------------------------------------

	template <typename C>
	size_t replace(C& chosen) const
	{
		size_t x = repeat(chosen);
		chosen[x] = true;
		return x;
	}

	template <typename S, typename C>
	array <size_t, S>& replace(array <size_t, S>& X, C& chosen) const
	{
		for(size_t n = 0; n < X.length(); n++)
			X[n] = replace(chosen);
		return X;
	}

//-----------------------------------------------------------------------------

public:

	sample_search_generator() : D(0), total(0) { }

	template <typename S>
	sample_search_generator(const array <T, S>& distr) :
		D(distr.length()), cum(cumsum(distr)), total(cum[D - 1])
		{ }

	void clear() { cum.init(); }

//-----------------------------------------------------------------------------

	size_t operator()() const { return sample(); }

	size_array operator()(const size_t N) const
	{
		size_array X(N);
		return sample(X);
	}

	array_2d <size_t> operator()(const size_t rows, size_t cols) const
	{
		array_2d <size_t> X(rows, cols);
		return sample(X);
	}

	array_nd <size_t> operator()(const size_array& sz) const
	{
		array_nd <size_t> X(sz);
		return sample(X);
	}

//-----------------------------------------------------------------------------

// 	template <typename C>
// 	size_t operator()(const C& chosen) const { return repeat(chosen); }
//
// 	template <typename C>
// 	size_array operator()(const size_t N, const C& chosen) const
// 	{
// 		size_array X(N);
// 		return repeat(X, chosen);
// 	}
//
// 	template <typename C>
// 	array_2d <size_t> operator()(const size_t rows, size_t cols,
// 										  const C& chosen) const
// 	{
// 		array_2d <size_t> X(rows, cols);
// 		return repeat(X, chosen);
// 	}
//
// 	template <typename C>
// 	array_nd <size_t> operator()(const size_array& sz,
// 										  const C& chosen) const
// 	{
// 		array_nd <size_t> X(sz);
// 		return repeat(X, chosen);
// 	}

//-----------------------------------------------------------------------------

	template <typename C>
	size_t operator()(C& chosen) const { return replace(chosen); }

	template <typename C>
	size_array operator()(const size_t N, C& chosen) const
	{
		size_array X(N);
		return replace(X, chosen);
	}

	template <typename C>
	array_2d <size_t> operator()(const size_t rows, size_t cols,
										  C& chosen) const
	{
		array_2d <size_t> X(rows, cols);
		return replace(X, chosen);
	}

	template <typename C>
	array_nd <size_t> operator()(const size_array& sz,
										  C& chosen) const
	{
		array_nd <size_t> X(sz);
		return replace(X, chosen);
	}

};

//-----------------------------------------------------------------------------

template <typename T, typename K>
class sample_search_generator : public sample_search_generator <T, size_t>
{
	typedef sample_search_generator <T, size_t> base;

	const array <K>& key;

public:

	template <typename S1, typename S2>
	sample_search_generator(const array <K, S1>& key,
									const array <T, S2>& distr) :
		base(distr), key(key)
		{ }

/*	array <K> operator()(const size_t N) const
	{
		return key[base::operator()(N)];
	}

	template <typename S>
	array <K> operator()(const size_t N, array <bool, S>& chosen) const
	{
		return key[base::operator()(N, chosen)];
	}*/
};

//-----------------------------------------------------------------------------

template <typename T, typename S>
sample_search_generator <T>
sample_search(const array <T, S>& distr)
{
	return sample_search_generator <T> (distr);
}

template <typename T, typename K, typename S1, typename S2>
sample_search_generator <T, K>
sample_search(const array <K, S1>& key, const array <T, S2>& distr)
{
	return sample_search_generator <T, K> (key, distr);
}

//-----------------------------------------------------------------------------

} // namespace ivl

#endif // LIB_RANDOM_HPP
