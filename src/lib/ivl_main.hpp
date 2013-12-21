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

#ifndef LIB_IVL_MAIN_HPP
#define LIB_IVL_MAIN_HPP

#include <set>

//-----------------------------------------------------------------------------

namespace ivl {

//-----------------------------------------------------------------------------

template <typename T>
T eps_() { return std::numeric_limits<T>::epsilon(); }

//-----------------------------------------------------------------------------

// discrete non-negative base-2 logarithm; returns (size_t) -1 if n == 0
size_t log2_(size_t n)
{
	size_t k = 0;
	while (n) { n >>= 1; k++; }
	return k - 1;
}

//-----------------------------------------------------------------------------

template <typename T>
static T div_ceil_(const T n, const T d)
{
	T q = n / d;
	return (q * d < n) ? q + 1 : q;
}

//-----------------------------------------------------------------------------

size_t pow_(size_t b, size_t e)
{
	return _[(unsigned int)b] ->* (unsigned int)e;
}

//-----------------------------------------------------------------------------

template <typename T, typename S>
size_t count_(const array <T, S>& a)
{
	return find(a).length();
}

//-----------------------------------------------------------------------------

template <typename T, typename S>
array <T> unique_(const array <T, S>& a)
{
	array <T> s(a);
	radix(s);
	return unique[ord](s);
}

//-----------------------------------------------------------------------------

template <typename T, typename S>
ret <array <T>, size_array>
unique_count_(const array <T, S>& a)
{
	array <T> s(a);
	radix(s);
	array <T> u;
	size_array p;
	(_, u, p) = unique[ord]++(s);
	p = diff(p.cat(arr(s.length())));
	return (_, u, p);
}

//-----------------------------------------------------------------------------

template <typename T, typename S>
size_array
uniq_count_(const array <T, S>& a)  // only return the count
{
	array <T> s(a);
	radix(s);
	array <T> u;
	size_array p;
	(_, u, p) = unique[ord]++(s);
	return diff(p.cat(arr(s.length())));
}

//-----------------------------------------------------------------------------

template <typename T, typename S>
size_array hist_(const array <T, S>& x, const size_t B)
{
	const T base = min(x), len = max(x) - base + eps_<T>(), bin = len / B;
	size_array h(B, size_t(0));
	h[(x - base) / bin]++;
	return h;
}

//-----------------------------------------------------------------------------

template <typename T, typename S>
array <size_array> alloc_(const array <T, S>& x, const size_t B)
{
	if (x.empty()) return array <size_array>();
	const T base = min(x), len = max(x) - base + eps_<T>(), bin = len / B;
	array <size_array> a(B);
	for (size_t n = 0, N = x.length(); n < N; n++)
		a[(x[n] - base) / bin].push_back(n);
	return a;
}

//-----------------------------------------------------------------------------

template <typename T, typename S>
ret <array <T>, size_array>
head_(const array <T, S>& x, const size_t k, const size_t B = 1000)
{
	if (x.empty()) return (_, array <T>(), size_array());
	const size_t N = x.length(), K = min(k, N);
	array <T> val(K);
	size_array off(K);
	array <size_array> a = alloc_(x, B);
	size_t len, from = 0, to = 0;
	size_range r;
	for (size_t b = 0; b < B; b++)
	{
		len = a[b].length();
		if (!len) continue;
		if (to + len > K) len = K - to;
		r = (from, _, (to += len) - 1);
		val[r] = x[off[r] = a[b][0, _, len - 1]];
		if (to == K) break;
		from = to;
	}
	return (_, val, off);
}

//-----------------------------------------------------------------------------

template <typename T>
std::set <T> to_set_(const array <T>& a)
{
	std::set <T> s;
	for (size_t n = 0; n < a.length(); n++)
		s.insert(a[n]);
	return s;
}

//-----------------------------------------------------------------------------

// string to formatted value
template <typename T>
T to_(const std::string& s)
{
	T v;
	std::istringstream is(s);
	is >> v;
	return v;
}

//-----------------------------------------------------------------------------

// formatted value to string
template <typename S, typename T>
std::string to_(const T& v)
{
	std::ostringstream os;
	os << v;
	return os.str();
}

template <>
std::string to_<std::string, bool>(const bool& v)
{
	return v ? "true" : "false";
}

//-----------------------------------------------------------------------------

template <typename E, typename I = unsigned int>
class int_
{
	union { E e; I i; } u;

public:

	int_(const E& e) { u.e = e; }
	E& operator=(const E& e) { return u.e = e; }
	operator E() const { return u.e; }
	I& operator()() { return u.i; }
};

//-----------------------------------------------------------------------------

} // namespace ivl

#endif // LIB_IVL_MAIN_HPP
