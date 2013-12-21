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

#ifndef LIB_RADIX_HPP
#define LIB_RADIX_HPP

//-----------------------------------------------------------------------------

namespace ivl {

namespace sort_details {

//-----------------------------------------------------------------------------
//! sort the source array based on b-th byte and store the result in destination array

template<typename T, typename SS, typename SD> inline
void radix_byte(size_t b, array<T, SS>& src, array<T, SD>& dest)
{
	b *= 8;
	size_t B = 256, N = src.size();

	size_array bytes = (src >> b) & 0xff;  // current byte of each element
	size_array count(B, size_t(0));  // occurrences of each element
	++count[bytes];

	if(count[0] == N)  // all bytes are zero; order remains unchanged
		{ dest = src; return; }

	size_array index = shift(cumsum(count), 1);  // index-list for each element
	size_array pos(N);  // position of each element in the destination array
	for(size_t i=0; i<N; i++) pos[i] = index[bytes[i]]++;

	dest[pos] = src;  // place elements in the destination array
}

//-----------------------------------------------------------------------------
//! sort an array of positive numbers using the radix sort algorithm

template<typename T, typename S> inline
void radix_base(array<T, S>& src)
{
	size_t N = src.size();
	size_t nb = sizeof(T) / sizeof(char);  // number of bytes used
	array<T> temp(N);  // temporary destination array

	for(size_t i=0; i<nb/2; i++) // sort based on each byte starting from the LSD
	{
		radix_byte(i*2, src, temp);
		radix_byte(i*2+1, temp, src);
	}
}

}  // namespace sort_details

//-----------------------------------------------------------------------------

struct radix_impl {
	template<typename T, typename S>
	void operator()(array<T, S>& a) { sort_details::radix_base(a); }
} radix;

//-----------------------------------------------------------------------------

}  // namespace ivl

#endif  // LIB_RADIX_HPP
