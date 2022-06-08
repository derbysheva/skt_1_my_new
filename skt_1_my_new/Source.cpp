#include "Anomaly.h"
#include "Area.h"
#include "Cell.h"
#include "Inverse.h"
#include "Receiver.h"
#include "Vector3.h"
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
		fout << cur_area.B[i].x << "\t" << cur_area.B[i].y << " " << cur_area.B[i].z << endl;
	}
	fout.close();
	//inverse
	Area inv_area;
	inv_area.generate("area.txt");
	inv_area.B = cur_area.B; //передаем выход прямой задачи на вход обратной
	Inverse cur_inv(inv_area, cur_rec);

	//регуляризация по альфа
	cur_inv.regularization();

	//РЕШЕНИЕ СЛАУ
	cur_inv.solve(cur_inv.A);

	ofstream fout2("result_p.txt");
	for (int i = 0; i < cur_inv.p.size() / 3; i++) {
		if (i % 9 == 0) fout2 << endl;
		fout2 << cur_inv.p[3 * i] << "\t" << cur_inv.p[3 * i + 1] << " " << cur_inv.p[3 * i + 2] << endl;
	}
	fout2.close();

	cout << cur_inv.functional;
	return 0;
}