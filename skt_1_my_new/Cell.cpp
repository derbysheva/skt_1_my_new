#define _USE_MATH_DEFINES
#include "Cell.h"
#include "Vector3.h"

void Cell::init()
{
	double hx = nodes[1].x - nodes[0].x;
	double hy = nodes[2].y - nodes[0].y;
	double hz = nodes[4].z - nodes[0].z;
	mes = hx * hy * hz;
	barycenter = { (nodes[1].x + nodes[0].x) / 2, (nodes[2].y + nodes[0].y) / 2, (nodes[4].z + nodes[0].z) / 2 };
};

vector3 Cell::get_B(vector3 point)
{
	vector3 res;

	double xv = point.x - barycenter.x;
	double yv = point.y - barycenter.y;
	double zv = point.z - barycenter.z;

	double r = sqrt(xv * xv + yv * yv + zv * zv);

	res.x = mes / (4 * M_PI * r * r * r) * (p.x * (3 * xv * xv / (r * r) - 1) + p.y * (3 * xv * yv / (r * r)) + p.z * (3 * xv * zv / (r * r)));
	res.y = mes / (4 * M_PI * r * r * r) * (p.x * (3 * xv * yv / (r * r)) + p.y * (3 * yv * yv / (r * r) - 1) + p.z * (3 * yv * zv / (r * r)));
	res.z = mes / (4 * M_PI * r * r * r) * (p.x * (3 * xv * zv / (r * r)) + p.y * (3 * yv * zv / (r * r)) + p.z * (3 * zv * zv / (r * r) - 1));

	return res;
};