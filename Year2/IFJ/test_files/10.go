// Pridajte co vbas napadne
// funguje
package main

func foo() {

	for a:= 0; a<5; a =a+1 {
		print(a, "\n")
		for b:= 2; b < 4; b = b+1 {
			print(b, "\n")
		}
		b:= 10
		print(b, "\n")
	}
	for i:= 0; i < 4; i = i+1 {
		print("Misko Kmet\n")
	}
}

func main() {

	foo()
}