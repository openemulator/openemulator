
/**
 * libcomposite-hal
 * Signal processing functions
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements signal processing functions.
 */

#include <math.h>

#include "OpenGLHALSignalProcessing.h"

// Vector processing

Vector::Vector()
{
}

Vector::Vector(vector<float> data)
{
	this->data = data;
}

float Vector::getValue(unsigned int i)
{
	return data[i];
}

Vector Vector::operator *(const float value)
{
	Vector w(data);
	
	for (int i = 0; i < data.size(); i++)
		w.data[i] *= value;
	
	return w;
}

Vector Vector::operator *(const Vector& v)
{
	if (v.data.size() != data.size())
		return Vector();
	
	Vector w(data);
	
	for (int i = 0; i < data.size(); i++)
		w.data[i] *= v.data[i];
	
	return w;
}

Vector Vector::normalize()
{
	float sum = 0.0;
	
	for (int i = 0; i < data.size(); i++)
		sum += data[i];
	
	Vector w(data);
	float gain = 1.0 / sum;
	
	for (int i = 0; i < data.size(); i++)
		w.data[i] *= gain;
	
	return w;
}

Vector Vector::realIDFT()
{
	Vector w;
	w.data.resize(data.size());
	
	for (int i = 0; i < data.size(); i++)
	{
		float omega = 2.0 * M_PI * i / data.size();
		
		for (int j = 0; j < data.size(); j++)
			w.data[i] += data[j] * cosf(j * omega);
	}
	
	for (int i = 0; i < data.size(); i++)
		w.data[i] /= data.size();
	
	return w;
}

Vector Vector::lanczosWindow(unsigned int n, float fc)
{
	Vector v;
	v.data.resize(n);
	
	int n2 = n / 2;
	
	for (int i = 0; i < n; i++)
	{
		float x = (i - n2) * fc;
		
		if (x == 0.0F)
			v.data[i] = 1.0F;
		else
			v.data[i] = sinf(M_PI * x) / (M_PI * x);
	}
	
	return v;
}

//
// Based on ideas at:
// http://www.dsprelated.com/showarticle/42.php
//
Vector Vector::chebyshevWindow(unsigned int n, float sidelobeDb)
{
	int m = n - 1;
	
	Vector w;
	w.data.resize(m);
	
	float alpha = coshf(acoshf(powf(10, sidelobeDb / 20.0)) / m);
	
	for (int i = 0; i < m; i++)
	{
		float a = fabsf(alpha * cosf(M_PI * i / m));
		if (a > 1)
			w.data[i] = powf(-1, i) * coshf(m * acosh(a));
		else
			w.data[i] = powf(-1, i) * cosf(m * acos(a));
	}
	
	w = w.realIDFT();
	
	w.data.resize(n);
	w.data[0] /= 2.0;
	w.data[n - 1] = w.data[0];
	
	float max = 0.0;
	
	for (int i = 0; i < n; i++)
		if (fabs(w.data[i]) > max)
			max = fabs(w.data[i]);
	
	for (int i = 0; i < n; i++)
		w.data[i] /= max;
	
	return w;
}

// Matrix processing

Matrix3::Matrix3()
{
	data.resize(9);
}

Matrix3::Matrix3(float c00, float c01, float c02,
				 float c10, float c11, float c12,
				 float c20, float c21, float c22)
{
	data.resize(9);
	
	data[0] = c00;
	data[1] = c01;
	data[2] = c02;
	data[3] = c10;
	data[4] = c11;
	data[5] = c12;
	data[6] = c20;
	data[7] = c21;
	data[8] = c22;
}

float Matrix3::getValue(unsigned int i, unsigned int j)
{
	return data[3 * i + j];
}

Matrix3 Matrix3::operator*(const float value)
{
	Matrix3 m;
	
	for (int i = 0; i < 9; i++)
		m.data[i] = data[i] * value;
	
	return m;
}

Matrix3 Matrix3::operator*(const Matrix3& m)
{
	Matrix3 n;
	
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				n.data[i * 3 + j] += m.data[i * 3 + k] * data[k * 3 + j];
	
	return n;
}


Matrix3& Matrix3::operator*=(const float value)
{
	Matrix3 m = *this * value;
	
	data = m.data;
	
	return *this;
}

Matrix3& Matrix3::operator*=(const Matrix3& m)
{
	Matrix3 n = *this * m;
	
	data = n.data;
	
	return *this;
}
