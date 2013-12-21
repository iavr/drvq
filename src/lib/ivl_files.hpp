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

#ifndef LIB_IVL_FILES_HPP
#define LIB_IVL_FILES_HPP

#include <fstream>

namespace ivl {

//-----------------------------------------------------------------------------

namespace load_details {

//-----------------------------------------------------------------------------

void open_fail(const std::string& filename)
{
	std::cerr << std::endl <<
		"error: cannot open file: " << filename << std::endl;
}

//-----------------------------------------------------------------------------

template <typename READ>
typename READ::return_type
load(const std::string& filename)
{
	typedef typename READ::return_type RET;
	std::ifstream s(filename.c_str(), std::ios::binary);
	if (!(s.is_open())) {
		open_fail(filename);
		return RET();
	}

	RET data = READ()(s);
	s.close();
	return data;
}

//-----------------------------------------------------------------------------

template <class T>
typename read_array_size <T>::return_type
load_array_size(const std::string& filename)
{
	return load <read_array_size <T> > (filename);
}

//-----------------------------------------------------------------------------

template <typename T>
typename read_array <T>::return_type
load_array(const std::string& filename)
{
	return load <read_array <T> > (filename);
}

//-----------------------------------------------------------------------------

template <typename T>
typename read_array_2d <T>::return_type
load_array_2d(const std::string& filename)
{
	return load <read_array_2d <T> > (filename);
}

//-----------------------------------------------------------------------------
// save array/vector or array of arrays into binary file

template < template < typename, typename > class CONT, class T, class K >
void save_array(const CONT<T, K>& a, const std::string& filename)
{
	std::ofstream file(filename.c_str(), std::ios::binary);
	write_array(a, file);
	file.close();
}

//-----------------------------------------------------------------------------
// save array_2d as array of arrays into binary file

template <class T, class K>
void save_array_2d(const array_2d <T, K>& a, const std::string& filename)
{
	std::ofstream file(filename.c_str(), std::ios::binary);
	write_array_2d(a, file);
	file.close();
}

//-----------------------------------------------------------------------------

template <typename T, bool MULTI = false, bool TYPE = false>
struct ifile
{
	static T* load(const std::string& filename)
	{
		std::ifstream s(filename.c_str(), std::ios::binary);
		if (!(s.is_open())) {
			open_fail(filename);
			return 0;
		}

		T* x = new T(s);
		s.close();
		return x;
	}
};

//-----------------------------------------------------------------------------

template <typename T>
struct ifile <T, true, false> : public ifile <T, false, false>
{
	using ifile <T, false, false>::load;

	static T* load(const array <std::string>& filenames)
	{
		T* x = new T();

		for (size_t n = 0; n < filenames.length(); n++)
			if (!filenames[n].empty())
				x->add(filenames[n]);

		for (size_t n = 0; n < filenames.length(); n++)
		{
			if (filenames[n].empty()) continue;
			std::ifstream s(filenames[n].c_str(), std::ios::binary);
			if (!(s.is_open())) {
				open_fail(filenames[n]);
				return x;
			}

			x->read(s);
			s.close();
		}

		return x;
	}
};

//-----------------------------------------------------------------------------

template <typename T>
struct ifile <T, false, true>
{
	static T* load(const std::string& filename)
	{
		std::ifstream s(filename.c_str(), std::ios::binary);
		if (!(s.is_open())) {
			open_fail(filename);
			return 0;
		}

		T* x = T::read(s);
		s.close();
		return x;
	}
};

//-----------------------------------------------------------------------------

template <typename T>
struct ofile
{
	bool save(const std::string& filename) const
	{
		std::ofstream s(filename.c_str(), std::ios::binary);
		if (!(s.is_open())) {
			open_fail(filename);
			return false;
		}

		static_cast <const T*>(this) -> write(s);
		s.close();
		return true;
	}
};

//-----------------------------------------------------------------------------

template <typename T, bool MULTI = false, bool TYPE = false>
struct iofile : public ifile <T, MULTI, TYPE>, public ofile <T> { };

//-----------------------------------------------------------------------------

std::string filename(const std::string& s) { return s.substr(0, s.rfind('.')); }

//-----------------------------------------------------------------------------

array <std::string> load_lines(const std::string& filename)
{
	std::string line;
	array <std::string> lines;

	std::ifstream s(filename.c_str());
	if (!(s.is_open())) {
		open_fail(filename);
		return array <std::string>();
	}

	while (getline(s, line)) lines.push_back(line);
	s.close();

	return lines;
}

//-----------------------------------------------------------------------------

}  // namespace load_details

//-----------------------------------------------------------------------------

using load_details::load_array_size;
using load_details::load_array;
using load_details::load_array_2d;
using load_details::save_array;
using load_details::save_array_2d;
using load_details::ifile;
using load_details::ofile;
using load_details::iofile;
using load_details::filename;
using load_details::load_lines;

//-----------------------------------------------------------------------------

}  // namespace ivl

#endif // LIB_IVL_FILES_HPP
