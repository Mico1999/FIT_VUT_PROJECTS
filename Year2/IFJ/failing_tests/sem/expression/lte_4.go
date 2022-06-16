/*

Compiler error output:
Error code: 0

----
ERROR: Unexpected exit code of IFJ compiler. Actual: 0 Expected: [5].

*/

package main

func main () {
	if (0<=(5==5)) { // should throw error 5
	} else {
	}
}
