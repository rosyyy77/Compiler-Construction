; ModuleID = './llvm_ir/test1.ll'
source_filename = "./llvm_ir/test1.ll"

; Function Attrs: nofree nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree nounwind
define i32 @main() local_unnamed_addr #0 {
  %_printf_format_1 = alloca [8 x i8], align 1
  %.fca.0.gep = getelementptr inbounds [8 x i8], [8 x i8]* %_printf_format_1, i64 0, i64 0
  store i8 37, i8* %.fca.0.gep, align 1
  %.fca.1.gep = getelementptr inbounds [8 x i8], [8 x i8]* %_printf_format_1, i64 0, i64 1
  store i8 100, i8* %.fca.1.gep, align 1
  %.fca.2.gep = getelementptr inbounds [8 x i8], [8 x i8]* %_printf_format_1, i64 0, i64 2
  store i8 32, i8* %.fca.2.gep, align 1
  %.fca.3.gep = getelementptr inbounds [8 x i8], [8 x i8]* %_printf_format_1, i64 0, i64 3
  store i8 37, i8* %.fca.3.gep, align 1
  %.fca.4.gep = getelementptr inbounds [8 x i8], [8 x i8]* %_printf_format_1, i64 0, i64 4
  store i8 100, i8* %.fca.4.gep, align 1
  %.fca.5.gep = getelementptr inbounds [8 x i8], [8 x i8]* %_printf_format_1, i64 0, i64 5
  store i8 32, i8* %.fca.5.gep, align 1
  %.fca.6.gep = getelementptr inbounds [8 x i8], [8 x i8]* %_printf_format_1, i64 0, i64 6
  store i8 10, i8* %.fca.6.gep, align 1
  %.fca.7.gep = getelementptr inbounds [8 x i8], [8 x i8]* %_printf_format_1, i64 0, i64 7
  store i8 0, i8* %.fca.7.gep, align 1
  %1 = call i32 (i8*, ...) @printf(i8* nonnull %.fca.0.gep, i32 1, i32 -2)
  ret i32 0
}

attributes #0 = { nofree nounwind }
