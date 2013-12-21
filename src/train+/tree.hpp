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

#ifndef TRAIN_TREE_HPP
#define TRAIN_TREE_HPP

namespace qc {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

template <typename T>
struct train_tree : public tree <T>
{
	typedef size_t pos;          // position type
	typedef unsigned int lab;    // label type
	typedef unsigned int count;  // count type

	// TODO: use indirect (eventually slice) types
	typedef array <array <T> > data;  // data type
	typedef array <array <T> > book;  // codeword type

	virtual void write(std::ostream& file) const = 0;
	virtual const array <lab>& labels() const = 0;
	virtual const array <array <T> >& centroids() const = 0;
	virtual void release() = 0;

	// TODO: remove size_array
	virtual array_2d <T> dist2(const data&, const size_array&) const = 0;
	virtual array_2d <T> dist2(const size_array& c) const = 0;
};

//-----------------------------------------------------------------------------

// defined below
template <typename T> class train_leaf;
template <typename T> class train_node;

//-----------------------------------------------------------------------------

// TODO: remove "at"
template <typename T>
train_tree <T>*
train(const array <array <T> >& X, const size_array& at,
		const train_options& opt)

{
	// TODO: remove at.empty()
	if (X.empty() || at.empty() || opt.cap.empty())
		return 0;
	if (at.length() == 1)
		// TODO: X[at[0]] -> X[0]
		return new train_leaf <T> (X[at[0]], opt);
	else
		// TODO: X, at -> X
		return new train_node <T> (X, at, opt);
}

//-----------------------------------------------------------------------------

}  // namespace qc

#endif  // TRAIN_TREE_HPP
