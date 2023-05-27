; ModuleID = 'test/test.algebraicidentity.optimized.bc'
source_filename = "test/test.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 0
  %3 = mul nsw i32 %0, 16
  %4 = mul nsw i32 %3, %0
  %5 = sdiv i32 %4, %0
  %6 = sdiv i32 %4, 10
  %7 = mul nsw i32 1, %5
  %8 = sdiv i32 %6, 128
  %9 = sdiv i32 %5, 54
  %10 = sdiv i32 %6, 1
  %11 = sub nsw i32 %9, 0
  %12 = add nsw i32 %9, 1
  %13 = add nsw i32 %6, 5
  %14 = mul nsw i32 %9, 17
  %15 = add nsw i32 %14, 10
  %16 = add nsw i32 %9, 1
  %17 = sub nsw i32 %16, 2
  ret void
}
