package main

func main()(){
	a := 0
	if ((a == 0)){
		a := 1
		if a == 1 {
			a := 3.5
			if a == 3.5 {
				print("Uz by aj stacilo","\n")
			} else{
				print(a) //3.5
			}
		} else{
			print(a) //1
		}
	} else{
		print(a) // 0
	}
}

// toto zanorenie tiez nefunguje