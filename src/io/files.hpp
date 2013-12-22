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

#ifndef IO_FILES_HPP
#define IO_FILES_HPP

namespace drvq {

using namespace ivl;
using namespace std;

using ivl::sqrt;

//-----------------------------------------------------------------------------

template <typename T>
size_array
view_sizes(const string& path, const array <string>& names, const string& ext)
{
	size_array sz(names.size());

	for (size_t f = 0; f < names.size(); f++)
		sz[f] = load_array_size <T> (path + '/' + names[f] + '.' + ext);

	return sz;
}

//-----------------------------------------------------------------------------

template <typename T>
array <array <T> >
load_view(const string& path, const string& name, const string& ext)
{
	typedef array <T> point;
	return load_array <point> (path + '/' + name + '.' + ext);
}

//-----------------------------------------------------------------------------

template <typename T>
array <array <T> >
load_feat(const string& path, const string& name, const string& ext)
{
	typedef array <T> point;
	array <point> x = load_array <point> (path + '/' + name + '.' + ext);
	for (size_t k = 0; k < x.length(); k++)
	{
		if (x[k].length() < 5) x[k].init(5, T());
		swap(x[k][2], x[k][3]);
		swap(x[k][3], x[k][4]);
		x[k][3] = T();
	}
	return x;
}

//-----------------------------------------------------------------------------

template <typename T>
array <array <array <T> > >
load_feats(const string& path, const array <string>& names, const string& ext)
{
	typedef array <T> point;
	size_t F = names.length();
	array <array <point> > X(F);
	for (size_t f = 0; f < F; f++)
	{
		msg::progress(info, f, F);
		X[f] = load_feat <T> (path, names[f], ext);
	}
	return X;
}

//-----------------------------------------------------------------------------

template <typename T>
void save_data(array <array <T> >& X, const string& name)
{
	typedef array <T> point;
	typedef array <point> view;
	typedef array <array <T> > data;

	size_t D = X.length(), N = X[0].length();

	view v(N);
	for (size_t n = 0; n < N; n++)
		v[n].init(D);

	for (size_t n = 0; n < N; n++)
		for (size_t d = 0; d < D; d++)
			v[n][d] = X[d][n];

	save_array(v, name);
}

//-----------------------------------------------------------------------------

template <typename T>
array <array <T> >
load_data(const string& path, const string& name, const string& ext)
{
	typedef array <T> point;
	typedef array <point> view;
	typedef array <array <T> > data;

	view v = load_array <point> (path + '/' + name + '.' + ext);
	if (v.empty()) return array <array <T> >();
	size_t N = v.length(), D = v[0].length();

	data X(D);
	for (size_t d = 0; d < D; d++)
		X[d].init(N);

	for (size_t n = 0; n < N; n++)
		for (size_t d = 0; d < D; d++)
			X[d][n] = v[n][d];

	return X;
}

//-----------------------------------------------------------------------------

template <typename T>
array <array <T> >
load_data(const string& path, const array <string>& names, const string& ext)
{
	typedef array <T> point;
	typedef array <point> view;
	typedef array <array <T> > data;

	size_array sz_dim = load_array_size <point> (path + '/' + names[0] + '.' + ext);
	if (sz_dim.empty()) return array <array <T> >();

	size_t D = sz_dim[1];
	size_array sz = view_sizes <T>(path, names, ext);
	size_t n = 0, N = sum(sz);

	view v;
	data X(D);
	for (size_t d = 0; d < D; d++)
		X[d].init(N);

	size_t F = names.size();
	for (size_t f = 0; f < F; f++)
	{
		msg::progress(info, f, F);
		v = load_array <point> (path + '/' + names[f] + '.' + ext);
		if (v.empty()) return X;

		for (size_t k = 0; k < v.size(); k++, n++)
			for (size_t d = 0; d < D; d++)
				X[d][n] = v[k][d];
	}

	return X;
}

//-----------------------------------------------------------------------------

template <typename T, typename O>
array <array <T> >
load_data(const O& opt)
{
	array <string> names = load_lines(opt.list);
	if (names.empty() || !opt.files)
		return array <array <T> >();
	if (opt.files > 0 && names.length() > opt.files)
		names = names[0, _, opt.files - 1];
	return load_data <T>(opt.path, names, opt.ext);
}

template <typename T, typename O>
array <array <T> >
load_data(const O& opt, const string& name)
{
	return load_data <T>(opt.path, name, opt.ext);
}

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // IO_FILES_HPP
