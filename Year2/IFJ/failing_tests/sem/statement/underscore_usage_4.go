/*

Compiler error output:
** Semantic error: Wrong number of IDs on left side for function: 'alpha' to unpack return values to. Line: 8 ! **
Error code: 6

----
ERROR: Unexpected exit code of IFJ compiler. Actual: 6 Expected: [7]. 			<#--------------

*/

package main

func alpha (a int) (int) {
	return 6
}

func main () {
	alpha(_) // Should throw error 7
}
