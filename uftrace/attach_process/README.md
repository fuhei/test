Attach Process
=================

# Content

1. user space ftrace
2. user space patch

# 分析

* call 指令

```
e8 b5 ff ff ff       	callq  40120a <func2>
```



# libcareplus

补丁前

```
Dump of assembler code for function print_hello:
   0x0000000000401136 <+0>:	push   %rbp
   0x0000000000401137 <+1>:	mov    %rsp,%rbp
   0x000000000040113a <+4>:	mov    $0x402010,%edi
   0x000000000040113f <+9>:	call   0x401030 <puts@plt>
   0x0000000000401144 <+14>:	nop
   0x0000000000401145 <+15>:	pop    %rbp
   0x0000000000401146 <+16>:	ret
```

* 补丁后

```
Dump of assembler code for function print_hello:
   0x0000000000401136 <+0>:	jmp    0x40648c
   0x000000000040113b <+5>:	adc    %ah,(%rax)
   0x000000000040113d <+7>:	add    %bpl,%al
   0x0000000000401140 <+10>:	in     (%dx),%al
   0x0000000000401141 <+11>:	(bad)
   0x0000000000401142 <+12>:	(bad)
   0x0000000000401143 <+13>:	call   *0x4855c35d(%rax)
```

# Links

* [libcareplus](https://github.com/rtoax/libcareplus)
* 开发初始发布版本后，将同步到仓库 [upatch](https://github.com/Rtoax/upatch)
