; RUN: opt -passes=debug-count -S -disable-output %s | FileCheck %s
; CHECK: llvm.dbg.declare:3
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local noundef i32 @_Z4fiboPii(ptr noundef %arr, i32 noundef %n) #0 !dbg !10 {
entry:
  %arr.addr = alloca ptr, align 8
  %n.addr = alloca i32, align 4
  %i = alloca i32, align 4
  store ptr %arr, ptr %arr.addr, align 8
  call void @llvm.dbg.declare(metadata ptr %arr.addr, metadata !17, metadata !DIExpression()), !dbg !18
  store i32 %n, ptr %n.addr, align 4
  call void @llvm.dbg.declare(metadata ptr %n.addr, metadata !19, metadata !DIExpression()), !dbg !20
  %0 = load ptr, ptr %arr.addr, align 8, !dbg !21
  %arrayidx = getelementptr inbounds i32, ptr %0, i64 0, !dbg !21
  store i32 0, ptr %arrayidx, align 4, !dbg !22
  %1 = load ptr, ptr %arr.addr, align 8, !dbg !23
  %arrayidx1 = getelementptr inbounds i32, ptr %1, i64 1, !dbg !23
  store i32 1, ptr %arrayidx1, align 4, !dbg !24
  call void @llvm.dbg.declare(metadata ptr %i, metadata !25, metadata !DIExpression()), !dbg !27
  store i32 2, ptr %i, align 4, !dbg !27
  br label %for.cond, !dbg !28

for.cond:                                         ; preds = %for.inc, %entry
  %2 = load i32, ptr %i, align 4, !dbg !29
  %3 = load i32, ptr %n.addr, align 4, !dbg !31
  %cmp = icmp sle i32 %2, %3, !dbg !32
  br i1 %cmp, label %for.body, label %for.end, !dbg !33

for.body:                                         ; preds = %for.cond
  %4 = load ptr, ptr %arr.addr, align 8, !dbg !34
  %5 = load i32, ptr %i, align 4, !dbg !36
  %sub = sub nsw i32 %5, 2, !dbg !37
  %idxprom = sext i32 %sub to i64, !dbg !34
  %arrayidx2 = getelementptr inbounds i32, ptr %4, i64 %idxprom, !dbg !34
  %6 = load i32, ptr %arrayidx2, align 4, !dbg !34
  %7 = load ptr, ptr %arr.addr, align 8, !dbg !38
  %8 = load i32, ptr %i, align 4, !dbg !39
  %sub3 = sub nsw i32 %8, 1, !dbg !40
  %idxprom4 = sext i32 %sub3 to i64, !dbg !38
  %arrayidx5 = getelementptr inbounds i32, ptr %7, i64 %idxprom4, !dbg !38
  %9 = load i32, ptr %arrayidx5, align 4, !dbg !38
  %add = add nsw i32 %6, %9, !dbg !41
  %10 = load ptr, ptr %arr.addr, align 8, !dbg !42
  %11 = load i32, ptr %i, align 4, !dbg !43
  %idxprom6 = sext i32 %11 to i64, !dbg !42
  %arrayidx7 = getelementptr inbounds i32, ptr %10, i64 %idxprom6, !dbg !42
  store i32 %add, ptr %arrayidx7, align 4, !dbg !44
  br label %for.inc, !dbg !45

for.inc:                                          ; preds = %for.body
  %12 = load i32, ptr %i, align 4, !dbg !46
  %inc = add nsw i32 %12, 1, !dbg !46
  store i32 %inc, ptr %i, align 4, !dbg !46
  br label %for.cond, !dbg !47, !llvm.loop !48

