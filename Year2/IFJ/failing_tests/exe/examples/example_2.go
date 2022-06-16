/*****************************
-----input factorial_B.in-----
PROGRAM INPUT:
5

----
Go error output:
<empty>
----
Interpret error output:
<empty>
----
Go output:
Zadejte cislo pro vypocet faktorialu: Vysledek je 120

----
Interpret output:
Zadejte cislo pro vypocet faktorialu: Vysledek je true

----
ERROR: Go and IFJ interprets have different outputs.

*****************************/



/***************************** SOLVED - CURRENTLY PASSING (COLAPSE THIS COMMENT) ... KEEPING THIS JUST IN CASE

	-----input factorial_C.in-----
	PROGRAM INPUT:
	0

	----
	Go error output:
	<empty>
	----
	Interpret error output:
	<empty>
	----
	Go output:
	Zadejte cislo pro vypocet faktorialu: Vysledek je 1

	----
	Interpret output:
	Zadejte cislo pro vypocet faktorialu: Vysledek je true

	----
	ERROR: Go and IFJ interprets have different outputs.
	PROGRAM INPUT:
	0

	----
	Go error output:
	<empty>
	----
	Interpret error output:
	<empty>
	----
	Go output:
	Zadejte cislo pro vypocet faktorialu: Vysledek je 1

	----
	Interpret output:
	Zadejte cislo pro vypocet faktorialu: Vysledek je true

	----
	ERROR: Go and IFJ interprets have different outputs.

*****************************/



/*****************************
-----input factorial_D.in-----
PROGRAM INPUT:
anfg

----
Interpret error output:
Error at line: 254
Wrong operand type!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 53 Expected: [0].


ASSEMBLY LINES 249 - 259:

	JUMPIFEQ $elseB GF@expr_result bool@false
	LABEL $ifC
	# EXPR BEGIN
	PUSHS LF@a
	PUSHS int@0
	LTS								<-----------
	POPS GF@expr_result
	# EXPR END
	JUMPIFEQ $elseC GF@expr_result bool@false
	CREATEFRAME
	DEFVAR TF@%param

*****************************/



/*****************************
-----   input empty.in   -----
Interpret error output:
Error at line: 254
Wrong operand type!

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 53 Expected: [0].

ASSEMBLY LINES 249 - 259

	JUMPIFEQ $elseB GF@expr_result bool@false
	LABEL $ifC
	# EXPR BEGIN
	PUSHS LF@a
	PUSHS int@0
	LTS								<-----------
	POPS GF@expr_result
	# EXPR END
	JUMPIFEQ $elseC GF@expr_result bool@false
	CREATEFRAME
	DEFVAR TF@%param


*****************************/


// Program 2: Vypocet faktorialu (rekurzivne)
package main

func factorial(n int) (int) {
	dec_n := n - 1
	if n < 2 {
		return 1
	} else {
		tmp := 0
		tmp = factorial(dec_n)
		return n * tmp
	}
}

func main() {
	print("Zadejte cislo pro vypocet faktorialu: ")
	a := 0
	err := 0
	a, err = inputi()
	if err == 0 {
		if a < 0 {
			print("Faktorial nejde spocitat!", "\n")
		} else {
			vysl := 0
			vysl = factorial(a)
			print("Vysledek je ", vysl, "\n")
		}
	} else {
		print("Chyba pri nacitani celeho cisla!\n")
	}
}
