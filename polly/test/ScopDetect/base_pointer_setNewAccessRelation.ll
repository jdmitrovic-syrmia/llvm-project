; RUN: opt %loadNPMPolly '-passes=print<polly-detect>,scop(polly-import-jscop,polly-codegen)' -disable-output < %s 2>&1 | FileCheck %s --allow-empty
;
; Polly codegen used to generate invalid code (referring to %ptr from the
; original region) when regeneration of the access function is necessary.
; The SCoP is now rejected as a whole because %ptr is not considered a valid
; base pointer.
;
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128"

declare ptr @getNextBasePtr(ptr) readnone nounwind

define void @base_pointer_is_inst_inside_invariant_1(i64 %n, ptr %A) {
entry:
  br label %for.i

for.i:
  %indvar.i = phi i64 [ %indvar.i.next, %for.i.inc ], [ 0, %entry ]
  br label %S1

S1:
  %ptr = call ptr @getNextBasePtr(ptr %A)
  %conv = sitofp i64 %indvar.i to float
  %arrayidx5 = getelementptr float, ptr %ptr, i64 %indvar.i
  store float %conv, ptr %arrayidx5, align 4
  br label %for.i.inc

for.i.inc:
  %indvar.i.next = add i64 %indvar.i, 1
  %exitcond.i = icmp ne i64 %indvar.i.next, %n
  br i1 %exitcond.i, label %for.i, label %exit

exit:
  ret void
}


; CHECK-NOT: Valid Region for Scop
