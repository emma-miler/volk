@.str.0 = private unnamed_addr constant [2 x i8] c"1\00", align 1
define dso_local noundef i32 @main() {
entry:
	; START DECLARATION
	%test = alloca ptr
	; END DECLARATION

	; START ASSIGNMENT VALUE
	; START STRING CONSTANT VALUE
	%0 = alloca ptr
	store ptr @.str.0, ptr %0
	; END STRING CONSTANT VALUE

	; START ASSIGNMENT
	store ptr %0, ptr %test
	; END ASSIGNMENT

	; START RETURN VALUE
	; START IMMEDIATE VALUE
	%1 = alloca i32
	store i32 6, ptr %1
	; END IMMEDIATE VALUE

	; END RETURN VALUE
	; START RETURN
	%2 = load i32, ptr %1
	ret i32 %2
	; END RETURN

}
declare i32 @printf(ptr noundef, ...) #99
