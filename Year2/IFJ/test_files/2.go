package main

func foo(){

	for i := 1; i < 5; i = i+1{
		if (i > 0) {
			print(":)\n")
		} else{
			print("Chyba jak svina\n")
		}
	}
	f := 5
	if (f == 5) {
		i := 4
		print(i, "\n")
		if (i == 4) {
			i := 5
		} else {

		}
		print(i, "\n")
		f := 10
		print(f, "\n")
	} else{
		print("nie\n")
	}
}

func main(){

	foo()
}

// AJ toto funguje :)