// IFJ20: Ukazka prace s vice navratovymi hodnotami a vicenasobnym prirazenim a prekryti promenne a

package main

func foo(x int, y int) (int, int) {
	i := x
	j := (y + 2) * 3
	i, j = j+1, i+1 // working
	return i, j
}

func main() {
	a := 1
	b := 2
	a, b = foo(a, b)
	if a < b {
		print(a, "<", b, "\n")
		a := 666
		print(a) // otherwise, warning "unused a"
	} else {
		print(a, ">=", b, "\n")
		a = 33
	}
	print(a)
}
