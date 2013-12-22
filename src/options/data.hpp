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

#ifndef OPTIONS_DATA_HPP
#define OPTIONS_DATA_HPP

namespace drvq {

using namespace ivl;
using namespace std;

//-----------------------------------------------------------------------------

struct descriptor_options
{
	enum descriptor_type { sift, surf };
	enum norm_type { l2, root };

	// parameters
	int_<descriptor_type> descriptor;  // descriptor type [SIFT / SURF]
	int_<norm_type> norm;              // descriptor normalization type

	descriptor_options() :
		descriptor(sift), norm(root)
		{ }

	template <typename A, typename C>
	void args(A* arg, C cmd)
	{
		arg->set(cmd, "descriptor", descriptor(), "D", "descriptor type [0: SIFT, 1: SURF]");
		arg->set(cmd, "norm",       norm(),       "n", "descriptor normalization [0: L2, 1: root]");
	}
};

//-----------------------------------------------------------------------------

template <bool demo>
struct file_opt
{
	static string home()    { return "/data/exp/"; }
	static string dataset() { return "oxford"; }
};

template <>
struct file_opt <true>
{
	static string home()    { return "../data/"; }
	static string dataset() { return "demo"; }
};

// choose DEMO version here
typedef file_opt <true> file_options;

//-----------------------------------------------------------------------------

struct data_options : public file_options
{
	// paths / files
	string dataset;  // dataset name
	string unit;     // dataset unit name
	string path;     // data path
	string list;     // data file list
	string ext;      // data file extension
	int files;    // maximum number of files to load

	data_options() : dataset(file_options::dataset()), files(-1) { }

	virtual void set_dataset() = 0;

	template <typename A, typename C>
	void args(A* arg, C cmd, const string& name)
	{
		arg->set(cmd, "dataset",    dataset,    "d", "dataset name");
		arg->set(cmd, "unit",       unit,       "u", "dataset unit name");
		arg->set(cmd, "path",       path,       "p", name + " path");
		arg->set(cmd, "list",       list,       "l", name + " file list");
		arg->set(cmd, "extension",  ext,        "e", name + " file extension");
		arg->set(cmd, "files",      files,      "f", "maximum number of files to load");
		set_dataset();
	}
};

//-----------------------------------------------------------------------------

struct offline_options : public data_options
{
	virtual void set_dataset()
	{
		if (dataset == "-") return;
		ext  = "hesaff.sift.bin";

		if (dataset == "tiny")
		{
			path = home() + "oxford/hesaff.sift/5k/descriptors";
			list = home() + "oxford/files/tiny.txt";
		}
		else if (dataset == "demo")
		{
			path = home() + "oxford/hesaff.sift/queries/descriptors";
			list = home() + "oxford/files/queries.txt";
		}
		else if (dataset == "oxford")
		{
			path = home() + "oxford/hesaff.sift/5k/descriptors";
			list = home() + "oxford/files/5062.txt";
		}
		else if (dataset == "paris")
		{
			path = home() + "paris/hesaff.sift/6k/descriptors";
			list = home() + "paris/files/6392.txt";
		}
		else if (dataset == "100k")
		{
			path = home() + "100k/hesaff.sift/descriptors/" + unit;
			list = home() + "100k/files/units/" + unit + ".txt";
		}
		else msg::error(check, "unsupported dataset");
	}

	offline_options() { set_dataset(); }
};

//-----------------------------------------------------------------------------

struct online_options : public data_options
{
	virtual void set_dataset()
	{
		if (dataset == "-") return;
		ext  = "hesaff.sift.bin";

		if (dataset == "demo")
		{
			path = home() + "oxford/hesaff.sift/queries/descriptors";
			list = home() + "oxford/files/queries.txt";
		}
		else if (dataset == "oxford")
		{
			path = home() + "oxford/hesaff.sift/queries/descriptors";
			list = home() + "oxford/files/queries.txt";
		}
		else if (dataset == "paris")
		{
			path = home() + "paris/hesaff.sift/queries/descriptors";
			list = home() + "paris/files/queries.txt";
		}
		else msg::error(check, "unsupported dataset");
	}

