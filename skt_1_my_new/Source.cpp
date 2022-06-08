#include "Anomaly.h"
#include "Area.h"
#include "Cell.h"
#include "Inverse.h"
#include "Receiver.h"
//#include "Vector3.h"
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
	cout << scientific << setprecision(15);
	Area cur_area; 
	cur_area.generate("area.txt");
	cur_area.generate_anoms("anoms.txt");
	Receiver cur_rec;
	cur_rec.read_count("receivers_n.txt");
	cur_rec.generate("receivers_min_max.txt", "receivers_coord.txt");
	cur_rec.read_coord("receivers_coord.txt");
	cur_area.direct(cur_rec, cur_area.B);

	ofstream fout;
	fout.open("direct_out.txt");
	for (int i = 0; i < cur_area.B.size(); i++) {
		fout << cur_rec.coords_rec[i].x << "\t" << cur_area.B[i].x << "\t" << cur_area.B[i].y << "\t" << cur_area.B[i].z << endl;
	}
	fout.close();

	//inverse
	Area inv_area;
	inv_area.generate("area.txt");
	inv_area.B = cur_area.B; //передаем выход прямой задачи на вход обратной
	Inverse cur_inv(inv_area, cur_rec);

	//регуляризация
	cur_inv.regular_case = 1;
	cur_inv.regularization();

	//РЕШЕНИЕ СЛАУ
	cur_inv.solve(cur_inv.A);

	for (int i = 0; i < inv_area.cells.size(); i++) {
		inv_area.cells[i].p.x = cur_inv.p[3 * i];
		inv_area.cells[i].p.y = cur_inv.p[3 * i + 1];
		inv_area.cells[i].p.z = cur_inv.p[3 * i + 2];
	}

	ofstream pFile("result_p.txt");
	//for (int i = 0; i < cur_inv.p.size() / 3; i++) {
	//	if (i % 9 == 0) pFile << endl; 
	//	//px py pz
	//	pFile << cur_inv.p[3 * i] << "\t" << cur_inv.p[3 * i + 1] << "\t" << cur_inv.p[3 * i + 2] << endl; 
	//}
	
	auto cell = inv_area.cells[0];
	pFile << cell.p.x << "\t";
	int gcounter = 1;
	do//пока есть соседи снизу по z
	{
		int counter = 0;
		do // пока есть соседи справа по x
		{
			cell = inv_area.cells[cell.neighbors[0]];
			pFile << cell.p.x << "\t";
			counter++;
			gcounter++;
		} while (cell.neighbors[0] != 0);
		if (cell.neighbors[4] != 0)
		{
			cell = inv_area.cells[cell.neighbors[4] - counter];
			pFile << endl;
			pFile << cell.p.x << "\t";
			gcounter++;
		}
	} while (gcounter != inv_area.cells.size());
	pFile.close();


	cout << cur_inv.functional;
	return 0;
}