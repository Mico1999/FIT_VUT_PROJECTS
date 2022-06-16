package main

func main (){
	a := 1
	b := 1
	for i := 0; i < 10; i = i+1 {
		a = a + b
		print(a, ", ")
		b = a + b 
		print(b, ", ")
	}
	if a < 100 {
		print("Something went Wrong\n")
	} else {

	}
}

//result :
// 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 
// TOTO FUNGUJEEE XDDD