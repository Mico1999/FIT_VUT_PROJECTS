/**

Interpret error output:
Error at line: 178
Division by zero!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 57 Expected: [9].


ASSEMBLY LINES 173 - 183

	DEFVAR LF@b
	MOVE LF@b GF@expr_result
		# EXPR BEGIN
		PUSHS int@5
		PUSHS LF@b
		IDIVS				<---------
		POPS GF@expr_result
		# EXPR END
	DEFVAR LF@a
	MOVE LF@a GF@expr_result
	CREATEFRAME

**/
package main

func main () {
	b := 0
	a := 5 / b
	print(a)
}
