package main

func main () {
	a := 0
	b:=0

	a,b = 8, 9, 10 // syn error...Ma byt sem error  @Veduci myslim ze moze byt aj syntax, lebo pride neocakavana ciarka...
}

func main2 () {
	a := 0
	b:=0

	a,b = 8 // Pokazi error system, syntax error - ma byt sem error  // ^^ aj tuto
}

func main3 () {
	a := 0
	b:=0

	a,b = "help", 9.4 // error code 4 - spravne ?    Dal by som chybu 5 alebo 7
}

func alpha (a int) (string) {
	return	// Chyba vylucenia void return - treba retest po oprave
}

func main4 () {
	alpha()
}

func main5 () {
	a := _ + 6 // error code 3 - spravne ? - pravdepodobne ma prednost error "_ nemozes citat"   Dal by som chyba 7 
}


func main6 () {
	alpha(_) // error 6 - spravne ? - pravdepodobne to iste  To je podla mna dobre
}