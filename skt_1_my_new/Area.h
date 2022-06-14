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
	int nx, ny, nz;

	vector3 get_B(vector3 point)
	{
		//общее ¬ - сумма ¬ по всем €чейкам
		vector3 result(0, 0, 0);
		for (size_t i = 0; i < cells.size(); i++)
		{
			result = result + cells[i].get_B(point);
		}
		return result;
	}

	void generate(const string& filename)
	{
		ifstream fin(filename);
		double x1, x2, y1, y2, z1, z2; // границы области
		//int nx, ny, nz; // количество €чеек
		fin >> x1 >> x2 >> nx;
		fin >> y1 >> y2 >> ny;
		fin >> z1 >> z2 >> nz;

		double hx = (x2 - x1) / nx;
		double hy = (y2 - y1) / ny;
		double hz = (z2 - z1) / nz;
		double xk = x1, yk = y1, zk = z1;

		// равномерна€ сетка
		for (int k = 0; k < nz + 1; k++) {
			yk = y1;
			for (int j = 0; j < ny + 1; j++) {
				xk = x1;
				for (int i = 0; i < nx + 1; i++) {
					nodes.push_back(vector3(xk, yk, zk));
					xk += hx;
				}
				yk += hy;
			}
			zk += hz;
		}

		K = nx * ny * nz;
		//€чейки
		for (int k = 0; k < nz; k++) {
			for (int j = 0; j < ny; j++) {
				for (int i = 0; i < nx; i++) {
					Cell t;
					t.nodes[0] = nodes[i + j * (nx + 1) + k * (nx + 1) * (ny + 1)];
					t.nodes[1] = nodes[(i + 1) + j * (nx + 1) + k * (nx + 1) * (ny + 1)];
					t.nodes[2] = nodes[i + (j + 1) * (nx + 1) + k * (nx + 1) * (ny + 1)];
					t.nodes[3] = nodes[(i + 1) + (j + 1) * (nx + 1) + k * (nx + 1) * (ny + 1)];
					t.nodes[4] = nodes[i + j * (nx + 1) + (k + 1) * (nx + 1) * (ny + 1)];
					t.nodes[5] = nodes[(i + 1) + j * (nx + 1) + (k + 1) * (nx + 1) * (ny + 1)];
					t.nodes[6] = nodes[i + (j + 1) * (nx + 1) + (k + 1) * (nx + 1) * (ny + 1)];
					t.nodes[7] = nodes[(i + 1) + (j + 1) * (nx + 1) + (k + 1) * (nx + 1) * (ny + 1)];
					t.init(); //здесь рассчитываетс€ объем и центр €чейки

					//вычислим (глобальный) номер текущей €чейки
					int cur_cube_num = k * (nx * ny) + j * nx + i;

					//правый сосед по х
					if (cur_cube_num + 1 < (j + 1) * nx + k * nx * ny)
						//t.neighbors.push_back(cur_cube_num + 1);
						t.neighbors[0] = cur_cube_num + 1;
					//левый сосед по х
					if (cur_cube_num - 1 >= j * nx + k * nx * ny)
						//t.neighbors.push_back(cur_cube_num - 1);
						t.neighbors[1] = cur_cube_num - 1;
					//правый сосед по у
					if (cur_cube_num + nx < (k + 1) * nx * ny)
						//t.neighbors.push_back(cur_cube_num + nx);
						t.neighbors[2] = cur_cube_num + nx;
					//левый сосед по y
					if (cur_cube_num - nx >= k * nx * ny)
						//t.neighbors.push_back(cur_cube_num - ny);
						t.neighbors[3] = cur_cube_num - ny;
					//правый сосед по z
					if (cur_cube_num + nx * ny < K)
						//t.neighbors.push_back(cur_cube_num + nx * ny);
						t.neighbors[4] = cur_cube_num + nx * ny;
					//левый сосед по z
					if (cur_cube_num - nx * ny >= 0)
						//t.neighbors.push_back(cur_cube_num - nx * ny);
						t.neighbors[5] = cur_cube_num - nx * ny;

					t.p = vector3(0, 0, 0);
					cells.push_back(t);//все разбиени€
				}
			}
		}
		fin.close();
	}
	void generate_anoms(const string& anomal) {

		//считали аномалии
		ifstream fin2(anomal);
		int n_anom;
		fin2 >> n_anom;
		vector <Anomaly> anoms;
		for (int i = 0; i < n_anom; i++)
		{
			Anomaly a;
			fin2 >> a.x_anom_min >> a.x_anom_max >> a.y_anom_min >> a.y_anom_max >> a.z_anom_min >> a.z_anom_max;
			fin2 >> a.p.x >> a.p.y >> a.p.z;
			anoms.push_back(a);
		}
		fin2.close();

		for (int i = 0; i < n_anom; i++) {
			Anomaly cur_inc = anoms[i];
			//выбрать €чейки, вход€щие в область текущей iтой аномалии и задать им р
			// €чейка полностью входит в область аномалии
			for (int j = 0; j < cells.size(); j++)
			{
				Cell& cur_cell = cells[j];
				if (cur_cell.nodes[0].x >= cur_inc.x_anom_min && cur_cell.nodes[1].x <= cur_inc.x_anom_max &&
					cur_cell.nodes[0].y >= cur_inc.y_anom_min && cur_cell.nodes[2].y <= cur_inc.y_anom_max &&
					cur_cell.nodes[0].z >= cur_inc.z_anom_min && cur_cell.nodes[4].z <= cur_inc.z_anom_max)
					cur_cell.p = cur_inc.p;
			}
		}
	}
	void direct(Receiver& cur_rec, vector<vector3>& B_cur)
	{
		//пр€ма€ задача, вход - вектор собственной намагниченности – во всей области(и аномалии),
		// выход - вектор магнитной индукции ¬ в каждом приемнике
		B_cur.clear();
		for (int i = 0; i < cur_rec.coords_rec.size(); i++)
		{
			vector3 temp = get_B(cur_rec.coords_rec[i]);
			B_cur.push_back(temp);
		}
	}
};