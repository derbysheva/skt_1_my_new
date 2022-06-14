#include "Anomaly.h"
#include "Area.h"
#include "Cell.h"
#include "Inverse.h"
#include "Receiver.h"
//#include "Vector3.h"
#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, char* argv[]) {
	cout << scientific << setprecision(15);
	Area cur_area; 
	//cur_area.generate("area.txt");
	//cur_area.generate_anoms("anoms.txt");
	//путь + название файла
	cur_area.generate(string(argv[1])+"area.txt");
	cur_area.generate_anoms(string(argv[1]) + "anoms.txt");
	Receiver cur_rec;
	//cur_rec.read_count("receivers_n.txt");
	/*cur_rec.generate("receivers_n_min_max.txt", "receivers_coord.txt");
	cur_rec.read_coord("receivers_coord.txt");*/
	cur_rec.generate(string(argv[1]) + "receivers_n_min_max.txt", string(argv[0]) + "receivers_coord.txt");
	cur_rec.read_coord(string(argv[1]) + "receivers_coord.txt");
	cur_area.direct(cur_rec, cur_area.B);

	ofstream fout;
	fout.open(string(argv[1]) + "direct_out.txt");
	//for (int i = 0; i < cur_area.B.size(); i++) {
	//	fout << cur_rec.coords_rec[i].x << "\t" << cur_area.B[i].x << "\t" << cur_area.B[i].y << "\t" << cur_area.B[i].z << endl;
	//}
	for (int i = 0; i < cur_area.B.size(); i++) {
		fout << cur_area.B[i].x << "\t" << cur_area.B[i].y << "\t" << cur_area.B[i].z << endl;
	}
	fout.close();

	//inverse
	Area inv_area;
	inv_area.generate(string(argv[1]) + "area.txt");
	inv_area.B = cur_area.B; //передаем выход пр€мой задачи на вход обратной
	Inverse cur_inv(inv_area, cur_rec);

	//регул€ризаци€
	cur_inv.regular_case = 3;
	cur_inv.regularization();

	//–≈Ў≈Ќ»≈ —Ћј”
	cur_inv.solve(cur_inv.A);

	for (int i = 0; i < inv_area.cells.size(); i++) {
		inv_area.cells[i].p.x = cur_inv.p[3 * i];
		inv_area.cells[i].p.y = cur_inv.p[3 * i + 1];
		inv_area.cells[i].p.z = cur_inv.p[3 * i + 2];
	}

	ofstream pFile(string(argv[1]) + "result_p.txt");
	//auto cell = inv_area.cells[0];
	//pFile << cell.p.x << "\t";
	//int gcounter = 1;
	//do//пока есть соседи снизу по z
	//{
	//	int counter = 0;
	//	do // пока есть соседи справа по x
	//	{
	//		cell = inv_area.cells[cell.neighbors[0]];
	//		pFile << cell.p.x << "\t";
	//		counter++;
	//		gcounter++;
	//	} while (cell.neighbors[0] != 0);
	//	if (cell.neighbors[4] != 0)
	//	{
	//		cell = inv_area.cells[cell.neighbors[4] - counter];
	//		pFile << endl;
	//		pFile << cell.p.x << "\t";
	//		gcounter++;
	//	}
	//} while (gcounter != inv_area.cells.size());
	auto cell = inv_area.cells[inv_area.cells.size()-inv_area.nx]; //крайн€€ лева€ в нижнем р€ду (но на самом деле сама€ верхн€€)
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
		if (cell.neighbors[5] != 0)
		{
			cell = inv_area.cells[cell.neighbors[5] - counter];
			pFile << endl;
			pFile << cell.p.x << "\t";
			gcounter++;
		}
	} while (gcounter != inv_area.cells.size());
	pFile.close();

	
	//печатать сетку
	ofstream cellFile(string(argv[1]) + "cells_mesh.txt");
	cell = inv_area.cells[inv_area.cells.size() - inv_area.nx]; //крайн€€ лева€ в нижнем р€ду (но на самом деле сама€ верхн€€)
	cellFile << cell.nodes[4].x << "\t" << cell.nodes[4].z << "\t" << cell.nodes[5].x << "\t" << cell.nodes[5].z
		<< "\t" << cell.nodes[0].x << "\t" << cell.nodes[0].z << "\t" << cell.nodes[1].x << "\t" << cell.nodes[1].z << endl;
	gcounter = 1;
	do//пока есть соседи снизу по z
	{
		int counter = 0;
		do // пока есть соседи справа по x
		{
			cell = inv_area.cells[cell.neighbors[0]];
			cellFile << cell.nodes[4].x << "\t" << cell.nodes[4].z << "\t" << cell.nodes[5].x << "\t" << cell.nodes[5].z
			<< "\t" << cell.nodes[0].x << "\t" << cell.nodes[0].z << "\t"  << cell.nodes[1].x << "\t" << cell.nodes[1].z << endl;
			counter++;
			gcounter++;
		} while (cell.neighbors[0] != 0);
		if (cell.neighbors[5] != 0)
		{
			cell = inv_area.cells[cell.neighbors[5] - counter];
			cellFile << cell.nodes[4].x << "\t" << cell.nodes[4].z << "\t" << cell.nodes[5].x << "\t" << cell.nodes[5].z
			<< "\t" << cell.nodes[0].x << "\t" << cell.nodes[0].z << "\t" << cell.nodes[1].x << "\t" << cell.nodes[1].z << endl;
			gcounter++;
		}
	} while (gcounter != inv_area.cells.size());
	pFile.close();

	cout << cur_inv.functional;
	return 0;
}