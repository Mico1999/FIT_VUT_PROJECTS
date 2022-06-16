/*

Compiler error output:
** Syntax error: token type 'EOL', line 4! **													<-----------------------------------------
** Semantic error: Function: 'alpha' does not return what it should on line: 5 ! **
** Semantic error: Unmatched number of parameter values in function: 'alpha' on line: 8 ! **
Error code: 2

----
ERROR: Unexpected exit code of IFJ compiler. Actual: 2 Expected: [0, 6]. 						<-----------------------------------------

*/

package main

func alpha (a int) (string) {
	return // Line 4 EOL - unwanted syntax error (line numering ignores comments) 
} 

func main () {
	alpha()
}


