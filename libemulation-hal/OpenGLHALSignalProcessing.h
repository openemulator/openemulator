
/**
 * libcomposite-hal
 * Signal processing functions
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements signal processing functions.
 */

#include <vector>

using namespace std;

class Vector
{
public:
	Vector();
	Vector(vector<float> data);
	
	float getValue(unsigned int i);
	
	Vector operator *(const float value);
	Vector operator *(const Vector& v);
	Vector normalize();
	Vector realIDFT();
	
	static Vector lanczosWindow(unsigned int n, float fc);
	static Vector chebyshevWindow(unsigned int n, float sidelobeDb);
	
private:
	vector<float> data;
};

class Matrix3
{
public:
	Matrix3();
	Matrix3(float c00, float c01, float c02,
			float c10, float c11, float c12,
			float c20, float c21, float c22);
	
	float getValue(unsigned int i, unsigned int j);
	
	Matrix3 operator*(float value);
	Matrix3 operator*(const Matrix3& m);
	Matrix3& operator*=(const float value);
	Matrix3& operator*=(const Matrix3& m);
	
private:
	vector<float> data;
};
