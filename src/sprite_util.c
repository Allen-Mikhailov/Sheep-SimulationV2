#include <windows.h>

HBITMAP createCompatibleBitmap(HDC hdc, int width, int height)
{
    // Get the number of color planes
    int nPlanes = GetDeviceCaps(hdc, PLANES);

    // Get the number of bits per pixel
    int nBitCount = GetDeviceCaps(hdc, BITSPIXEL);

    printf("nPlanes: %d, nBitCount %d\n", nPlanes, nBitCount);

    const void* lpBits = malloc((((width * nPlanes * nBitCount + 15) >> 4) << 1) * height);
    return CreateBitmap(width, height, nPlanes, nBitCount, lpBits);
}