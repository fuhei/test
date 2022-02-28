输出内容 gcc --verbose main.c -flto=auto -o main.1.flto.out
============================

# 开始编译

```
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/libexec/gcc/x86_64-redhat-linux/11/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-redhat-linux
Configured with: ../configure --enable-bootstrap --enable-host-pie --enable-host-bind-now --enable-languages=c,c++,fortran,lto --prefix=/usr --mandir=/usr/share/man --infodir=/usr/share/info --with-bugurl=http://bugzilla.redhat.com/bugzilla --enable-shared --enable-threads=posix --enable-checking=release --enable-multilib --with-system-zlib --enable-__cxa_atexit --disable-libunwind-exceptions --enable-gnu-unique-object --enable-linker-build-id --with-gcc-major-version-only --with-linker-hash-style=gnu --enable-plugin --enable-initfini-array --without-isl --enable-offload-targets=nvptx-none --without-cuda-driver --enable-gnu-indirect-function --enable-cet --with-tune=generic --with-arch_64=x86-64-v2 --with-arch_32=x86-64 --build=x86_64-redhat-linux --with-build-config=bootstrap-lto --enable-link-serialization=1
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 11.2.1 20220127 (Red Hat 11.2.1-9) (GCC) 
```

# 1. CC1

```
COLLECT_GCC_OPTIONS='-v' '-flto=auto' '-o' 'main.1.flto.out' '-mtune=generic' '-march=x86-64-v2' '-dumpdir' 'main.1.flto.out-'
 /usr/libexec/gcc/x86_64-redhat-linux/11/cc1 -quiet -v main.c -quiet -dumpdir main.1.flto.out- -dumpbase main.c -dumpbase-ext .c -mtune=generic -march=x86-64-v2 -version -flto=auto -o /tmp/ccVSy8GQ.s
GNU C17 (GCC) version 11.2.1 20220127 (Red Hat 11.2.1-9) (x86_64-redhat-linux)
	compiled by GNU C version 11.2.1 20220127 (Red Hat 11.2.1-9), GMP version 6.2.0, MPFR version 4.1.0-p9, MPC version 1.2.1, isl version none
GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
ignoring nonexistent directory "/usr/lib/gcc/x86_64-redhat-linux/11/include-fixed"
ignoring nonexistent directory "/usr/lib/gcc/x86_64-redhat-linux/11/../../../../x86_64-redhat-linux/include"
#include "..." search starts here:
#include <...> search starts here:
 /usr/lib/gcc/x86_64-redhat-linux/11/include
 /usr/local/include
 /usr/include
End of search list.
GNU C17 (GCC) version 11.2.1 20220127 (Red Hat 11.2.1-9) (x86_64-redhat-linux)
	compiled by GNU C version 11.2.1 20220127 (Red Hat 11.2.1-9), GMP version 6.2.0, MPFR version 4.1.0-p9, MPC version 1.2.1, isl version none
GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
Compiler executable checksum: be7e5758ba853e3e21856ca6cb72ba20
```

# 2. 汇编

```
COLLECT_GCC_OPTIONS='-v' '-flto=auto' '-o' 'main.1.flto.out' '-mtune=generic' '-march=x86-64-v2' '-dumpdir' 'main.1.flto.out-'
 as -v --64 -o /tmp/cc2ZdVgx.o /tmp/ccVSy8GQ.s
GNU assembler version 2.35.2 (x86_64-redhat-linux) using BFD version version 2.35.2-17.el9
COMPILER_PATH=/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/
LIBRARY_PATH=/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/:/lib/../lib64/:/usr/lib/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../:/lib/:/usr/lib/
COLLECT_GCC_OPTIONS='-v' '-flto=auto' '-o' 'main.1.flto.out' '-mtune=generic' '-march=x86-64-v2' '-dumpdir' 'main.1.flto.out.'
```

# 3

