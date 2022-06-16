/* CURRENTLY PASSING (CAN BE REMOVED)

Compiler error output:
** Semantic error: Identifier: 'a' on line 8 was not defined yet ! **
munmap_chunk(): invalid pointer

----
ERROR: Unexpected exit code of IFJ compiler. Actual: -6 Expected: [3].

*/

package main

func alpha () {
	a := 0
}

func main () {
	a = 5
}
