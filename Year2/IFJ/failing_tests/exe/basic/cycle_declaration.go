/*

Interpret error output:
Error at line: 199
Symbol already exists!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 52 Expected: [0].


ASSEMBLY 195 - 205: 
	# EXPR BEGIN
	PUSHS int@0
	POPS GF@expr_result
	# EXPR END
	DEFVAR LF@b 				<--------
	MOVE LF@b GF@expr_result
	# EXPR BEGIN
	PUSHS LF@a
	PUSHS LF@a
	MULS
	POPS GF@expr_result
-----------------------------------------	

*/


package main

func main () {
	for a:=0;a<5;a=a+1 {
		b := 0
		b = a*a
		print(b,"\n")
	}
}
