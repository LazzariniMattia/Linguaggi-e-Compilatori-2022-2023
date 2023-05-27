define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 0
  %3 = sdiv i32 %2, 4
  %4 = mul nsw i32 4, %3
  %5 = add nsw i32 %4, 10
  ret void
}