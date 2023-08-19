define dso_local noundef i32 @sum(ptr noundef %a, ptr noundef %b) #0
{
	; START INDIRECT VALUE
	%1 = load i32, ptr %a, align 4
	; END INDIRECT VALUE

	; START INDIRECT VALUE
	%2 = load i32, ptr %b, align 4
	; END INDIRECT VALUE

	; START BINARY OPERATOR
	%3 = add i32 %1, %2
	; END BINARY OPERATOR

	; START RETURN
	ret i32 %3
	; END RETURN

}
define dso_local noundef i32 @main() {
entry:
	; START DECLARATION
	%b = alloca i32, align 4
	; END DECLARATION

	; START FUNCTION CALL ARGUMENTS
	; START IMMEDIATE VALUE
	%0 = alloca i32, align 4
	store i32 5, ptr %0, align 4
	; END IMMEDIATE VALUE

	; START IMMEDIATE VALUE
	%1 = alloca i32, align 4
	store i32 6, ptr %1, align 4
	; END IMMEDIATE VALUE

	; END FUNCTION CALL ARGUMENTS
	; START FUNCTION CALL
	%2 = call noundef i32 @sum(ptr %0, ptr %1)
	; END FUNCTION CALL

	; START ASSIGNMENT
	store i32 %2, ptr %b, align 4
	; END ASSIGNMENT

	; START INDIRECT VALUE
	%3 = load i32, ptr %b, align 4
	; END INDIRECT VALUE

	; START RETURN
	ret i32 %3
	; END RETURN

}
declare i32 @printf(ptr noundef, ...) #99
