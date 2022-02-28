


在文章[为什么不总是使用编译器优化？](https://stackoverflow.com/questions/7857601/why-not-always-use-compiler-optimization)中说到这个示例:

```
#include <stdio.h>

int puts(const char *str) {
    fputs("Hello, world!\n", stdout);
    return 1;
}

int main() {
    printf("Goodbye!\n");
    return 0;
}
```


```
$ clang a.c
$ ./a.out
Goodbye!
```

```
$ clang a.c -O3
$ ./a.out
Hello, world!
```
