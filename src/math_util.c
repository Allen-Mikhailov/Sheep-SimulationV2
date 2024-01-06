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

int rawBinarySearch(int arr[], int l, int r, int x)
{
    while (l <= r) {
        int m = (l + r) / 2;
        if (arr[m] == x)
            return m;
        
        if (arr[m] < x)
            l = m + 1;
        else
            r = m - 1;
    }
    return -1;
}

int binarySearch(int arr[], int length, int target)
{
    return rawBinarySearch(arr, 0, length-1, target);
}

#define COMPARE_ANGLES(a1, a2) fmod(fabs(a1 - a2) + M_PI, M_PI*2) - M_PI;