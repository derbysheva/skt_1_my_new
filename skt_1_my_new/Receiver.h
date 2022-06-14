#pragma once
#include <vector>
#include <fstream>
#include "Vector3.h"
using namespace std;

class Receiver
{
public:
	vector<vector3>coords_rec;
	int n_rec;//����� ��������� �� ����� �������
	int n_pr;//����� ��������
	double x_rec_max, x_rec_min, y_rec_min, y_rec_max, z_rec; //���������� ����� ����������

	//void read_count(const string& filename)
	//{
	//	ifstream fin(filename);
	//	fin >> n_pr >> n_rec; //����� ��������, ����� ����������
	//	fin.close();
	//}
	void generate(const string& filename, const string& outfile) // ������������� ���������� ���������
	{
		ifstream fin(filename);
		ofstream fout(outfile);

		fin >> n_pr >> n_rec; //����� ��������, ����� ����������
		fin >> x_rec_min >> x_rec_max >> y_rec_min >> y_rec_max >> z_rec;
		double hxr = (x_rec_max - x_rec_min) / (n_rec - 1.);
		double hyp = (y_rec_max - y_rec_min) / (n_pr - 1.);
		double x_cur, y_cur = y_rec_min;
		for (int i = 0; i < n_pr; i++) {
			x_cur = x_rec_min;
			for (int j = 0; j < n_rec; j++) {
				fout << x_cur << " " << y_cur << " " << z_rec << endl;
				x_cur += hxr;
			}
			y_cur += hyp;
		}
		fin.close();
		fout.close();
	}
	void read_coord(const string& filename) //������� ���������� ���� ��������� �� �����
	{
		double rec_x, rec_y, rec_z;
		ifstream fin(filename);
		for (int i = 0; i < n_rec * n_pr; i++) {
			fin >> rec_x >> rec_y >> rec_z;
			coords_rec.push_back(vector3(rec_x, rec_y, rec_z));
		}
		fin.close();
	}
};