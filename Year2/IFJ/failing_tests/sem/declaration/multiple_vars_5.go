/*
Interpret error output:
Error at line: 179
Symbol already exists!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 52 Expected: [0].

ASSEMBLY LINES 174 - 184

	MOVE LF@a GF@expr_result
		# EXPR BEGIN
		PUSHS int@0
		POPS GF@expr_result
		# EXPR END
	DEFVAR LF@a					<--------
	MOVE LF@a GF@expr_result
	LABEL $forA
	LABEL $conditionA
		# EXPR BEGIN
		PUSHS int@0

*/
package main

func main () {
	a:=3.6
	for a:=0;0!=0;{
		a:="help"
	}
}
