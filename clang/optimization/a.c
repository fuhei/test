#include <stdio.h>

int puts(const char *str) {
    fputs("Hello, world!\n", stdout);
    return 1;
}

int main() {
    printf("Goodbye!\n");
    return 0;
}
