// IFJ20: inputs n-times

package main

func main() {
  print("Zadej pocet radku k nacteni: ")
  n, _ := inputi()
  print("Nacitame ", n, " radku...\n")

  for i := 1; i <= n; i = i + 1 {
    print("Zadej retezec c. ", i, ": ")
    s, err := inputs()
    print("STRING", i, "=", s, ".", " CHYBA=", err, "\n")  
  }
  
}
// tento subor je  asi spatne lebo s, err := to sa u nas nepovuluje