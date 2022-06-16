/*

Interpret error output:
Error at line: 183
Symbol already exists!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 52 Expected: [0].

ASSEMBLY LINES 178 - 188

	WRITE TF@%param
	# EXPR BEGIN
	PUSHS int@10
	POPS GF@expr_result
	# EXPR END
	DEFVAR LF@a				<---------
	MOVE LF@a GF@expr_result
	LABEL $forA
	LABEL $conditionA
	# EXPR BEGIN
	PUSHS LF@a

*/

package main

func main () {
	a := 30
	print(a)
	for a:=10;a>0;a=a-1 {
		print(a)
		a := 3.14
		print(a)
		if a>5.0 {
			print(a)
			a:="hello"
			print(a)
		} else {
			print(a)
			a:="world"
			print(a)
		}
	}
	print(a)
}
