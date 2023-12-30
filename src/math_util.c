#include <windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

double random()
{
    return (double) rand() / RAND_MAX;
}

double atanRange(double a)
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

double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
}

double compare_angles(double a1, double a2)
{
    double dif = a2-a1;

    if(dif < -M_PI)
        dif += M_PI*2;
    if(dif > M_PI)
        dif -= M_PI*2;
    return dif;
}