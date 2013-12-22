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

#ifndef SEARCH_TREE_HPP
#define SEARCH_TREE_HPP

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

template <typename T>
struct tree
{
	typedef size_t pos;        // position type
	typedef unsigned int lab;  // label type

	// TODO: use indirect (eventually slice) types
	typedef array <array <T> > data;  // data type

	virtual size_t size() const = 0;
	virtual const array <array <lab> >& edges() const = 0;
	virtual const array <array <T> >& weights() const = 0;

	// TODO: remove size_array
	virtual array <lab> quant(const data&, const size_array&) const = 0;
	virtual array <lab> quant(const data&, const size_array&, const T) const = 0;

	virtual array_2d <T> dist2(const data&, const size_array&) const = 0;

	virtual array <T> dist2(const data&, const size_array&,
									const array <lab>&) const = 0;

	virtual array <array <T> > dist2(const data&, const size_array&,
												const array <array <lab> >&) const = 0;

	virtual void flat(data& C, const size_array& at, const array <lab>& c) = 0;
};

//-----------------------------------------------------------------------------

// defined below
template <typename T> class leaf;
template <typename T> class node;

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // SEARCH_TREE_HPP
