#pragma once
#include "Cell.h"
#include "Receiver.h"
#include "Vector3.h"
#include "Anomaly.h"
#include <string>
#include <fstream>
using namespace std;

class Area
{
public:
	vector<vector3> nodes;
	vector<Cell> cells;
	vector<vector3> B;
	vector<vector3> B_new;
	int K;  //количество €чеек

	vector3 get_B(vector3 point);
	double get_abs_B(vector3 point) const;
	void generate(const string& filename);
	void generate_anoms(const string& anomal);
	void direct(Receiver& cur_rec, vector<vector3>& B_cur);
};