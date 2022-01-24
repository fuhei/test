Dwarf - Debugging With Attributed RecordFormats
================

* [主页](http://wiki.dwarfstd.org/index.php?title=Main_Page)

DWARF调试格式和ELF格式的关系是：DWARF是ELF最常用的调试信息格式，它不一定与ELF相关，但两者是一起发展的，因此在开发中常一起使用。

* DWARF格式中，高级语言的源文件、函数、变量、类型等调试信息在`.debug_info`节区中存储;
* 为了节省存储空间，DWARF在`.debug_abbrev`节区中定义了所有节点的类型和格式;
* `.debug_info`和`.debug_abbrev`节区中，每个节点表示一种调试信息，例如`DW_TAG_compile_unit`表示源文件调试信息;

例如：

```
$ gcc -g hello.c
$ readelf -S a.out | grep debug
  [27] .debug_aranges    PROGBITS         0000000000000000  00002c0c
  [28] .debug_info       PROGBITS         0000000000000000  00002c3c
  [29] .debug_abbrev     PROGBITS         0000000000000000  00002f66
  [30] .debug_line       PROGBITS         0000000000000000  00003047
  [31] .debug_str        PROGBITS         0000000000000000  0000313b
```
