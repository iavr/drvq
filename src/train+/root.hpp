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

#ifndef TRAIN_ROOT_HPP
#define TRAIN_ROOT_HPP

namespace drvq {

using namespace ivl;
using namespace std;
using ivl::partition;

//-----------------------------------------------------------------------------

template <typename T>
class train_root : public root <T>, public ofile <train_root <T> >
{
	typedef typename tree <T>::data data;  // data type

	using root <T>::C;      // number of children
	using root <T>::J;      // number of children centroids
	using root <T>::dim;    // child dimension ranges
	using root <T>::child;  // children

//-----------------------------------------------------------------------------

	static array <size_array> split(const size_t D, const size_t C)
	{
		const size_t Z = 0, S = div_ceil_(D, C);
		size_array all = (Z, _, D - 1), pos = (Z, _, S, _, D - 1);
		return partition(all, pos);  // TODO: remove size_array copy
	}

	static size_t capacity(const size_t D, const train_options& opt)
	{
		return opt.cap[min(opt.cap.length() - 1, log2_(div_ceil_(D, opt.books)))];
	}

//-----------------------------------------------------------------------------

public:

	train_root
	(
		const data& X,             // data points
		const train_options& opt   // training options
	) :
		root <T>(
			opt.books,                    // C
			capacity(X.length(), opt),    // J
			split(X.length(), opt.books)  // dim
		)
	{
		for (size_t c = 0; c < C; c++)
		{
			msg::head(info, "training codebook ", c);
			child[c] = train(X, dim[c], opt);
			msg::nl(info);
		}
	}

//-----------------------------------------------------------------------------

	void write(std::ostream& s) const
	{
		ivl::write(C, s);
		ivl::write(J, s);
		write_array(dim, s);
		for (size_t c = 0; c < C; c++)
			static_cast <train_tree <T>*> (child[c]) -> write(s);
	}

};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // TRAIN_ROOT_HPP
