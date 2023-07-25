define dso_local i32 @main() {
entry:
	; START DECLARATION
	%a = alloca i32, align 4
	; END DECLARATION

	; START ASSIGNMENT
	store i32 5, ptr %a, align 4
	; END ASSIGNMENT

	; START DECLARATION
	%b = alloca i32, align 4
	; END DECLARATION

	; START ASSIGNMENT
	store i32 16, ptr %b, align 4
	; END ASSIGNMENT

	; START DECLARATION
	%c = alloca i32, align 4
	; END DECLARATION

	; START INDIRECT VALUE
	%0 = load i32, ptr %a, align 4
	; END INDIRECT VALUE

	; START INDIRECT VALUE
	%1 = load i32, ptr %b, align 4
	; END INDIRECT VALUE

	; START BINARY OPERATOR
	%2 = add i32 %0, %1
	; END BINARY OPERATOR

	; START IMMEDIATE VALUE
	%3 = alloca i32, align 4
	store i32 5, ptr %3, align 4
	; END IMMEDIATE VALUE

	; START UNARY OPERATOR
	%4 = load i32, ptr %3, align 4
	%5 = sub nsw i32 0, %4
	; END UNARY OPERATOR

	; START BINARY OPERATOR
	%6 = add i32 %2, %5
	; END BINARY OPERATOR

	; START ASSIGNMENT
	store i32 %6, ptr %c, align 4
	; END ASSIGNMENT

	; START DECLARATION
	%a1b = alloca i32, align 4
	; END DECLARATION

	; START ASSIGNMENT
	store i32 1, ptr %a1b, align 4
	; END ASSIGNMENT

	; START DECLARATION
	%a2c = alloca i32, align 4
	; END DECLARATION

	; START ASSIGNMENT
	store i32 2, ptr %a2c, align 4
	; END ASSIGNMENT

	; START DECLARATION
	%a3d = alloca i32, align 4
	; END DECLARATION

	; START ASSIGNMENT
	store i32 3, ptr %a3d, align 4
	; END ASSIGNMENT

	; START INDIRECT VALUE
	%7 = load i32, ptr %c, align 4
	; END INDIRECT VALUE

	; START IMMEDIATE VALUE
	%8 = alloca i32, align 4
	store i32 10, ptr %8, align 4
	; END IMMEDIATE VALUE

	; START BINARY OPERATOR PRELOAD
	%9 = load i32, ptr %8, align 4
	; END BINARY OPERATOR PRELOAD

	; START BINARY OPERATOR
	%10 = mul i32 %7, %9
	; END BINARY OPERATOR

	; START RETURN
	ret i32 %10
	; END RETURN

}
declare i32 @printf(ptr noundef, ...) #2
