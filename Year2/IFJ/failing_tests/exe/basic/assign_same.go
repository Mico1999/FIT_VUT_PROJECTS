/*
Go error output:
<empty>
----
Interpret error output:
<empty>
----
Go output:
2
0x1.7333333333333p+01

----
Interpret output:
0
0x1.d99999999999ap+1

----
ERROR: Go and IFJ interprets have different outputs.
*/

package main

func alpha () (int, int) {
	return 1, 2
}

func beta (param float64) (float64, string, float64) {
	param = param + 3.7
	return param, "help" ,2.9
}

func main () {
	a := 0
	a,a = alpha()
	print(a, "\n")

	f := 0.0
	f, _, f = beta(f)
	print(f, "\n")
}
