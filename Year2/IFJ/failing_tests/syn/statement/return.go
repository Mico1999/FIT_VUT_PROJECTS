/* CURRENTLY PASSING (CAN BE REMOVED) ... KEEPING THIS JUST IN CASE

   Interpret error output:
   Error at line: 169
   Label does not exist!

   ----
   ERROR: Unexpected exit code of IFJ interpreter. Actual: 52 Expected: [0].

   ASSEMBLY LINES 164 - 174:

      164	RETURN
      165	
      166	LABEL $main
      167	 CREATEFRAME
      168	 PUSHFRAME
      169	JUMP $main&return	<-----
      170	POPFRAME
      171	CLEARS
      172	JUMP $end_of_code
      173	
      174	LABEL $end_of_code

*/
package main

func main () {
	return
}