	online_options() { set_dataset(); }
};

//-----------------------------------------------------------------------------

struct match_options : public file_options
{
	// paths / files
	string dataset;      // dataset name
	string query_path;   // query path
	string query_list;   // query file list
	string search_path;  // search path
	string search_list;  // search file list
	string ext;      // data file extension

	void set_dataset()
	{
		if (dataset == "-") return;
		ext  = "hesaff.bin";

		if (dataset == "oxford")
		{
			search_path = home() + "oxford/hesaff.sift/5k/features";
			search_list = home() + "oxford/files/5062.txt";
			query_path  = home() + "oxford/hesaff.sift/queries/features";
			query_list  = home() + "oxford/files/queries.txt";
		}
		else if (dataset == "paris")
		{
			search_path = home() + "paris/hesaff.sift/6k/features";
			search_list = home() + "paris/files/6392.txt";
			query_path  = home() + "paris/hesaff.sift/queries/features";
			query_list  = home() + "paris/files/queries.txt";
		}
		else msg::error(check, "unsupported dataset");
	}

//-----------------------------------------------------------------------------

	match_options() : dataset(file_options::dataset()) { set_dataset(); }

	template <typename A, typename C>
	void args(A* arg, C cmd)
	{
		arg->set(cmd, "dataset",      dataset,      "d",   "dataset name");
		arg->set(cmd, "search_path",  search_path,  "sp",  "search path");
		arg->set(cmd, "search_list",  search_list,  "sl",  "search file list");
		arg->set(cmd, "query_path",   query_path,   "qp",  "query path");
		arg->set(cmd, "query_list",   query_list,   "ql",  "query file list");
		arg->set(cmd, "extension",    ext,          "e",   "file extension");
		set_dataset();
	}
};

//-----------------------------------------------------------------------------

struct gt_options : public file_options
{
	// paths / files
	string dataset;     // dataset name
	string queries;     // query list
	string search;      // search file list
	string gt_path;     // ground truth data path
	string gt_queries;  // ground truth query list

	string distr_set;   // distractor dataset name
	string distr;       // distractor file list

	void set_dataset()
	{
		if (dataset == "-") return;
		else if (dataset == "oxford")
		{
			queries    = home() + "oxford/files/queries.txt";
			search     = home() + "oxford/files/5062.txt";
			gt_path    = "/data/media/datasets/oxford_buildings/gt";
			gt_queries = home() + "oxford/files/gt.txt";
		}
		else if (dataset == "paris")
		{
			queries    = home() + "paris/files/queries.txt";
			search     = home() + "paris/files/6392.txt";
			gt_path    = "/data/media/datasets/paris/gt";
			gt_queries = home() + "paris/files/gt.txt";
		}
		else msg::error(check, "unsupported dataset");
	}

	void set_distractor_set()
	{
		if (distr_set.empty()) return;
		else if (distr_set == "100k")
			distr = home() + "100k/files/all/100071.txt";
		else msg::error(check, "unsupported distractor set");
	}

//-----------------------------------------------------------------------------

	gt_options() : dataset(file_options::dataset()) { set_dataset(); }

	template <typename A, typename C>
	void args(A* arg, C cmd)
	{
		arg->set(cmd, "dataset",    dataset,    "d",  "dataset name");
		arg->set(cmd, "queries",    queries,    "ql", "query list");
		arg->set(cmd, "search",     search,     "s",  "search file list");
		arg->set(cmd, "gt_path",    gt_path,    "gp", "ground truth data path");
		arg->set(cmd, "gt_queries", gt_queries, "gl", "ground truth query list");
		set_dataset();

		arg->set(cmd, "distr_set",  distr_set,  "ds", "distractor set name");
		arg->set(cmd, "distr",      distr,      "dl", "distractor file list");
		set_distractor_set();
	}
};

//-----------------------------------------------------------------------------

}  // namespace drvq

#endif  // OPTIONS_DATA_HPP
