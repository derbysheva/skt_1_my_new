#pragma once
#include <vector>
using namespace std;

class Cell
{
public:
	int num;
	vector3 nodes[8], p, barycenter;
	double mes;
	vector<int> neighbors;
	Cell() {};

	void init();
	vector3 get_B(vector3 point);

};