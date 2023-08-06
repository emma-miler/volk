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

	; START IMMEDIATE VALUE
	%0 = alloca i32, align 4
	store i32 42, ptr %0, align 4
	; END IMMEDIATE VALUE

	; START INDIRECT VALUE
	%1 = load i32, ptr %a, align 4
	; END INDIRECT VALUE

	; START BINARY OPERATOR PRELOAD
	%2 = load i32, ptr %0, align 4
	; END BINARY OPERATOR PRELOAD

	; START BINARY OPERATOR
	%3 = mul i32 %2, %1
	; END BINARY OPERATOR

	; START INDIRECT VALUE
	%4 = load i32, ptr %b, align 4
	; END INDIRECT VALUE

	; START IMMEDIATE VALUE
	%5 = alloca i32, align 4
	store i32 16, ptr %5, align 4
	; END IMMEDIATE VALUE

	; START BINARY OPERATOR PRELOAD
	%6 = load i32, ptr %5, align 4
	; END BINARY OPERATOR PRELOAD

	; START BINARY OPERATOR
	%7 = add i32 %4, %6
	; END BINARY OPERATOR

	; START BINARY OPERATOR
	%8 = mul i32 %3, %7
	; END BINARY OPERATOR

	; START ASSIGNMENT
	store i32 %8, ptr %c, align 4
	; END ASSIGNMENT

	; START INDIRECT VALUE
	%9 = load i32, ptr %c, align 4
	; END INDIRECT VALUE

	; START RETURN
	ret i32 %9
	; END RETURN

}
declare i32 @printf(ptr noundef, ...) #2