for.end:                                          ; preds = %for.cond
  %13 = load ptr, ptr %arr.addr, align 8, !dbg !51
  %14 = load i32, ptr %n.addr, align 4, !dbg !52
  %sub8 = sub nsw i32 %14, 1, !dbg !53
  %idxprom9 = sext i32 %sub8 to i64, !dbg !51
  %arrayidx10 = getelementptr inbounds i32, ptr %13, i64 %idxprom9, !dbg !51
  %15 = load i32, ptr %arrayidx10, align 4, !dbg !51
  ret i32 %15, !dbg !54
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !1, producer: "clang version 16.0.0 (https://github.com/llvm/llvm-project.git 15151315f76b0840423bd3bd62c5f9fc647d31c6)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "/home/syrmia/kursevi/test/fibo.cpp", directory: "/home/syrmia/llvm-project/build", checksumkind: CSK_MD5, checksum: "a27f46439ba72546423ae26251ac1328")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"clang version 16.0.0 (https://github.com/llvm/llvm-project.git 15151315f76b0840423bd3bd62c5f9fc647d31c6)"}
!10 = distinct !DISubprogram(name: "fibo", linkageName: "_Z4fiboPii", scope: !11, file: !11, line: 1, type: !12, scopeLine: 1, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !16)
!11 = !DIFile(filename: "kursevi/test/fibo.cpp", directory: "/home/syrmia", checksumkind: CSK_MD5, checksum: "a27f46439ba72546423ae26251ac1328")
!12 = !DISubroutineType(types: !13)
!13 = !{!14, !15, !14}
!14 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!15 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!16 = !{}
!17 = !DILocalVariable(name: "arr", arg: 1, scope: !10, file: !11, line: 1, type: !15)
!18 = !DILocation(line: 1, column: 18, scope: !10)
!19 = !DILocalVariable(name: "n", arg: 2, scope: !10, file: !11, line: 1, type: !14)
!20 = !DILocation(line: 1, column: 27, scope: !10)
!21 = !DILocation(line: 2, column: 2, scope: !10)
!22 = !DILocation(line: 2, column: 9, scope: !10)
!23 = !DILocation(line: 3, column: 2, scope: !10)
!24 = !DILocation(line: 3, column: 9, scope: !10)
!25 = !DILocalVariable(name: "i", scope: !26, file: !11, line: 5, type: !14)
!26 = distinct !DILexicalBlock(scope: !10, file: !11, line: 5, column: 2)
!27 = !DILocation(line: 5, column: 10, scope: !26)
!28 = !DILocation(line: 5, column: 6, scope: !26)
!29 = !DILocation(line: 5, column: 17, scope: !30)
!30 = distinct !DILexicalBlock(scope: !26, file: !11, line: 5, column: 2)
!31 = !DILocation(line: 5, column: 22, scope: !30)
!32 = !DILocation(line: 5, column: 19, scope: !30)
!33 = !DILocation(line: 5, column: 2, scope: !26)
!34 = !DILocation(line: 6, column: 12, scope: !35)
!35 = distinct !DILexicalBlock(scope: !30, file: !11, line: 5, column: 30)
!36 = !DILocation(line: 6, column: 16, scope: !35)
!37 = !DILocation(line: 6, column: 17, scope: !35)
!38 = !DILocation(line: 6, column: 23, scope: !35)
!39 = !DILocation(line: 6, column: 27, scope: !35)
!40 = !DILocation(line: 6, column: 28, scope: !35)
!41 = !DILocation(line: 6, column: 21, scope: !35)
!42 = !DILocation(line: 6, column: 3, scope: !35)
!43 = !DILocation(line: 6, column: 7, scope: !35)
!44 = !DILocation(line: 6, column: 10, scope: !35)
!45 = !DILocation(line: 7, column: 2, scope: !35)
!46 = !DILocation(line: 5, column: 25, scope: !30)
!47 = !DILocation(line: 5, column: 2, scope: !30)
!48 = distinct !{!48, !33, !49, !50}
!49 = !DILocation(line: 7, column: 2, scope: !26)
!50 = !{!"llvm.loop.mustprogress"}
!51 = !DILocation(line: 9, column: 9, scope: !10)
!52 = !DILocation(line: 9, column: 13, scope: !10)
!53 = !DILocation(line: 9, column: 14, scope: !10)
!54 = !DILocation(line: 9, column: 2, scope: !10)
