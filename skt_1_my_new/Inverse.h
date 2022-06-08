#pragma once
#define _USE_MATH_DEFINES
#include <iostream>
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

	Inverse(Area& cur_area, Receiver& cur_rec)
	{
		ifstream fin("regularization.txt");
		fin >> regular_case;
		fin.close();

		inv_area = cur_area;
		inv_rec = cur_rec;
		K = inv_area.cells.size();// количество ячеек
		N = inv_rec.coords_rec.size(); // количество приёмников
		A.resize(3 * K);
		for (int i = 0; i < A.size(); i++)
			A[i].resize(3 * K);

		b.resize(3 * K);
		p.resize(3 * K);

		L.resize(3 * K);
		for (int i = 0; i < L.size(); i++)
			L[i].resize(N);
		gamma.resize(K);

		make_L();
		make_A();
		make_B();
	};
	
	int gauss(vector<vector<double>> a, vector<double> y, int n, vector<double>& solve)
	{
		double max;
		int k, index;
		const double eps = 1e-15;  // точность
		k = 0;
		while (k < n)
		{
			//Поиск строки с максимальным a[i][k]
			max = abs(a[k][k]);
			index = k;
			for (int i = k + 1; i < n; i++)
			{
				if (abs(a[i][k]) > max)
				{
					max = abs(a[i][k]);
					index = i;
				}
			}

			//Проверка на наличие ненулевых элементов в столбце ? ?
			if (max <= eps)
			{
				cout << "Решение получить невозможно из-за нулевого столбца ";
				cout << index << " матрицы A" << endl;
				return 1;
			}

			//Перестановка строк
			for (int j = 0; j < n; j++) {
				double temp = a[k][j];
				a[k][j] = a[index][j];
				a[index][j] = temp;
			}
			double temp = y[k];
			y[k] = y[index];
			y[index] = temp;

			//Нормализация уравнений
			for (int i = k; i < n; i++) {
				double temp = a[i][k];
				if (abs(temp) > eps) { // для нулевого коэффициента пропустить
					for (int j = 0; j < n; j++) {
						a[i][j] = a[i][j] / temp;
					}
					y[i] = y[i] / temp;
					if (i != k) { // уравнение не вычитать само из себя
						for (int j = 0; j < n; j++) {
							a[i][j] = a[i][j] - a[k][j];
						}
						y[i] = y[i] - y[k];
					}
				}
			}
			k++;
		}

		//обратный ход
		for (k = n - 1; k >= 0; k--)
		{
			solve[k] = y[k];
			for (int i = 0; i < k; i++) {
				y[i] = y[i] - a[i][k] * solve[k];
			}
		}
		return 0;
	}

	void solve(vector<vector<double>>& A_c)
	{
		gauss(A_c, b, b.size(), p);
		//р по ячейкам
		for (int i = 0; i < inv_area.cells.size(); i++) {
			inv_area.cells[i].p.x = p[3 * i];
			inv_area.cells[i].p.y = p[3 * i + 1];
			inv_area.cells[i].p.z = p[3 * i + 2];
		}

		inv_area.direct(inv_rec, inv_area.B_new);

		//считаем функционал
		functional = 0;
		for (int i = 0; i < inv_area.B.size(); i++) {
			functional += (inv_area.B[i].x - inv_area.B_new[i].x) * (inv_area.B[i].x - inv_area.B_new[i].x);
			functional += (inv_area.B[i].y - inv_area.B_new[i].y) * (inv_area.B[i].y - inv_area.B_new[i].y);
			functional += (inv_area.B[i].z - inv_area.B_new[i].z) * (inv_area.B[i].z - inv_area.B_new[i].z);
		}
	}

	void make_L()
	{
		//	 без Гаусса
		//по всем кубсонам
		//L заполняется по строкам: кубсоны по всем компонентам(x,y,z), по столбцам:все приёмники
		for (int mu = 0; mu < K; mu++)
		{
			//по всем приемникам
			for (int i = 0; i < N; i++)
			{
				double dx = inv_rec.coords_rec[i].x - inv_area.cells[mu].barycenter.x,
					dy = inv_rec.coords_rec[i].y - inv_area.cells[mu].barycenter.y,
					dz = inv_rec.coords_rec[i].z - inv_area.cells[mu].barycenter.z;
				double r = sqrt(dx * dx + dy * dy + dz * dz), r2 = 1.0 / (r * r);
				double koef = inv_area.cells[mu].mes / (4.0 * M_PI * r * r * r);

				//коэффициенты для Вх
				L[3 * mu][i].x += koef * (3.0 * dx * dx * r2 - 1.0);
				L[3 * mu][i].y += koef * (3.0 * dx * dy * r2);
				L[3 * mu][i].z += koef * (3.0 * dx * dz * r2);

				//коэффициенты для Вy
				L[3 * mu + 1][i].x += koef * (3.0 * dx * dy * r2);
				L[3 * mu + 1][i].y += koef * (3.0 * dy * dy * r2 - 1.0);
				L[3 * mu + 1][i].z += koef * (3.0 * dy * dz * r2);

				//коэффициенты для Вz
				L[3 * mu + 2][i].x += koef * (3.0 * dx * dz * r2);
				L[3 * mu + 2][i].y += koef * (3.0 * dy * dz * r2);
				L[3 * mu + 2][i].z += koef * (3.0 * dz * dz * r2 - 1.0);
			}
		}
	};

	void make_A()
	{
		for (int q = 0; q < A.size(); q++)
			for (int s = 0; s < A[q].size(); s++)
			{
				A[q][s] = 0;
				for (int i = 0; i < N; i++)
					A[q][s] += L[q][i] * L[s][i];
			}
	};

	void make_B()
	{
		for (int q = 0; q < b.size(); q++)
		{
			b[q] = 0;
			for (int i = 0; i < N; i++)
				b[q] += L[q][i] * inv_area.B[i];
		}
	};

	double find_best_alpha()
	{
		double alpha_pred = 0, alpha_2_pred, alpha_cur;
		double func_first = 0, func = 0;
		solve(A);
		func_first = functional;

		//подберем первую альфу
		alpha_cur = alpha_first;

		cout << "  alpha " << alpha_cur << endl;
		do {
			add_alpha(alpha_cur, A_cur);
			//вычислим функционал при текущей альфе
			solve(A_cur);
			func = functional;
			cout << "  alpha " << alpha_cur << endl;
			alpha_2_pred = alpha_pred;
			alpha_pred = alpha_cur;
			alpha_cur = alpha_pred * 10;
		} while (func / func_first < 1000);
		return alpha_2_pred;
	}

	void add_alpha(double alp, vector<vector<double>>& A_c)
	{
		A_c = A;
		for (int q = 0; q < A.size(); q++)
			A_c[q][q] += alp;
	}

	vector<vector3> find_best_gamma()
	{
		vector<vector3> gamma_cur(K), gamma_pred(K), gamma_2_pred(K);
		bool is_gamma_changed;
		//инициализируем
		for (int i = 0; i < K; i++) {
			gamma_cur[i] = vector3(gamma_first, gamma_first, gamma_first);
			gamma_pred[i] = vector3(0, 0, 0);
		}
		A_cur = A;
		solve(A);
		double func_first = functional, func = 0;

		//вот здесь цикл по функционалам
		do
		{
			A_cur = A;
			//добавим в матрицу
			add_gamma(gamma_cur, A_cur);

			solve(A_cur);
			func = functional;
			cout << func << endl;
			gamma_2_pred = gamma_pred;
			gamma_pred = gamma_cur;

			//коррекция гаммы по р
			is_gamma_changed = false;
			//по всем ячейкам
			for (int k = 0; k < K; k++)
			{
				//по всем соседям
				int count_neib = inv_area.cells[k].neighbors.size();
				for (int a = 0; a < count_neib; a++)
				{
					int m = inv_area.cells[k].neighbors[a];
					// Если разница между соотв. компонентами P больше порядка и при этом это не околонулевые значения (не вышли  за границы аномальной области)
					// Тогда обновим гамму и запомним это

					//по x
					if (abs(log10(inv_area.cells[k].p.x / inv_area.cells[m].p.x)) > 1. && inv_area.cells[k].p.x > gamma_diff && inv_area.cells[m].p.x > gamma_diff)
					{
						gamma_cur[k] = gamma_cur[k] * 10;
						is_gamma_changed = true;
					}
					//по y
					if (abs(log10(inv_area.cells[k].p.y / inv_area.cells[m].p.y)) > 1. && inv_area.cells[k].p.y > gamma_diff && inv_area.cells[m].p.y > gamma_diff)
					{
						gamma_cur[k] = gamma_cur[k] * 10;
						is_gamma_changed = true;
					}
					//по z
					if (abs(log10(inv_area.cells[k].p.z / inv_area.cells[m].p.z)) > 1. && inv_area.cells[k].p.z > gamma_diff && inv_area.cells[m].p.z > gamma_diff)
					{
						gamma_cur[k] = gamma_cur[k] * 10;
						is_gamma_changed = true;
					}
				}
			}
		} while (func / func_first < 1000 && is_gamma_changed == true);

		return gamma_2_pred;
	}

	void add_gamma(vector<vector3> gam, vector<vector<double>>& A_c)
	{
		//добавим в матрицу
		int m;
		//по всем ячейкам
		for (int k = 0; k < K; k++)
		{
			vector3 add_di(0, 0, 0);
			//по всем соседям
			int count_neib = inv_area.cells[k].neighbors.size();
			for (int a = 0; a < count_neib; a++)
			{
				m = inv_area.cells[k].neighbors[a];
				//второе слагаемое из диагонали
				add_di = add_di + gam[m];

				//внедиагональный элемент
				vector3 add = gam[k] + gam[m];
				A_c[k * 3][m * 3] -= add.x;
				A_c[k * 3 + 1][m * 3 + 1] -= add.y;
				A_c[k * 3 + 2][m * 3 + 2] -= add.z;
			}
			//первое слагаемое из диагонали
			add_di = add_di + gam[k] * count_neib;

			//добавим диагональку в матрицу А_cur
			A_c[k * 3][k * 3] += add_di.x;
			A_c[k * 3 + 1][k * 3 + 1] += add_di.y;
			A_c[k * 3 + 2][k * 3 + 2] += add_di.z;
		}
	}

	void regularization()
	{
		switch (regular_case) {
		case 1: // только по альфе
			//alpha = alpha_first;
			alpha = find_best_alpha();
			cout << endl << alpha << endl;
			add_alpha(alpha, A);
			break;
		case 2: // только по гамме
			gamma = find_best_gamma();
			add_gamma(gamma, A);
			break;
		case 3: // и по альфе, и по гамме
			alpha = find_best_alpha();
			add_alpha(alpha, A);
			gamma = find_best_gamma();
			add_gamma(gamma, A);
			break;
		default: break;
		}
	}
};