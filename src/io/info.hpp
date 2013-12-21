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

#ifndef IO_INFO_HPP
#define IO_INFO_HPP

namespace qc {

using namespace ivl;
using namespace std;

using ivl::find;
using ivl::max;
using ivl::min;
using ivl::mean;

//-----------------------------------------------------------------------------

types::t_true info;
types::t_true check;
types::t_true error;

namespace msg {

//-----------------------------------------------------------------------------

typedef types::t_true yes;
typedef types::t_false no;

//-----------------------------------------------------------------------------

template <typename T>
void require(no,  const T& x, const string& message) { }

template <typename T>
void require(yes, const T& x, const string& message)
{
	if (x) return;
	cerr << endl << bright << "error: " << message << "... aborting" <<
		normal << endl << endl;
	exit(0);
}

void error(no,  const std::string& message) { }
void error(yes, const std::string& message) { require(yes(), false, message); }

void load_fail(no,  const std::string& filename) { }
void load_fail(yes, const std::string& filename)
{
	error(yes(), string() + "cannot open binary file: " + filename);
}

//-----------------------------------------------------------------------------

void disp(no,  const string& message) { }
void disp(yes, const string& message)
{
	cout << message << endl;
}

template <typename T>
void disp(no,  const string& pre, const T& x, const string& post = "") { }

template <typename T>
void disp(yes, const string& pre, const T& x, const string& post = "")
{
	cout << pre << x << post << endl;
}

template <typename T>
void head(no,  const string& pre, const T& x, const string& post = "") { }

template <typename T>
void head(yes, const string& pre, const T& x, const string& post = "")
{
	cout << bright << pre << x << post << normal << endl;
}

void in_line(no,  const string& message) { }
void in_line(yes, const string& message)
{
	cout << message;
	cout.flush();
}

template <typename T>
void in_line(no,  const string& pre, const T& x, const string& post = "") { }

template <typename T>
void in_line(yes, const string& pre, const T& x, const string& post = "")
{
	cout << pre << x << post;
	cout.flush();
}

void nl(no,  bool detail = true) { }
void nl(yes, bool detail = true)
{
	if (detail) cout << endl;
	else cout.flush();
}

template <typename ARG>
void param(no,  const ARG& arg) { }

template <typename ARG>
void param(yes, const ARG& arg, const string& head = "parameters: ")
{
	cout << head << endl;
	arg.display();
}

//-----------------------------------------------------------------------------

void dot(no) { }
void dot(yes)
{
	cout << ".";
	cout.flush();
}

void progress(no,  size_t i, size_t total) { }
void progress(yes, size_t i, size_t total)
{
	if ((i % (total / 20)) == 0) dot(yes());
}

void percent(no,  const string& message, size_t n, size_t N) { }
void percent(yes, const string& message, size_t n, size_t N)
{
	cout << message << " (" << 100. * n / N << "%)" << endl;
}

void percent(no,  size_t i, const string& message, size_t n, size_t N) { }
void percent(yes, size_t i, const string& message, size_t n, size_t N)
{
	cout << i << " " << message << " (" << 100. * n / N << "%)" << endl;
}

void done(no) { }
void done(yes)
{
	cout << " done" << endl;
}

void test(no) { }
void test(yes, bool ok)
{
	cout << bright << " " << (ok ? "OK" : "FAIL") << normal << endl;
}

//-----------------------------------------------------------------------------

void time(no,  const string& message, double t) { }
void time(yes, const string& message, double t)
{
	cout << message << " " << bright << t / 1000 << normal << " s" << endl;
}

void avg_time(no,  const string& message, double t_file,
				  const string& file_name = "file") { }

void avg_time(yes, const string& message, double t_file,
				  const string& file_name = "file")
{
	cout << message << " " << bright << t_file << normal << " ms/" <<
		file_name << endl;
}

void avg_time(no,  const string& message, double t_file, double t_point,
				  const string& file_name = "file", const string pt_name = "point")
{ }

void avg_time(yes, const string& message, double t_file, double t_point,
				  const string& file_name = "file", const string pt_name = "point")
{
	cout << message << " " <<
		bright << t_file << normal << " ms/" << file_name << ", " <<
		bright << t_point * 1000 << normal << " us/" << pt_name << endl;
}

//-----------------------------------------------------------------------------

void data(no,  size_t F, size_t N) { }
void data(yes, size_t F, size_t N)
{
	cout << F << " files, " << N << " points" << endl;
}

void data(no,  size_t F, size_t N, size_t D) { }
void data(yes, size_t F, size_t N, size_t D)
{
	cout << F << " files, " << N << " points, " << D << " dimensions" << endl;
}

template <typename T>
void data(no,  size_t F, const array <array <T> >& X) { }

template <typename T>
void data(yes, size_t F, const array <array <T> >& X)
{
	size_t D = X.length();
	size_t N = D ? X[0].length() : 0;
	data(yes(), F, N, D);
}

//-----------------------------------------------------------------------------

void capacity(no,  const size_array& cap) { }
void capacity(yes, const size_array& cap)
{
	cout << "capacity (codebook size), for levels 0-" <<
		cap.length() - 1 << ": " << bright << cap << normal << endl;
}

//-----------------------------------------------------------------------------

void mid(no,  const size_array& mid) { }
void mid(yes, const size_array& mid)
{
	if (any(diff(mid) == 0))
		cout << "--> midpoint collision" << endl;
}

//-----------------------------------------------------------------------------

void level(no,  size_t L, const size_array& at, size_t K, size_t J,
			  bool detail) { }

void level(yes, size_t L, const size_array& at, size_t K, size_t J,
			  bool detail)
{
	if (detail) cout << bright;
	cout << "level " << L;
	if (detail) cout << normal;
	cout << " (dimensions " << at[0] << "-" << at[at.length()-1] << ", " <<
		K << " centroids, " << J << "x" << J << " grid)";
}

void iter(no,  size_t it) { }
void iter(yes, size_t it)
{
	cout << "iteration " << it;
	cout.flush();
}

void term(no,  size_t it) { }
void term(yes, size_t it)
{
	cout << " - " << it << " iterations" << endl;
}

//-----------------------------------------------------------------------------

template <typename T>
void edge(no,  const array <array <T> >& weight, double range) { }

template <typename T>
void edge(yes, const array <array <T> >& weight, double range)
{
	size_t K = weight.length();
	size_array degree(K), pruned(K);
	for(size_t k = 0; k < K; k++)
	{
		degree[k] = weight[k].length() - 1;
		pruned[k] = find(weight[k] < range).length() - 1;
	}
	cout << "max " << max(degree) << ", avg " << mean(degree)
		<< "; pruned: max " << max(pruned) << ", avg " <<
		bright << mean(pruned) << normal << endl;
}

template <typename L, typename S1, typename S2>
bool zero(no, size_t K, const array <L, S1>& source,
			 const array <L, S2>& b_pop) { return false; }

template <typename L, typename S1, typename S2>
bool zero(yes, size_t K, const array <L, S1>& source,
			 const array <L, S2>& b_pop)
{
	size_array pop(K, size_t(0));
	pop[source] += b_pop;
	size_array z = find(!pop);
	if (!z.empty()) cout << bright << "--> " <<
		z.length() << "/" << K << " zero centroids" << normal << endl;
	return !z.empty();
}

//-----------------------------------------------------------------------------

template <typename T>
void max_update(no,  T& max_val, T cur_val) { }
template <typename T>
void max_update(yes, T& max_val, T cur_val)
{
	max_val = max(max_val, cur_val);
}

void queue(no,  size_t queue_length, size_t heap_length) { }
void queue(yes, size_t queue_length, size_t heap_length)
{
	if (heap_length)
		cout << " queue / heap length " << queue_length << " / " <<
			heap_length << endl;
	else
		cout << " queue length " << queue_length << endl;
}

void collide(no,  const array_2d <size_t>& state, size_t p, size_t alive,
				 size_t k) { }
void collide(yes, const array_2d <size_t>& state, size_t p, size_t alive,
				 size_t k)
{
	if (state[p] != alive) cout << bright << "--> centroid " << k <<
		" collided" << normal << endl;
}

void visit(no,  const array_2d <size_t>& state, size_t burnt, bool term) { }
void visit(yes, const array_2d <size_t>& state, size_t burnt,
			  const array_2d <size_t>& target, bool term)
{
	size_t bad = ivl::find(target && state != burnt).length();  // TODO: count
	if (!bad) return;
	cout << bright << "--> " << bad << "/" << state.length() << " bins alive";
	if (term) cout << " at termination";
	cout << normal << endl;
}

//-----------------------------------------------------------------------------

void book(no,  size_t D, size_t C, size_t J, size_t B) { }
void book(yes, size_t D, size_t C, size_t J, size_t B)
{
	cout << D << " dimensions, " << C << " codebooks, " <<
		J << "^" << C << " grid, " << B << " bins" << endl;
}

//-----------------------------------------------------------------------------

void index(no,  size_t F, size_t N, size_t B, size_t E) { }
void index(yes, size_t F, size_t N, size_t B, size_t E)
{
	cout << F << " files, " << N << " points, " <<
		B << " bins (" << double(E) / B * 100 << "% non-empty)" << endl;
	cout << "average non-empty bin size: " << N / double(E) << endl;
}

void index(no,  size_t N, size_t B, size_t E) { }
void index(yes, size_t N, size_t B, size_t E)
{
	cout << "total bins: " << B << endl;
	cout << "non-empty bins: " <<
		E << " (" << double(E) / B * 100 << "%)" << endl;
	cout << "average non-empty bin size: " << N / double(E) << endl;
}

//-----------------------------------------------------------------------------

template <typename T>
void rank(no,  T rank, size_t J) { }

template <typename T>
void rank(yes, T rank, size_t J)
{
	cout << "avg rank: " << bright << rank << normal << " out of " << J << endl;
}

template <typename T>
void bounds(no,  T lower, T approx, T upper) { }

template <typename T>
void bounds(yes, T lower, T approx, T upper)
{
	cout << "distance: min " << lower << ", approx " <<
		bright << approx << normal << ", max " << upper << endl;
}

template <typename T1, typename S1, typename T2, typename S2>
void recall(no,  const array <T1, S1>& at, const array <T2, S2>& rec) { }

template <typename T1, typename S1, typename T2, typename S2>
void recall(yes, const array <T1, S1>& at, const array <T2, S2>& rec)
{
	cout << !wrap << "recall @ " << at << ":" << endl <<
		"         " << bright << rec << normal << endl;
}

template <typename T1, typename S1, typename T2, typename S2>
void ratio(no,  const array <T1, S1>& rat, const array <T2, S2>& freq) { }

template <typename T1, typename S1, typename T2, typename S2>
void ratio(yes, const array <T1, S1>& rat, const array <T2, S2>& freq)
{
	cout << !wrap << "ratio at " << rat << ":" << endl <<
		"         " << bright << freq << normal << endl;
}

//-----------------------------------------------------------------------------

void query(no,  const string& name, size_t id, size_t relevant,
			  size_t window = -1) { }

void query(yes, const string& name, size_t id, size_t relevant,
			  size_t window = -1)
{
	if (window != -1)
		cout << "top " << window << " ";
	cout << "results for query " << bright << name << normal <<
		" [" << id << "] (" << relevant << " relevant items)" << endl << endl;
}

template <typename T>
void result(no,  const array <string>& search_list, const size_array& id,
				const array <T>& score, const size_array& votes,
				const set <string>& good, size_t window = -1) { }

template <typename T>
void result(yes, const array <string>& search_list, const size_array& id,
				const array <T>& score, const size_array& votes,
				const set <string>& good, size_t window = -1)
{
	window = min(window, id.length());
	cout << "# R query name [id] (score / votes)" << endl;
	for (size_t n = 0, r = 0; n < window; n++)
	{
		cout << n << " ";
		string name = id[n] < search_list.length() ?
			search_list[id[n]] : "???";
		if (good.count(filename(name)))
			cout << bright << r++ << normal;
		else
		{	// TODO: formatting
			cout << " ";
			if (r > 9) cout << " ";
			if (r > 99) cout << " ";
		}
		cout << " " << name << " [" << id[n] << "] " <<
			"(" << score[n] << " / " << votes[n] << ")" << endl;
	}
}

//-----------------------------------------------------------------------------

void eval_ranks(no,  size_t Q, size_t F) { }
void eval_ranks(yes, size_t Q, size_t F)
{
	cout << Q << " queries, " << F << " files" << endl;
}

template <typename T>
void eval_map(no,  const array <T>& ap) { }

template <typename T>
void eval_map(yes, const array <T>& ap)
{
	cout << "mean average precision: " << bright << mean(ap) << normal << endl;
}

template <typename T>
void eval_pr(no,  const size_array& at, const array <T>& precision,
				 const array <T>& recall) { }

template <typename T>
void eval_pr(yes, const size_array& at, const array <array <T> >& precision,
				 const array <array <T> >& recall)
{
	size_t Q = precision.length();
	array <T> P(at.length(), T()), R(at.length(), T());
	for (size_t q = 0; q < Q; q++)  // TODO: mean(precision), mean(recall)
	{
		P += precision[q];
		R += recall[q];
	}
	cout << "mean precision / recall @ " << at << endl;
	cout << "P = " << P / Q << endl;
	cout << "R = " << R / Q << endl;
};

template <typename T>
void eval_ap(no,  const array <T>& ap, const array <string>& queries) { }

template <typename T>
void eval_ap(yes, const array <T>& ap, const array <string>& queries)
{
	cout << "# query - ap" << endl;
	for (size_t q = 0; q < queries.length(); q++)
	{
		cout << q << " " << queries[q] << " - ";
		if (ap[q] < .5) cout << bright;
		cout << ap[q];
		if (ap[q] < .5) cout << normal;
		cout << endl;
	}
}

//-----------------------------------------------------------------------------

}  // namespace msg

//-----------------------------------------------------------------------------

}  // namespace qc

#endif  // IO_INFO_HPP
