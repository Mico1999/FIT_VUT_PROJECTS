/*

Go error output:
<empty>
----
Interpret error output:
<empty>
----
Go output:
0x1.4p+02
674
674 0x1.51p+09
hello world !?
14
world ! 7
111
011112
011@

----
Interpret output:
0x1.4p+2
674
674 0x1.51p+9
hello world !?
14
world ! 7
111
111112
011@

----
ERROR: Go and IFJ interprets have different outputs.


*/
package main

func PrintSuccess(cond int) {
	if (cond==0) {
		print("0")
	} else {
		print("1")
	}
}

func main () {
	i := 0
	f := 0.0
	s := "hello world !?"

	// casts
	f = int2float(5)
	print(f, "\n")
	i = float2int(6.748e+2)
	print(i, "\n")
	f = int2float(i)
	i = float2int(f)
	print(i, " ", f, "\n")

	// len
	print(s, "\n")
	i = len(s)
	print(i, "\n")
	s,_ = substr(s,6,7)
	i = len(s)
	print(s, " ", i, "\n")

	// substr
	s,i = substr("help_me", 10, 1)
	PrintSuccess(i)
	i = 5-10
	s,i = substr("help_me", i, 1)
	PrintSuccess(i)
	i = 5-10
	s,i = substr("help_me", 3, i)
	PrintSuccess(i)
	print("\n")

	// ord
	_,i = ord("help", 1)
	PrintSuccess(i)
	i = 2-8
	_,i = ord("help", i)
	PrintSuccess(i)
	_,i = ord("help", 9)
	PrintSuccess(i)
	i = 3
	i,_ = ord("help", i)
	print(i, "\n")

	// chr
	_,i = chr(20)
	PrintSuccess(i)
	i = 3-10
	s,i = chr(i)
	PrintSuccess(i)
	_,i = chr(256)
	PrintSuccess(i)
	s,_ = chr(64)
	print(s, "\n")
}
