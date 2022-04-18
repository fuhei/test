; ModuleID = 'showdebug.c'
source_filename = "showdebug.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @compute_factorial(i32 %0) #0 !dbg !9 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !13, metadata !DIExpression()), !dbg !14
  %5 = load i32, i32* %3, align 4, !dbg !15
  %6 = icmp sle i32 %5, 1, !dbg !17
  br i1 %6, label %7, label %8, !dbg !18

7:                                                ; preds = %1
  store i32 1, i32* %2, align 4, !dbg !19
  br label %20, !dbg !19

8:                                                ; preds = %1
  call void @llvm.dbg.declare(metadata i32* %4, metadata !20, metadata !DIExpression()), !dbg !21
  %9 = load i32, i32* %3, align 4, !dbg !22
  store i32 %9, i32* %4, align 4, !dbg !21
  br label %10, !dbg !23

10:                                               ; preds = %14, %8
  %11 = load i32, i32* %3, align 4, !dbg !24
  %12 = add nsw i32 %11, -1, !dbg !24
  store i32 %12, i32* %3, align 4, !dbg !24
  %13 = icmp sgt i32 %12, 1, !dbg !25
  br i1 %13, label %14, label %18, !dbg !23

14:                                               ; preds = %10
  %15 = load i32, i32* %3, align 4, !dbg !26
  %16 = load i32, i32* %4, align 4, !dbg !27
  %17 = mul nsw i32 %16, %15, !dbg !27
  store i32 %17, i32* %4, align 4, !dbg !27
  br label %10, !dbg !23, !llvm.loop !28

18:                                               ; preds = %10
  %19 = load i32, i32* %4, align 4, !dbg !30
  store i32 %19, i32* %2, align 4, !dbg !31
  br label %20, !dbg !31

20:                                               ; preds = %18, %7
  %21 = load i32, i32* %2, align 4, !dbg !32
  ret i32 %21, !dbg !32
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32 %0, i8** %1) #0 !dbg !33 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i8, align 1
  %7 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !39, metadata !DIExpression()), !dbg !40
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !41, metadata !DIExpression()), !dbg !42
  %8 = load i32, i32* %4, align 4, !dbg !43
  %9 = icmp slt i32 %8, 2, !dbg !45
  br i1 %9, label %10, label %11, !dbg !46

10:                                               ; preds = %2
  store i32 -1, i32* %3, align 4, !dbg !47
  br label %22, !dbg !47

11:                                               ; preds = %2
  call void @llvm.dbg.declare(metadata i8* %6, metadata !48, metadata !DIExpression()), !dbg !49
  %12 = load i8**, i8*** %5, align 8, !dbg !50
  %13 = getelementptr inbounds i8*, i8** %12, i64 1, !dbg !50
  %14 = load i8*, i8** %13, align 8, !dbg !50
  %15 = getelementptr inbounds i8, i8* %14, i64 0, !dbg !50
  %16 = load i8, i8* %15, align 1, !dbg !50
  store i8 %16, i8* %6, align 1, !dbg !49
  call void @llvm.dbg.declare(metadata i32* %7, metadata !51, metadata !DIExpression()), !dbg !52
  %17 = load i8, i8* %6, align 1, !dbg !53
  %18 = sext i8 %17 to i32, !dbg !53
  %19 = sub nsw i32 %18, 48, !dbg !54
  %20 = call i32 @compute_factorial(i32 %19), !dbg !55
  store i32 %20, i32* %7, align 4, !dbg !52
  %21 = load i32, i32* %7, align 4, !dbg !56
  store i32 %21, i32* %3, align 4, !dbg !57
  br label %22, !dbg !57

22:                                               ; preds = %11, %10
  %23 = load i32, i32* %3, align 4, !dbg !58
  ret i32 %23, !dbg !58
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 13.0.1 (Red Hat 13.0.1-1.el9)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "showdebug.c", directory: "/home/rongtao/Git/test/llvm/jit/debugging")
!2 = !{}
!3 = !{i32 7, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{i32 7, !"frame-pointer", i32 2}
!8 = !{!"clang version 13.0.1 (Red Hat 13.0.1-1.el9)"}
!9 = distinct !DISubprogram(name: "compute_factorial", scope: !1, file: !1, line: 6, type: !10, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!10 = !DISubroutineType(types: !11)
!11 = !{!12, !12}
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !DILocalVariable(name: "n", arg: 1, scope: !9, file: !1, line: 6, type: !12)
!14 = !DILocation(line: 6, column: 27, scope: !9)
!15 = !DILocation(line: 8, column: 9, scope: !16)
!16 = distinct !DILexicalBlock(scope: !9, file: !1, line: 8, column: 9)
!17 = !DILocation(line: 8, column: 11, scope: !16)
!18 = !DILocation(line: 8, column: 9, scope: !9)
!19 = !DILocation(line: 9, column: 9, scope: !16)
!20 = !DILocalVariable(name: "f", scope: !9, file: !1, line: 11, type: !12)
!21 = !DILocation(line: 11, column: 9, scope: !9)
!22 = !DILocation(line: 11, column: 13, scope: !9)
!23 = !DILocation(line: 12, column: 5, scope: !9)
!24 = !DILocation(line: 12, column: 12, scope: !9)
!25 = !DILocation(line: 12, column: 16, scope: !9)
!26 = !DILocation(line: 13, column: 14, scope: !9)
!27 = !DILocation(line: 13, column: 11, scope: !9)
!28 = distinct !{!28, !23, !26, !29}
!29 = !{!"llvm.loop.mustprogress"}
!30 = !DILocation(line: 14, column: 12, scope: !9)
!31 = !DILocation(line: 14, column: 5, scope: !9)
!32 = !DILocation(line: 15, column: 1, scope: !9)
!33 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 17, type: !34, scopeLine: 18, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!34 = !DISubroutineType(types: !35)
!35 = !{!12, !12, !36}
!36 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !37, size: 64)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !38, size: 64)
!38 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!39 = !DILocalVariable(name: "argc", arg: 1, scope: !33, file: !1, line: 17, type: !12)
!40 = !DILocation(line: 17, column: 14, scope: !33)
!41 = !DILocalVariable(name: "argv", arg: 2, scope: !33, file: !1, line: 17, type: !36)
!42 = !DILocation(line: 17, column: 27, scope: !33)
!43 = !DILocation(line: 19, column: 9, scope: !44)
!44 = distinct !DILexicalBlock(scope: !33, file: !1, line: 19, column: 9)
!45 = !DILocation(line: 19, column: 14, scope: !44)
!46 = !DILocation(line: 19, column: 9, scope: !33)
!47 = !DILocation(line: 20, column: 9, scope: !44)
!48 = !DILocalVariable(name: "firstletter", scope: !33, file: !1, line: 21, type: !38)
!49 = !DILocation(line: 21, column: 10, scope: !33)
!50 = !DILocation(line: 21, column: 24, scope: !33)
!51 = !DILocalVariable(name: "result", scope: !33, file: !1, line: 22, type: !12)
!52 = !DILocation(line: 22, column: 9, scope: !33)
!53 = !DILocation(line: 22, column: 36, scope: !33)
!54 = !DILocation(line: 22, column: 48, scope: !33)
!55 = !DILocation(line: 22, column: 18, scope: !33)
!56 = !DILocation(line: 25, column: 12, scope: !33)
!57 = !DILocation(line: 25, column: 5, scope: !33)
!58 = !DILocation(line: 26, column: 1, scope: !33)
