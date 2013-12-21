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

#ifndef LIB_IVL_IO_HPP
#define LIB_IVL_IO_HPP

namespace ivl {

//-----------------------------------------------------------------------------

namespace load_details {

//-----------------------------------------------------------------------------

template <typename T>
void read(T& x, std::istream& s, std::streamsize elements = 1)
{
	s.read(reinterpret_cast <char*>(&x), elements * sizeof(T));
}

template <typename T>
void write(const T& x, std::ostream& s, std::streamsize elements = 1)
{
	s.write(reinterpret_cast<const char*>(&x), elements * sizeof(T));
}

//-----------------------------------------------------------------------------

void read(array <bool>& a, std::istream& s)
{
	for (size_t n = 0; n < a.length(); n++)  // TODO: FIX!
	{
		bool b = a[n];
		read(b, s);
		a[n] = b;
	}
}

//-----------------------------------------------------------------------------

template <typename T>
void read(array <T>& a, std::istream& s)
{
	read(a[0], s, a.length());
}

template <typename T>
void write(const array <T>& a, std::ostream& s)
{
	write(a[0], s, a.length());
}

//-----------------------------------------------------------------------------

template <typename T>
void read(array_2d <T>& a, std::istream& s)
{
	read(a[0], s, a.length());
}

template <typename T>
void write(const array_2d <T>& a, std::ostream& s)
{
	write(a[0], s, a.length());
}

//-----------------------------------------------------------------------------

template <typename T>
T read(std::istream& s)
{
	T x;
	read(x, s);
	return x;
}

template <typename T>
array <T> read(std::istream& s, std::streamsize elements)
{
	array <T> a(elements);
	read(a, s);
	return a;
}

//-----------------------------------------------------------------------------

size_t read_size(std::istream& s)
{
	return read <double>(s);
}

void write_size(const size_t n, std::ostream& s)
{
	double sz = n;
	write(sz, s);
}

//-----------------------------------------------------------------------------
// read size of array from binary file

template <class T>
struct read_array_size
{
	typedef size_t return_type;
	size_t operator()(std::istream& s) { return read_size(s); }
};

//-----------------------------------------------------------------------------
// read size of array of arrays from binary file

template <class T>
struct read_array_size <array <T> >
{
	typedef size_array return_type;
	size_array operator()(std::istream& s)
	{
		size_t arr = read_size(s);            // number of arrays
		size_t dim = arr ? read_size(s) : 0;  // size of each array
		return ivl::arr(arr, dim);
	}
};

//-----------------------------------------------------------------------------
// read array from binary file

template <class T>
struct read_array
{
	typedef array <T> return_type;

	array <T>
	operator()(std::istream& s)
	{
		array <T> a(read_size(s));
		read(a, s);
		return a;
	}
};

//-----------------------------------------------------------------------------
// read array of arrays of equal size from binary file

template <class T>
struct read_array <array <T> >
{
	typedef array <array <T> > return_type;

	array <array <T> >
	operator()(std::istream& s)
	{
		size_t arr = read_size(s);  // number of arrays
		array <array<T> > a(arr);

		size_t dim;  // dimension of each array
		for (size_t n = 0; n < arr; n++) {
			dim = read_size(s);
			a[n].init(dim);
			read(a[n], s);
		}
		return a;
	}
};

//-----------------------------------------------------------------------------
// read array_2d from binary file

template <class T>
struct read_array_2d
{
	typedef array_2d <T> return_type;

	array_2d <T>
	operator()(std::istream& s)
	{
		array_2d <T> a(read_size(s), read_size(s));
		read(a, s);
		return a;
	}
};

//-----------------------------------------------------------------------------
// read array of arrays of equal size from binary file into array_2d

template <class T>
struct read_array_2d <array <T> >
{
	typedef array_2d <T> return_type;

	array_2d <T>
	operator()(std::istream& s)
	{
		size_t arr = read_size(s);  // number of arrays
		if (arr < 1)
			return array_2d <T>();

		size_t dim = read_size(s);  // dimension of each array
		array_2d <T> a(dim, arr);

		for (size_t n = 0, p = 0; n < arr; n++, p += dim) {
			if(n > 0) dim = read_size(s);
			read(a[p], s, dim);
		}
		return a;
	}
};

//-----------------------------------------------------------------------------

template <template <typename, typename> class CONT, class T, class K>
void write_array(const CONT<T, K>& a, std::ostream& s)
{
	write_size(a.size(), s);
	write(a, s);
}

//-----------------------------------------------------------------------------

template <
	template <typename, typename > class CONT_EX,
	template <typename, typename> class CONT_IN,
	class T, class K, class D
>
void write_array(const CONT_EX <CONT_IN<T, K>, D>& a, std::ostream& s)
{
	write_size(a.size(), s);
	for (size_t n = 0; n < a.size(); n++)
		write_array(a[n], s);
}

//-----------------------------------------------------------------------------

template <class T, class K>
void write_array_2d(const array_2d <T, K>& a, std::ostream& s)
{
	size_t arr = a.columns();  // number of arrays
	write_size(arr, s);

	size_t dim = a.rows();  // dimension of each array
	for (size_t n = 0, p = 0; n < arr; n++, p += dim) {
		write_size(dim, s);
		write(a[p], s, dim);
	}
}

//-----------------------------------------------------------------------------

}  // namespace load_details

//-----------------------------------------------------------------------------

using load_details::read;
using load_details::write;
using load_details::read_array;
using load_details::read_array_2d;
using load_details::write_array;
using load_details::write_array_2d;

//-----------------------------------------------------------------------------

}  // namespace ivl

#endif // IVL_IO_HPP
