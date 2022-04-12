LLVM - Low Level Virtual Machine
================================

# 介绍

LLVM后端支持的平台很多，我们不需要担心CPU、操作系统的问题（运行库除外）
LLVM后端的优化水平较高，我们只需要将代码编译成LLVM IR，就可以由LLVM后端作相应的优化
LLVM IR本身比较贴近汇编语言，同时也提供了许多ABI层面的定制化功能

# test.c

```c
int main() {
    return 0;
}
```

```bash
# 输出我们test.c经过编译器前端的预处理、语法分析、语义分析之后，生成的抽象语法树（AST）
$ clang -Xclang -ast-dump -fsyntax-only test.c
TranslationUnitDecl 0x55cea79ef808 <<invalid sloc>> <invalid sloc>
|-TypedefDecl 0x55cea79f00d0 <<invalid sloc>> <invalid sloc> implicit __int128_t '__int128'
| `-BuiltinType 0x55cea79efdb0 '__int128'
|-TypedefDecl 0x55cea79f0140 <<invalid sloc>> <invalid sloc> implicit __uint128_t 'unsigned __int128'
| `-BuiltinType 0x55cea79efdd0 'unsigned __int128'
|-TypedefDecl 0x55cea79f0448 <<invalid sloc>> <invalid sloc> implicit __NSConstantString 'struct __NSConstantString_tag'
| `-RecordType 0x55cea79f0220 'struct __NSConstantString_tag'
|   `-Record 0x55cea79f0198 '__NSConstantString_tag'
|-TypedefDecl 0x55cea79f04e0 <<invalid sloc>> <invalid sloc> implicit __builtin_ms_va_list 'char *'
| `-PointerType 0x55cea79f04a0 'char *'
|   `-BuiltinType 0x55cea79ef8b0 'char'
|-TypedefDecl 0x55cea7a30f20 <<invalid sloc>> <invalid sloc> implicit __builtin_va_list 'struct __va_list_tag [1]'
| `-ConstantArrayType 0x55cea79f0780 'struct __va_list_tag [1]' 1 
|   `-RecordType 0x55cea79f05c0 'struct __va_list_tag'
|     `-Record 0x55cea79f0538 '__va_list_tag'
`-FunctionDecl 0x55cea7a30fd0 <test.c:1:1, line:4:1> line:1:5 main 'int ()'
  `-CompoundStmt 0x55cea7a310e8 <line:2:1, line:4:1>
    `-ReturnStmt 0x55cea7a310d8 <line:3:5, col:12>
      `-IntegerLiteral 0x55cea7a310b8 <col:12> 'int' 0
```

# 前端生成中间代码

```bash
$ clang -S -emit-llvm test.c
```
