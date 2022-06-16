/*


*/package main/*



*/ func main(){
	i := 0
	j := 0
	res := 0
	for ; i < 20; i = i+1 {
		for ; j < 20; j= j+1 {
			res = i*j
			print(i," * ",j," = ",res,"\n")
		}
	}
// for vo fore nefunguje este pri generovani, vykona sa iba prva itaracia vonajsieho cyklu a 20 vnutorneho
	
	foo()

	hardcore(0,0)
}

// not the same as above 
func foo(){
	j:= 0
	res := 0
	for i := 0; i < 20; i,j = i+1,j+1{ // working
		res = i*j
		print(i," * ",j," = ",res,"\n")
	}
}

func hardcore(i int, j int){
	res := 0
	a := 0
	i,j,a = 0+0,0+0,0+0
	for ; i < 20; i,j = i+1,j+1{
		res = i*j
		print(i," * ",j," = ",res,"\n")
	}
}
