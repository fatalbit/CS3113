#include "math_internal.h"
float lerp(float v0, float v1, float t){
	return (1.0f - t) * v0 + (t * v1);
}

float map_value(float value, float srcMin, float srcMax, float dstMin, float dstMax){
	float retVal = dstMin + ((value - srcMin) / (srcMax - srcMin) * (dstMax - dstMin));
	if (retVal < dstMin){
		retVal = dstMin;
	}
	if (retVal > dstMax){
		retVal = dstMax;
	}
	return retVal;
}