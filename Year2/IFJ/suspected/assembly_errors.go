// ASSEMBLY ERRORS
// Symbol already exists! errors so far - error code 52  - conflicting symbol being "DEFVAR LF@a" a

package main

func main3 () {
	a:=3.6
	for a:=0;0!=0;{
		a:="help" // a already defined -- Neviem preco nepreslo
	}
}


func main2 () {
	a := 0
	if 0==0 {
		a:=1 // a already defined error? isto nie!
	}else {
		a:=2
	}
}