@.str.0 = private unnamed_addr constant [5 x i8] c"test\00", align 1
define dso_local noundef i32 @main() {
entry:
	; START DECLARATION
	%t = alloca ptr
	; END DECLARATION

	; START ASSIGNMENT VALUE
	; PUSHED STRING CONSTANT VALUE
	; START ASSIGNMENT
	store ptr @.str.0, ptr %t
	; END ASSIGNMENT

	; START FUNCTION CALL ARGUMENTS
	; START INDIRECT VALUE
	%0 = load i32, ptr %t
	; END INDIRECT VALUE

	; END FUNCTION CALL ARGUMENTS
	; START FUNCTION CALL
	%1 = call noundef i32 @printf(ptr %0)
	; END FUNCTION CALL

	; START RETURN VALUE
	; START IMMEDIATE VALUE
	%2 = alloca i32
	store i32 2, ptr %2
	; END IMMEDIATE VALUE

	; END RETURN VALUE
	; START RETURN
	%3 = load i32, ptr %2
	ret i32 %3
	; END RETURN

}
declare i32 @printf(ptr noundef, ...) #99
