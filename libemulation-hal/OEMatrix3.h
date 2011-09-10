
/**
 * libemulation-hal
 * 3x3 matrix
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 3x3 matrix
 */

#ifndef _OEMATRIX3_H
#define _OEMATRIX3_H

#include <vector>

using namespace std;

class OEMatrix3
{
public:
	OEMatrix3();
	OEMatrix3(float c00, float c01, float c02,
              float c10, float c11, float c12,
              float c20, float c21, float c22);
	
	float getValue(unsigned int i, unsigned int j);
	float *getValues();
	
	OEMatrix3 operator*(float value);
	OEMatrix3 operator*(const OEMatrix3& m);
	OEMatrix3& operator*=(const float value);
	OEMatrix3& operator*=(const OEMatrix3& m);
	
private:
	vector<float> data;
};

#endif
