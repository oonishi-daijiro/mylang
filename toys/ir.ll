; ModuleID = 'main'
source_filename = "main"

define i32 @"0"() {
  %hoge = alloca i32, align 4
  store i32 100, ptr %hoge, align 4
  %1 = load i32, ptr %hoge, align 4
  %2 = icmp sgt i32 %1, 100
  br i1 %2, label %3, label %14

3:                                                ; preds = %0
  %4 = load i32, ptr %hoge, align 4
  %5 = icmp sgt i32 %4, 100
  br i1 %5, label %6, label %7

6:                                                ; preds = %3
  br label %13

7:                                                ; preds = %3
  %8 = load i32, ptr %hoge, align 4
  %9 = icmp sge i32 %8, 200
  br i1 %9, label %10, label %11

10:                                               ; preds = %7
  br label %12

11:                                               ; preds = %7
  br label %12

12:                                               ; preds = %10, %11
  br label %13

13:                                               ; preds = %6, %12
  br label %20

14:                                               ; preds = %0
  %15 = load i32, ptr %hoge, align 4
  %16 = icmp sge i32 %15, 200
  br i1 %16, label %17, label %18

17:                                               ; preds = %14
  br label %19

18:                                               ; preds = %14
  br label %19

19:                                               ; preds = %17, %18
  br label %20

20:                                               ; preds = %13, %19
  %21 = load i32, ptr %hoge, align 4
  ret i32 %21
}
