/* 
Interpret error output:
Error at line: 187
Symbol already exists!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 52 Expected: [0].

ASSEMBLY LINES 182 - 192

	JUMPIFEQ $elseA GF@expr_result bool@false
		# EXPR BEGIN
		PUSHS int@1
		POPS GF@expr_result
		# EXPR END
	DEFVAR LF@a					<--------
	MOVE LF@a GF@expr_result
	JUMP $if_endA
	LABEL $elseA
		# EXPR BEGIN
		PUSHS int@2

*/


package main

func main () {
	a := 0
	if 0==0 {
		a:=1
	}else {
		a:=2
	}
}
