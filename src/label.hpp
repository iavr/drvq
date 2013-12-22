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

#ifndef LABEL_HPP
#define LABEL_HPP

#include "io/files.hpp"
#include "options/label.hpp"
#include "data/norm.hpp"

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

template <typename T, bool DIST = true>
class labeler : public iofile <labeler <T, DIST>, true>
{
	typedef types::t_true yes;
	typedef types::t_false no;

	typedef typename tree <T>::pos pos;    // position type (label here)
	typedef typename tree <T>::data data;  // data type

	size_t F;  // total number of files
	size_t N;  // total number of points

//-----------------------------------------------------------------------------

	ret <array <pos>, array <T> >
	label(const root <T>* book, const data& X, const label_options& opt)
	{
		switch (opt.method)
		{
			case label_options::fast:   return book->quant(yes(), X);
			case label_options::approx: return book->quant(yes(), X, opt.range);
			case label_options::exact:  return book->exact(yes(), X);
		}
	}

//-----------------------------------------------------------------------------

	void update_points(size_t len)
	{
		for (size_t f = F; f < F + len; f++)
			N += labels[f].length();
	}

//-----------------------------------------------------------------------------

public:

	array <array <pos> > labels;    // point labels per file
	array <array <T> > distortion;  // point distances to labels per file

	labeler() : F(0), N(0) { }

	labeler(const root <T>* book, const label_options& opt, timer& t) : N(0)
	{
		array <string> names = load_lines(opt.list);
		msg::require(check, !names.empty(), "empty data file list");
		F = names.length();
		labels.init(F);
		distortion.init(F);
		for (size_t f = 0; f < F; f++)
		{
			opt.brief() ? msg::progress(info, f, F) :
			              msg::percent(info, f, names[f], f, F);
			data X = load_data <T>(opt, names[f]);
			normalize(X, opt);
			if (X.empty()) continue;
			N += X[0].length();
			t.tic();
			(_, labels[f], distortion[f]) = label(book, X, opt);
			t.tac();
		}
	}

//-----------------------------------------------------------------------------

	size_t files()  const { return F; }
	size_t points() const { return N; }

//-----------------------------------------------------------------------------

	void add(const string& filename)
	{
		size_t alloc = labels.length() + load_array_size <pos>(filename);
		labels.init(alloc);
		distortion.init(alloc);
	}

	void read(std::istream& s)
	{
		char c;
		msg::dot(info);
		array <array <pos> > lab = read_array <array <pos> >()(s);
		size_t len = lab.length();
		labels[F, _, F + len - 1] = lab;
		if (s.get(c))  // TODO: more elegant way to check if more data are available?
		{
			s.putback(c);
			distortion[F, _, F + len - 1] = read_array <array <T> >()(s);
		}
		update_points(len);
		F += len;
	}

//-----------------------------------------------------------------------------

	labeler(std::istream& s) :
		F(0), N(0), labels(read_array <array <pos> >()(s))
		{
			if (s) distortion = read_array <array <T> >()(s);
			update_points(labels.length());
			F = labels.length();
		}

//-----------------------------------------------------------------------------

	void write(std::ostream& s) const
	{
		write_array(labels, s);
		write_array(distortion, s);
	}

};

//-----------------------------------------------------------------------------

template <typename T>
class labeler <T, false> : public iofile <labeler <T, false>, true>
{
	typedef types::t_true yes;
	typedef types::t_false no;

	typedef typename tree <T>::pos pos;    // position type (label here)
	typedef typename tree <T>::data data;  // data type

	size_t F;  // total number of files
	size_t N;  // total number of points

//-----------------------------------------------------------------------------

	array <pos>
	label(const root <T>* book, const data& X, const label_options& opt)
	{
		switch (opt.method)
		{
			case label_options::fast:   return book->quant(no(), X);
			case label_options::approx: return book->quant(no(), X, opt.range);
			case label_options::exact:  return book->exact(no(), X);
		}
	}

//-----------------------------------------------------------------------------

	void update_points(size_t len)
	{
		for (size_t f = F; f < F + len; f++)
			N += labels[f].length();
	}

//-----------------------------------------------------------------------------

public:

	array <array <pos> > labels;    // point labels per file

	labeler() : F(0), N(0) { }

	labeler(const root <T>* book, const label_options& opt, timer& t) : N(0)
	{
		array <string> names = load_lines(opt.list);
		msg::require(check, !names.empty(), "empty data file list");
		F = names.length();
		labels.init(F);
		for (size_t f = 0; f < F; f++)
		{
			opt.brief() ? msg::progress(info, f, F) :
			              msg::percent(info, f, names[f], f, F);
			data X = load_data <T>(opt, names[f]);
			normalize(X, opt);
			if (X.empty()) continue;
			N += X[0].length();
			t.tic();
			labels[f] = label(book, X, opt);
			t.tac();
		}
	}

//-----------------------------------------------------------------------------

	size_t files()  const { return F; }
	size_t points() const { return N; }

//-----------------------------------------------------------------------------

	void add(const string& filename)
	{
		labels.init(labels.length() + load_array_size <pos>(filename));
	}

	void read(std::istream& s)
	{
		msg::progress(info, F, labels.length());
		array <array <pos> > lab = read_array <array <pos> >()(s);
		size_t len = lab.length();
		labels[F, _, F + len - 1] = lab;
		update_points(len);
		F += len;
	}

//-----------------------------------------------------------------------------

	labeler(std::istream& s) :
		F(0), N(0), labels(read_array <array <pos> >()(s))
		{
			update_points(labels.length());
			F = labels.length();
		}

//-----------------------------------------------------------------------------

	void write(std::ostream& s) const
	{
		write_array(labels, s);
	}

};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // LABEL_HPP
