
package main


// Neprechadza dalsich par testov s rovnakymi operandmi, no rozdielnymi logickymi operatormi

func main () {
	if (0>=(5==5)) { // testsuite.py neprechadza  @Monsinour Peter toto by mozno mala byt chyba
	} else {
	}
}

func main1 () {
	if (0<(5==5)) { // testsuite.py neprechadza  //  ^^ aj toto
	} else {
	}
}


func main2 () {
	if ("help">="pleh") {
	} else {
	}
}

func main () {
	if ("help"<"pleh") {
	} else {
	}
}


