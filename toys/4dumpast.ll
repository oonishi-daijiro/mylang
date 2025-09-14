; ModuleID = './4dumpast.cpp'
source_filename = "./4dumpast.cpp"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

; Function Attrs: mustprogress noinline norecurse nounwind optnone uwtable
define dso_local noundef i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 100, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 100
  br i1 %4, label %5, label %16

5:                                                ; preds = %0
  %6 = load i32, ptr %2, align 4
  %7 = icmp sgt i32 %6, 100
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  br label %15

9:                                                ; preds = %5
  %10 = load i32, ptr %2, align 4
  %11 = icmp sge i32 %10, 200
  br i1 %11, label %12, label %13

12:                                               ; preds = %9
  br label %14

13:                                               ; preds = %9
  br label %14

14:                                               ; preds = %13, %12
  br label %15

15:                                               ; preds = %14, %8
  br label %22

16:                                               ; preds = %0
  %17 = load i32, ptr %2, align 4
  %18 = icmp sge i32 %17, 200
  br i1 %18, label %19, label %20

19:                                               ; preds = %16
  br label %21

20:                                               ; preds = %16
  br label %21

21:                                               ; preds = %20, %19
  br label %22

22:                                               ; preds = %21, %15
  %23 = load i32, ptr %2, align 4
  ret i32 %23
}

attributes #0 = { mustprogress noinline norecurse nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 1, !"MaxTLSAlign", i32 65536}
!4 = !{!"clang version 19.1.4"}
