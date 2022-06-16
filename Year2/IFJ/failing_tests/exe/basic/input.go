
/**************************************
-----------input input_A.in------------
PROGRAM INPUT:
5 5.6 hello

----
Go error output:
<empty>
----
Interpret error output:
<empty>
----
Go output:
VALID INT 5
INVALID FLOAT
INVALID STRING

----
Interpret output:
VALID INT 5
VALID FLOAT 
VALID STRING 

----
ERROR: Go and IFJ interprets have different outputs.

**************************************/


/**************************************
-----------input input_D.in------------
PROGRAM INPUT:
hello
word
$#<>"

----
Go error output:
<empty>
----
Interpret error output:
<empty>
----
Go output:
INVALID INT
INVALID FLOAT
VALID STRING $#<>"

----
Interpret output:
VALID INT 
VALID FLOAT 
VALID STRING $#<>"

----
ERROR: Go and IFJ interprets have different outputs.

**************************************/


/**************************************
-----------input input_E.in------------
PROGRAM INPUT:
4.6
string

----
Go error output:
<empty>
----
Interpret error output:
<empty>
----
Go output:
VALID INT 4
INVALID FLOAT
INVALID STRING

----
Interpret output:
VALID INT 4
VALID FLOAT 
VALID STRING 

----
ERROR: Go and IFJ interprets have different outputs.

**************************************/


/**************************************
-----------input empty.in------------
Go error output:
<empty>
----
Interpret error output:
<empty>
----
Go output:
INVALID INT
INVALID FLOAT
INVALID STRING

----
Interpret output:
VALID INT 
VALID FLOAT 
VALID STRING 

----
ERROR: Go and IFJ interprets have different outputs.

**************************************/

package main

func main () {
	err := 0
	resultI := 0
	resultI, err = inputi()
	if (err ==1) {
		print("INVALID INT\n")
	} else {
		print("VALID INT ", resultI, "\n")
	}

	resultF := 0.0
	resultF, err = inputf()
	if (err ==1) {
		print("INVALID FLOAT\n")
	} else {
		print("VALID FLOAT ", resultF, "\n")
	}

	resultS := ""
	resultS, err = inputs()
	if (err ==1) {
		print("INVALID STRING\n")
	} else {
		print("VALID STRING ", resultS, "\n")
	}
}
