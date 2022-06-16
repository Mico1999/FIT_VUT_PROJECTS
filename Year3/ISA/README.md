ISA 2021/2022, klient POP3 s podporou TLS
==========================

### Autor: Marek Miček, xmicek08
### Priložené súbory:
* popcl.cpp   (výsledný program implementujúci POP3 klienta s podporou TLS)
* popcl.hpp   (hlavičkový súbor k výslednému programu)
* Makefile  (súbor slúžiaci k prekladu výsledného programu)
* manual.pdf (dokumnetácia k výslednému programu)
* README.md (tento súbor)

### Popis programu:
Program popcl je implementovaný v jazyzku C++, pričom sa jedná klientskú sieťovú aplikáciu,
ktorá využíva POP3 protokol k čítaniu elektronickej pošty. Klient dokáže všetky správy zo servera stiahnúť
do svojho lokálneho adresára, vymazať tieto správy zo servera, prípadne umožňí sťahovanie len nových správ, 
ktoré došli na server od posledného sťahovania. Okrem klasickej nešifrovanej komunikácie, klient podporuje
aj šifrovanú variantu, buď použitím POP3s protokolu alebo príkazom STARTTLS (viz manual.pdf).

### Preklad programu:
Preklad zdrojového súboru realizujeme zadaním príkazu make

### Spustenie programu:
Program sa spúšťa nasledovne: ./popcl <server> [-p <port>] [-T|-S [-c <certfile>]
[-C <certaddr>]] [-d] [-n] -a <auth_file> -o <out_dir>

* Povinne je uvedený názov <server> (IP adresa, nebo doménové meno) požadovaného zdroja

* Voliteľný parameter -p špecifikuje číslo portu <port> na serveru

* Parameter -T zapína šifrovanie celej komunikácie (pop3s), ak parameter nie je uvedený použije sa nešifrovaná varianta protokolu

* Parameter -S naviaže nešifrované spojení so serverom a pomocou príkazu STLS (RFC 2595) prejde na šifrovanou variantu protokolu

* Voliteľný parameter -c definuje súbor <certfile> s certifikáty, ktorý sa použije pre overenie platnosti certifikátu SSL/TLS predloženého serverom (len s parametrom -T, alebo -S)
* Voliteľný parameter -C určuje adresár <certaddr>, v ktorom sa majú vyhľadať certifikáty, ktoré sa použijú pro overenie platnosti certifikátu SSL/TLS predloženého serverom (len s parametrom -T, alebo -S.) Ak nie je uvedený parameter -c ani -C, tak sa použije úložisko certifikátov získané funkciou SSL_CTX_set_default_verify_paths()
* Pri použití parametru -d sa zašle serveru príkaz pro zmazanie správ, klient vyžaduje, aby sa nekombinoval s parametrom -n, vymazané správy sa predom neuložia do lokálneho adresára, takže je potrebné používať daný parameter opatrne

* Pri použití parametru -n sa bude pracovať len s novými správami, klient vyžaduje, aby sa nekombinoval s parametrom -d

* Povinný parameter -a <auth_file> udáva súbor s autentifikačnými údajmi

* Povinný parameter -o <out_dir> špecifikuje výstupní adresár <out_dir>, do ktorého má program stiahnuté správy uložiť


### Odstránenie binárnych súborov po preklade:
Realizujeme zadaním príkazu make clean