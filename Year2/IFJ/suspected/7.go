// 6x fail na 6 testov
// LOWEST PRIORITY

/* Ak z tohoto kohosi napadne ako to fixnut, mozte sa o to pokusit. Inak radsej ostatne

** Semantic error: Invalid operand for expression of type 'T_STRING' on line 129 ! **
** Semantic error: Function: 'ProcessShape' does not return what it should on line: 131 ! **
** Semantic error: Invalid operand for expression of type 'T_STRING' on line 132 ! **
** Semantic error: Function: 'ProcessShape' does not return what it should on line: 134 ! **
** Semantic error: Invalid operand for expression of type 'T_STRING' on line 135 ! **
** Semantic error: Function: 'ProcessShape' does not return what it should on line: 137 ! **
** Semantic error: Invalid operand for expression of type 'T_STRING' on line 138 ! **
** Semantic error: Function: 'ProcessShape' does not return what it should on line: 140 ! **
** Semantic error: Invalid operand for expression of type 'T_STRING' on line 141 ! **
Error code: 5
*/

//input geometry_square.in
//input geometry_ractange.in
//input geometry_circle.in
//input geometry_triangle.in
//input geometry_multiple_A.in
//input geometry_multiple_B.in
//
package main

func PrintResult(o float64, s float64) {
	print("o = ", o, "\ns = ", s, "\n")
}

func ProcessSquare()(int) {
	print("a = ")
	a := 0.0
	err := 0
	a,err = inputf()
	if err!=0 {
		return 1
	} else {
		o := 0.0
		s := 0.0
		o = 4.0 * a
		s = a * a
		PrintResult(o,s)
		return 0
	}
}

func ProcessRectangle()(int) {
	print("a = ")
	a:=0.0
	b:=0.0
	two:=0.0
	two=int2float(2)
	err:=0
	a,err=inputf()
	if err!=0 {
		return 1
	} else {
		print("b = ")
		b,err = inputf()
		if err!=0 {
			return 1
		} else {
			o:=0.0
			s:=0.0
			o = two*a + two*b
			s = a * b
			PrintResult(o,s)
			return 0
		}
	}
}

func ProcessCircle(PI float64) (int) {
	err := 0
	r := 0.0
	o := 0.0
	s := 0.0

	print("r = ")
	r,err = inputf()

	if (err != 0) {

		return 1

	} else {

		o = 2.0 * PI * r
		s = PI * r * r

		PrintResult(o,s)
	}

	return 0
}

func ProcessTriangle () (int) {
	a:=0.0
	b:=0.0
	c:=0.0
	va:=0.0
	err:=0

	print("a = ")
	a,err = inputf()
	if err == 1 {
		return 1
	} else {
	}

	print("b = ")
	b,err = inputf()
	if err == 1 {
		return 1
	} else {
	}

	print("c = ")
	c,err = inputf()
	if err == 1 {
		return 1
	} else {
	}

	print("va = ")
	va,err = inputf()
	if err == 1 {
		return 1
	} else {
	}

	o := 0.0
	s := 0.0
	two := 0.0
	two = int2float(2)
	o = a + b + c
	s = a * va / two

	PrintResult(o,s)
	return 0
}

func ProcessShape (shape string, PI float64) (int,int) {
	err := 0
	if (shape == "square") {
		err = ProcessSquare()
	} else {
		if (shape == "ractangle") {
			err = ProcessRectangle()
		} else {
			if (shape == "circle") {
				err = ProcessCircle(PI)
			} else {
				if (shape == "triangle") {
					err = ProcessTriangle()
				} else {
					if (shape == "exit") {
						return 0,1
					} else {
						return 1,0
					}
				}
			}
		}
	}
	return err, 0
}

func main () {
	PI := 3.14159
	print("GEOMETRY CALCULATIONS\n", "exit program but typing \"exit\"")
	exit := 0
	for ;exit==0; {
		print("Select shape (square, ractangle, circle, triangle): ")
		shape := ""
		err := 0
		shape,err = inputs()
		if err == 1 {
			print("Invalid input. Try again\n")
		} else {
			err, exit = ProcessShape(shape, PI)
			if err == 1 {
				print("Invalid input, Try again\n")
			} else {
			}
		}
	}
	print("PROGRAM EXITING\n")
}
