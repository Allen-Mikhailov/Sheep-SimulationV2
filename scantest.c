#include <windows.h>
#include <stdio.h>

int main()
{
    FILE *wfp = fopen("./test.sim", "w");
    fprintf(wfp, "1 2 3");
    fclose(wfp);

    int a, b, c;
    FILE *rfp = fopen("./test.sim", "r");
    fscanf(rfp, "%d %d %d", &a, &b, &c);
    fclose(rfp);

    printf("a: %d, b: %d, c: %d\n", a, b, c);
}   