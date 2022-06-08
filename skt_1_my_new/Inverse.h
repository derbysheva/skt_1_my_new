#pragma once
#include "Area.h"
#include "Receiver.h"

class Inverse
{
public:
	int regular_case;
	double functional;
	Area inv_area;
	Receiver inv_rec;
	double alpha;
	vector<vector3> gamma;
	double gamma_first = 1e-15;
	double alpha_first = 1e-15;
	double gamma_diff = 1e-5;  // Критерий близости к нулю

	vector<vector<double> > A, A_cur;
	vector<double> b;
	vector<vector<vector3> > L;
	int K, N;
	vector<double> p;

	Inverse(Area& cur_area, Receiver& cur_rec);
	void input(const string& fn_area, const string& fn_receivers, const string& fn_config);
	void calc();
	void solve(vector<vector<double>>& A_c);
	void make_L();
	void make_A();
	void make_B();
	double find_best_alpha();
	void add_alpha(double alp, vector<vector<double>>& A_c);
	vector<vector3> find_best_gamma();
	void add_gamma(vector<vector3> gam, vector<vector<double>>& A_c);
	void regularization();
};