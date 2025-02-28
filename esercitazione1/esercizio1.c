#include <stdio.h>

int main()
{
#ifdef TEST
    printf("Testing mode\n");
#endif
    printf("Running...\n");
    return 0;
}