#include <windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

float random()
{
    return (float) rand() / RAND_MAX;
}

float atanRange(float a)
{
    // printf("Inoput: %f\n", a);
    a = fmod(a, M_PI*2);
    // printf("FMOD: %f\n", a);
    if (a > M_PI)
        return a - M_PI*2;
    if (a < -M_PI)
        return a + M_PI*2;

    return a;
}

float clamp(float d, float min, float max) {
  const float t = d < min ? min : d;
  return t > max ? max : t;
}

#define COMPARE_ANGLES(a1, a2) fmod(fabs(a1 - a2) + M_PI, M_PI*2) - M_PI;