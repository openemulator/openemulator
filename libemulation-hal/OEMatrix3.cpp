
/**
 * libcomposite-hal
 * Signal processing functions
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements signal processing functions.
 */

#include <math.h>

#include "OEMatrix3.h"

OEMatrix3::OEMatrix3()
{
	data.resize(9);
}

OEMatrix3::OEMatrix3(float c00, float c01, float c02,
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

float OEMatrix3::getValue(unsigned int i, unsigned int j)
{
	return data[3 * i + j];
}

float *OEMatrix3::getValues()
{
	return &data.front();
}

OEMatrix3 OEMatrix3::operator*(const float value)
{
	OEMatrix3 m;
	
	for (int i = 0; i < 9; i++)
		m.data[i] = data[i] * value;
	
	return m;
}

OEMatrix3 OEMatrix3::operator*(const OEMatrix3& m)
{
	OEMatrix3 n;
	
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				n.data[i * 3 + j] += m.data[i * 3 + k] * data[k * 3 + j];
	
	return n;
}

OEMatrix3& OEMatrix3::operator*=(const float value)
{
	OEMatrix3 m = *this * value;
	
	data = m.data;
	
	return *this;
}

OEMatrix3& OEMatrix3::operator*=(const OEMatrix3& m)
{
	OEMatrix3 n = *this * m;
	
	data = n.data;
	
	return *this;
}
