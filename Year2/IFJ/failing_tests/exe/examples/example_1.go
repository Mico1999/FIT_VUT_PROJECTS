/*****************************************
-----------input factorial_D.in-----------
PROGRAM INPUT:
anfg

----
Interpret error output:
Error at line: 188
Wrong operand type!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 53 Expected: [0].

ASSEMBLY LINES 183 - 193:
	MOVE GF@_ TF@%retval2
	LABEL $ifA
		# EXPR BEGIN
		PUSHS LF@a
		PUSHS int@0
		LTS
		POPS GF@expr_result
		# EXPR END
	JUMPIFEQ $elseA GF@expr_result bool@false
	CREATEFRAME
	DEFVAR TF@%param



*****************************************/



/*****************************************
-----------input ../empty.in-----------
Interpret error output:
Error at line: 188
Wrong operand type!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 53 Expected: [0].

ASSEMBLY LINES 183 - 193:
	MOVE GF@_ TF@%retval2
	LABEL $ifA
		# EXPR BEGIN
		PUSHS LF@a
		PUSHS int@0
		LTS						<----------------
		POPS GF@expr_result
		# EXPR END
	JUMPIFEQ $elseA GF@expr_result bool@false
	CREATEFRAME
	DEFVAR TF@%param


*****************************************/


// Program 1: Vypocet faktorialu (iterativne)
package main

func main() {
	print("Zadejte cislo pro vypocet faktorialu: ")
	a := 0
	a, _ = inputi()
	if a < 0 {
		print("Faktorial nejde spocitat!\n")
	} else {
		vysl := 1
		for ; a > 0; a = a - 1 {
			vysl = vysl * a
		}
		print("Vysledek je ", vysl, "\n")
	}
}
