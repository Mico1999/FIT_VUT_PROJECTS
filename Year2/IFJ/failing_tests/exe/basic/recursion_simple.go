/*

Interpret error output:
Error at line: 175
Operand stack is empty

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 56 Expected: [0].

ASSEMBLY LINE 170 - 180:

# EXPR BEGIN
  PUSHS LF@%param1
  PUSHS int@0
  POPS GF@tmp1
  POPS GF@tmp2
  LTS				<------------
  PUSHS GF@tmp1
  PUSHS GF@tmp2
  EQS
  ORS
  POPS GF@expr_result


*/
package main

func alpha(a int) (int) {
	if a<=0 {
		return 0
	} else {
		b := a-1
		result := 0
		result = alpha(b)
		return result+a
	}
}

func main() {
	result := 0
	result = alpha(10)
	print(result)
}
