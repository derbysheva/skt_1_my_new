#pragma once
struct vector3
{
	double x, y, z;
	vector3(double x, double y, double z) : x(x), y(y), z(z) {};
	vector3() { x = 0; y = 0; z = 0; };

	vector3 operator+ (vector3 secondvector3)
	{
		return vector3(this->x + secondvector3.x, this->y + secondvector3.y, this->z + secondvector3.z);
	}
	vector3 operator- (vector3 secondvector3)
	{
		return vector3(this->x - secondvector3.x, this->y - secondvector3.y, this->z - secondvector3.z);
	}
	double operator* (vector3 secondvector3)
	{
		return double(this->x * secondvector3.x + this->y * secondvector3.y + this->z * secondvector3.z);
	}
	vector3 operator* (int number)
	{
		return vector3(this->x * number, this->y * number, this->z * number);
	}
};