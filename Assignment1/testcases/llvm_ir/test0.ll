; Declare printf
declare i32 @printf(i8*, ...)

;Declare scanf
declare i32 @scanf(i8*, ...)

define i32 @main() {
	%A = alloca i32
	store i32 10, i32* %A
	%_tmp_1 = load i32, i32* %A
	%_tmp_2 = add i32 %_tmp_1, 20
	%B = alloca i32
	store i32 %_tmp_2, i32* %B
	%_printf_format_1 = alloca [5 x i8]
	store [5 x i8] c"%d \0A\00", [5 x i8]* %_printf_format_1
	%_printf_str_1 = getelementptr [5 x i8], [5 x i8]* %_printf_format_1, i32 0, i32 0
	%_tmp_3 = load i32, i32* %B
	call i32 (i8*, ...) @printf(i8* %_printf_str_1, i32 %_tmp_3)
	ret i32 0
}
