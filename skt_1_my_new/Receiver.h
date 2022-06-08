#pragma once
#include <vector>
#include "Vector3.h"
using namespace std;

class Receiver
{
public:
	vector<vector3>coords_rec;
	int n_rec;//����� ��������� �� ����� �������
	int n_pr;//����� ��������
	double x_rec_max, x_rec_min, y_rec_min, y_rec_max, z_rec; //���������� ������� ����� ���������

	void read_count(const string& filename);
	void generate(const string& filename, const string& outfile); // ������������� ���������� ���������
	void read_coord(const string& filename); //������� ���������� ���� ��������� �� �����
};