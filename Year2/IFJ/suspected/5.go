/////
///// INVALID ASSEMBLY ERROR
///// LOWEST PRIORITY
/////


package main
func main () {
	a := 30
	print(a, "\n")
	for a:=10;a>0;a=a-1 {
		print(a, "\n")
		a := 3.14 // @Rucko toto je chyba, iteracna premenna sa moze prekryt
		print(a, "\n")
		if a>5.0 {
			print(a)
			a:="hello" //toto ukazuje spravne chybu
			print(a, "\n")
		} else {
			print(a, "\n")
			a:="world" // aj toto
			print(a, "\n")
		}
	}
}
// funguje ale pri zatieneni s premennymi zo vzdialenych scopov nie => najlepsie vymysliet nejako cez zasobnik
// lenze pekne ISS a pekne INP a pekne matiky a pekny cas
// test sa vola exe/basic/shadowing.go - pravdepodobne problem s definovanim lokalnych vars s id rovnakym ako v globalnom/nadradenom scope