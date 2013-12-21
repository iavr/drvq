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

#ifndef LIB_ARGS_HPP
#define LIB_ARGS_HPP

#include <string>
#include <sstream>

//-----------------------------------------------------------------------------

namespace ivl {

//-----------------------------------------------------------------------------
// handle binary command line arguments

template <typename OPT, typename ARG>
class cmd_args : public OPT
{

//-----------------------------------------------------------------------------

	// representation of argument data
	struct argument
	{
		std::string name;
		std::string default_value;
		std::string abbrev;
		std::string help_text;
		bool mandatory;

		template <typename T>
		argument(const std::string& n, const T& d,
					const std::string& a, const std::string& h, bool m) :
			name("--" + n), default_value(to_<std::string> (d)),
			abbrev("-" + a), help_text(h), mandatory(m)
			{ }

		bool match(const std::string& s) const
		{
			return name.compare(s) == 0 || abbrev.compare(s) == 0;
		}

		void help()
		{
			using namespace std;
			cout << ++indent << endl << bright << abbrev << normal << ", " <<
				bright << name << normal << ++indent << endl << help_text <<
				" [default: " << bright << default_value << normal << "]" <<
				--indent << --indent << endl;
		}

	};

//-----------------------------------------------------------------------------

	// locate an argument with name or abbreviation occurring in the input
	// arguments
	size_t locate(const argument& a)
	{
		return first(_[a][&argument::match](args));
	}

//-----------------------------------------------------------------------------

	// get value from k-th argument, if present
	template <typename T, int D = 0>
	struct get_value
	{
		ret <T, bool>
		operator()(const array <std::string>& a,
					  const size_t k, const T& def)
		{
			return (k < a.length()) ?
				(_, to_<T> (a[k]), true) : (_, def, false);
		}
	};

	// bool (flag) specialization: negate default value of flag
	template <int D>
	struct get_value <bool, D>
	{
		ret <bool, bool>
		operator()(const array <std::string>& a,
					  const size_t k, const bool& def)
		{
			return (_, !def, true);
		}
	};

//-----------------------------------------------------------------------------

	// contains data of all arguments to display help
	array <argument> data;

	// input arguments
	array <std::string> args;

	// indicates that a mandatory argument is not set
	bool mandatory_not_set;

//-----------------------------------------------------------------------------

	ARG& derived() { return static_cast <ARG&> (*this); }
	const ARG& derived() const { return static_cast <const ARG&> (*this); }
	ARG& const_derived() const { return const_cast <ARG&> (derived()); }

//-----------------------------------------------------------------------------

	// commands
	struct read { };
	struct disp { };

//-----------------------------------------------------------------------------

public:

	// construct by command line arguments of main()
	cmd_args(int argc, char* argv[]) :
		args(array <char*> (argc, argv)),  // TODO: args(argc, argv) ?
		mandatory_not_set (false)
	{
		derived().args(read());
	}

//-----------------------------------------------------------------------------

	virtual void display() const { const_derived().args(disp()); }

//-----------------------------------------------------------------------------

	// get value of type T for argument with given name or abbreviation;
	// specify argument data to display help
	template <typename T>
	T get(read, std::string name, const T& default_value,
		   std::string abbrev = "", std::string help_text = "",
		   bool mandatory = false)
	{

		argument a(name, default_value, abbrev, help_text, mandatory);
		data.push_back(a);

		T value;
		bool ok = false;
		size_t k = locate(a);
		if (k != args.length())
			(_, value, ok) = get_value <T> ()(args, k + 1, default_value);

		if (!ok && mandatory)
			mandatory_not_set = true;

		return ok ? value : default_value;
	}

//-----------------------------------------------------------------------------

	// set value of type T for argument with given name or abbreviation;
	// specify argument data to display help
	template <typename T>
	bool set(read, std::string name, T& value,
			   std::string abbrev = "", std::string help_text = "",
			   bool mandatory = false)
	{

		argument a(name, value, abbrev, help_text, mandatory);
		data.push_back(a);

		bool ok = false;
		size_t k = locate(a);
		if (k != args.length())
			(_, value, ok) = get_value <T> ()(args, k + 1, value);

		if (!ok && mandatory)
			mandatory_not_set = true;

		return ok;
	}

//-----------------------------------------------------------------------------

	// display value of type T for argument with given name or abbreviation
	template <typename T>
	bool get(disp, std::string name, T& value,
			   std::string abbrev = "", std::string help_text = "",
			   bool mandatory = false) const
	{
		std::cout << name << " = " << bright << value << normal << std::endl;
		return false;
	}

	template <typename T>
	bool set(disp, std::string name, T& value,
			   std::string abbrev = "", std::string help_text = "",
			   bool mandatory = false) const
	{
		std::cout << name << " = " << bright << value << normal << std::endl;
		return false;
	}

//-----------------------------------------------------------------------------

	// allow -h and --help as flags to display help (and exit); has to be the
	// last argument
	void help_arg()
	{
		argument a("help", "none", "h",
					  "displays help and exits the binary", false);
		data.push_back(a);

		if (locate(a) != args.length())
			abort();
	}

//-----------------------------------------------------------------------------

	// check whether the minimum number of arguments is given, otherwise
	// display help and exit
	void check_min_args(size_t n) { if (args.length() < n) abort(); }

	// check whether all mandatory arguments are set, otherwise display help
	// and exit
	void check_mandatory_args() { if (mandatory_not_set) abort(); }

//---------------------------------------------------------------------------------

	//  shortcut for adding help and checking
	void done()
	{
		help_arg();
		check_min_args(0);
		check_mandatory_args();
	}

private:

//-----------------------------------------------------------------------------

	// display help and exit
	void abort() { display_help(); exit(1); }

//-----------------------------------------------------------------------------

	// display help given the complete argument data
	void display_help(const std::string& title = "",
							const std::string& usage_line= "",
							const std::string& info_line = "")
	{
		using namespace std;

		if (!title.empty())
			cout << endl << title << endl;

		if (!usage_line.empty())
			cout << endl << "usage: " << usage_line << endl;

		if (!info_line.empty())
			cout << endl << info_line << endl;

		array <bool> m = data[&argument::mandatory];
		display_args(data[m], "mandatory arguments:");
		display_args(data[!m], "optional arguments:");
	}

//-----------------------------------------------------------------------------

	// display help for a list of arguments
	template <typename T, typename S>
	void display_args(const array <T, S>& a, const std::string& head)
	{
		using namespace std;

		if (a.empty())
			return;

		if (!head.empty())
			cout << endl << head << endl;

 		a[&argument::help]();
		cout << endl;
	}

};

//---------------------------------------------------------------------------------

} // namespace ivl

#endif  // LIB_ARGS_HPP