```
/usr/libexec/gcc/x86_64-redhat-linux/11/collect2 -plugin /usr/libexec/gcc/x86_64-redhat-linux/11/liblto_plugin.so -plugin-opt=/usr/libexec/gcc/x86_64-redhat-linux/11/lto-wrapper -plugin-opt=-fresolution=/tmp/cc6fagmd.res -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lc -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s -flto=auto --build-id --no-add-needed --eh-frame-hdr --hash-style=gnu -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o main.1.flto.out /usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/crt1.o /usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/crti.o /usr/lib/gcc/x86_64-redhat-linux/11/crtbegin.o -L/usr/lib/gcc/x86_64-redhat-linux/11 -L/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 -L/usr/lib/gcc/x86_64-redhat-linux/11/../../.. /tmp/cc2ZdVgx.o -lgcc --push-state --as-needed -lgcc_s --pop-state -lc -lgcc --push-state --as-needed -lgcc_s --pop-state /usr/lib/gcc/x86_64-redhat-linux/11/crtend.o /usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/crtn.o
/usr/libexec/gcc/x86_64-redhat-linux/11/lto-wrapper -fresolution=/tmp/cc6fagmd.res -flinker-output=exec /tmp/cc2ZdVgx.o 
/usr/libexec/gcc/x86_64-redhat-linux/11/lto-wrapper -fresolution=/tmp/cc6fagmd.res -flinker-output=exec /tmp/cc2ZdVgx.o 
LTO parallelism level set to 12
gcc @/tmp/ccRhESQn
```

```
Using built-in specs.
COLLECT_GCC=gcc
OFFLOAD_TARGET_NAMES=nvptx-none
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-redhat-linux
Configured with: ../configure --enable-bootstrap --enable-host-pie --enable-host-bind-now --enable-languages=c,c++,fortran,lto --prefix=/usr --mandir=/usr/share/man --infodir=/usr/share/info --with-bugurl=http://bugzilla.redhat.com/bugzilla --enable-shared --enable-threads=posix --enable-checking=release --enable-multilib --with-system-zlib --enable-__cxa_atexit --disable-libunwind-exceptions --enable-gnu-unique-object --enable-linker-build-id --with-gcc-major-version-only --with-linker-hash-style=gnu --enable-plugin --enable-initfini-array --without-isl --enable-offload-targets=nvptx-none --without-cuda-driver --enable-gnu-indirect-function --enable-cet --with-tune=generic --with-arch_64=x86-64-v2 --with-arch_32=x86-64 --build=x86_64-redhat-linux --with-build-config=bootstrap-lto --enable-link-serialization=1
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 11.2.1 20220127 (Red Hat 11.2.1-9) (GCC) 
COLLECT_GCC_OPTIONS='-c' '-fno-openmp' '-fno-openacc' '-fno-pie' '-fcf-protection=none' '-v' '-mtune=generic' '-march=x86-64-v2'   '-fltrans-output-list=/tmp/ccuAGiy7.ltrans.out' '-fwpa=12' '-fresolution=/tmp/cc6fagmd.res' '-flinker-output=exec'
```

# 4

```
/usr/libexec/gcc/x86_64-redhat-linux/11/lto1 -quiet -dumpbase ./main.1.flto.out.wpa -mtune=generic -march=x86-64-v2 -version -fno-openmp -fno-openacc -fno-pie -fcf-protection=none -fltrans-output-list=/tmp/ccuAGiy7.ltrans.out -fwpa=12 -fresolution=/tmp/cc6fagmd.res -flinker-output=exec @/tmp/ccmMlxKT
GNU GIMPLE (GCC) version 11.2.1 20220127 (Red Hat 11.2.1-9) (x86_64-redhat-linux)
	compiled by GNU C version 11.2.1 20220127 (Red Hat 11.2.1-9), GMP version 6.2.0, MPFR version 4.1.0-p9, MPC version 1.2.1, isl version none
GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
GNU GIMPLE (GCC) version 11.2.1 20220127 (Red Hat 11.2.1-9) (x86_64-redhat-linux)
	compiled by GNU C version 11.2.1 20220127 (Red Hat 11.2.1-9), GMP version 6.2.0, MPFR version 4.1.0-p9, MPC version 1.2.1, isl version none
GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
COMPILER_PATH=/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/:/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/
LIBRARY_PATH=/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/../lib64/:/lib/../lib64/../lib64/:/usr/lib/../lib64/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/:/lib/../lib64/:/usr/lib/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/:/lib/../lib64/:/usr/lib/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/:/lib/../lib64/:/usr/lib/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../:/lib/:/usr/lib/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../:/lib/:/usr/lib/
COLLECT_GCC_OPTIONS='-c' '-fno-openmp' '-fno-openacc' '-fno-pie' '-fcf-protection=none' '-v' '-mtune=generic' '-march=x86-64-v2'   '-fltrans-output-list=/tmp/ccuAGiy7.ltrans.out' '-fwpa=12' '-fresolution=/tmp/cc6fagmd.res' '-flinker-output=exec' '-dumpdir' './main.1.flto.out.wpa.'
make -f /tmp/ccUkYkO4.mk -j12 all
Using built-in specs.
COLLECT_GCC=gcc
OFFLOAD_TARGET_NAMES=nvptx-none
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-redhat-linux
Configured with: ../configure --enable-bootstrap --enable-host-pie --enable-host-bind-now --enable-languages=c,c++,fortran,lto --prefix=/usr --mandir=/usr/share/man --infodir=/usr/share/info --with-bugurl=http://bugzilla.redhat.com/bugzilla --enable-shared --enable-threads=posix --enable-checking=release --enable-multilib --with-system-zlib --enable-__cxa_atexit --disable-libunwind-exceptions --enable-gnu-unique-object --enable-linker-build-id --with-gcc-major-version-only --with-linker-hash-style=gnu --enable-plugin --enable-initfini-array --without-isl --enable-offload-targets=nvptx-none --without-cuda-driver --enable-gnu-indirect-function --enable-cet --with-tune=generic --with-arch_64=x86-64-v2 --with-arch_32=x86-64 --build=x86_64-redhat-linux --with-build-config=bootstrap-lto --enable-link-serialization=1
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 11.2.1 20220127 (Red Hat 11.2.1-9) (GCC) 
COLLECT_GCC_OPTIONS='-c' '-fno-openmp' '-fno-openacc' '-fno-pie' '-fcf-protection=none' '-v' '-mtune=generic' '-march=x86-64-v2'   '-fltrans' '-o' '/tmp/ccuAGiy7.ltrans0.ltrans.o'
```

