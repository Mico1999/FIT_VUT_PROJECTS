/* CURRENTLY PASSING (CAN BE REMOVED)

Compiler error output:
** Semantic error: Identifier: 'a' on line 4 was not defined yet ! **
munmap_chunk(): invalid pointer

----
ERROR: Unexpected exit code of IFJ compiler. Actual: -6 Expected: [3].

*/
package main

func alpha(){
	a = 5
}

func main () {
	a := 5
	alpha()
}
