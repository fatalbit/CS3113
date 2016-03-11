#include "math_internal.h"
float lerp(float v0, float v1, float t){
	return (1.0f - t) * v0 + (t * v1);
}