# 5

```
 /usr/libexec/gcc/x86_64-redhat-linux/11/lto1 -quiet -dumpbase ./main.1.flto.out.ltrans0.ltrans -mtune=generic -march=x86-64-v2 -version -fno-openmp -fno-openacc -fno-pie -fcf-protection=none -fltrans /tmp/ccuAGiy7.ltrans0.o -o /tmp/cclkJB9v.s
GNU GIMPLE (GCC) version 11.2.1 20220127 (Red Hat 11.2.1-9) (x86_64-redhat-linux)
	compiled by GNU C version 11.2.1 20220127 (Red Hat 11.2.1-9), GMP version 6.2.0, MPFR version 4.1.0-p9, MPC version 1.2.1, isl version none
GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
GNU GIMPLE (GCC) version 11.2.1 20220127 (Red Hat 11.2.1-9) (x86_64-redhat-linux)
	compiled by GNU C version 11.2.1 20220127 (Red Hat 11.2.1-9), GMP version 6.2.0, MPFR version 4.1.0-p9, MPC version 1.2.1, isl version none
GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
COLLECT_GCC_OPTIONS='-c' '-fno-openmp' '-fno-openacc' '-fno-pie' '-fcf-protection=none' '-v' '-mtune=generic' '-march=x86-64-v2'   '-fltrans' '-o' '/tmp/ccuAGiy7.ltrans0.ltrans.o'
```

# 6

```
 as -v --64 -o /tmp/ccuAGiy7.ltrans0.ltrans.o /tmp/cclkJB9v.s
GNU assembler version 2.35.2 (x86_64-redhat-linux) using BFD version version 2.35.2-17.el9
MAKEFLAGS= -j12 
COMPILER_PATH=/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/:/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/11/:/usr/libexec/gcc/x86_64-redhat-linux/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/
LIBRARY_PATH=/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/../lib64/:/lib/../lib64/../lib64/:/usr/lib/../lib64/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/:/lib/../lib64/:/usr/lib/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/:/lib/../lib64/:/usr/lib/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../../lib64/:/lib/../lib64/:/usr/lib/../lib64/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../:/lib/:/usr/lib/:/usr/lib/gcc/x86_64-redhat-linux/11/../../../:/lib/:/usr/lib/
COLLECT_GCC_OPTIONS='-c' '-fno-openmp' '-fno-openacc' '-fno-pie' '-fcf-protection=none' '-v' '-mtune=generic' '-march=x86-64-v2'   '-fltrans' '-o' '/tmp/ccuAGiy7.ltrans0.ltrans.o' '-dumpdir' './main.1.flto.out.ltrans0.ltrans.'
COLLECT_GCC_OPTIONS='-v' '-flto=auto' '-o' 'main.1.flto.out' '-mtune=generic' '-march=x86-64-v2' '-dumpdir' 'main.1.flto.out.'
```


