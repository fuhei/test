#!/bin/bash 

cstool x64 "90 91"
# 0  90                                               nop	
# 1  91                                               xchg	eax, ecx

cstool -u x64 "488d 2551 3f60 01e8 d400 0000 488d 3ded ffff ff56 e8d7 0100 005e 4805 0060 f203 eb20 0f1f 4000 662e 0f1f 8400 0000 0000 e8ab 0000 0056 e8c5 1f00 005e 4805 00a0"
#  0  90                                               nop	
#  1  91                                               xchg	eax, ecx
#  0  48 8d 25 51 3f 60 01                             lea	rsp, [rip + 0x1603f51]
#  7  e8 d4 00 00 00                                   call	0xe0
#  c  48 8d 3d ed ff ff ff                             lea	rdi, [rip - 0x13]
# 13  56                                               push	rsi
# 14  e8 d7 01 00 00                                   call	0x1f0
# 19  5e                                               pop	rsi
# 1a  48 05 00 60 f2 03                                add	rax, 0x3f26000
# 20  eb 20                                            jmp	0x42
# 22  0f 1f 40 00                                      nop	dword ptr [rax]
# 26  66 2e 0f 1f 84 00 00 00 00 00                    nop	word ptr cs:[rax + rax]
# 30  e8 ab 00 00 00                                   call	0xe0
# 35  56                                               push	rsi
# 36  e8 c5 1f 00 00                                   call	0x2000
# 3b  5e                                               pop	rsi
