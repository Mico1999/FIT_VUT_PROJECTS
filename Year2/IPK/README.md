IPK 2020/2021, projekt č.2
==========================

Varianta OMEGA: Scanner síťových služieb
----------------------------------------

### Autor: Marek Miček, xmicek08
### Priložené súbory:
* ipk-l4-scan.cpp   (výsledný program implementujúci scanner sieťových služieb)
* ipk-l4-scan.hpp   (hlavičkový súbor k výslednému programu)
* Makefile  (súbor slúžiaci k prekladu výsledného programu)
* manual.pdf (dokumnetácia k výslednému programu)
* README.md (tento súbor)

### Popis programu:
Program ipk-l4-scan je implementovaný v jazyzku C++, pričom sa jedná o jednoduchý TCP/UDP skener,
ktorý na zadanom rozhraní a IP adrese oskenuje porty pomocou UDP/TCP-SYN skenovania,
pričom čísla portov a sposob skenovania si vyberie užívateľ.

### Preklad programu:
Preklad zdrojového súboru realizujeme zadaním príkazu make

### Spustenie programu:
Program spustíme nasledovne: ./ipk-l4-scan [-i interface | --interface *interface*] [--pu port-ranges | --pt port-ranges | -u port-ranges | -t port-ranges] {-w timeout | --wait timeout} [domain-name | ip-address]
kde:

* -i interface | --interface *interface*: definuje rozhranie na ktorom bude skenovanie prebiehať,
v prípade, že tento parameter chýba, vypíše sa zoznam všetkých aktívnych rozhraní a ukončí sa program
* --pu port-ranges | -u port-ranges: definuje zoznam portov skenovaných pomocou UDP,
pričom povolený zápis je napríklad --pu 12, --pu 12-20, --pu 12,13,20
* --pt port-ranges | -t port-ranges: definuje zoznam portov skenovaných pomocou TCP-SYN,
pričom povolený zápis je napríklad --pt 12, --pt 12-20, --pt 12,13,20
* -w timeout | --wait timeout: definuje čas v milisekundách, ktorým sa simuluje čakanie na odpoveď skenovaného portu,
v prípade, že tento parameter chýba, požije sa implicitná hodnota 5000 ms
* domain-name | ip-address: definuje doménové meno/IP adresu skenovaného stroja

Zátvorky typu [] popisujú povinný parameter a zátvorky typu {} nepovinný (v prípade neprítomnosti sa použije imlicitná hodnota).
Argumenty môžu byť v ľubovolnom poradí.

### Odstránenie binárnych súborov po preklade:
Realizujeme zadaním príkazu make clean
