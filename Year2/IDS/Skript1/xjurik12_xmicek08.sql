-- SQL srcipt for generating of basic objects in database
-- Task 59: Social site
-- Authors: Matej Jurík, Marek Miček
-- Logins: xjurik12, xmicek08
-- Date: 3.5.2021

DROP TABLE Zamestnanie CASCADE CONSTRAINTS;
DROP TABLE Uzivatel CASCADE CONSTRAINTS;
DROP TABLE Je_priatel_s CASCADE CONSTRAINTS;
DROP TABLE Konverzacia CASCADE CONSTRAINTS;
DROP TABLE Sprava CASCADE CONSTRAINTS;
DROP TABLE Uzivatel_prislucha_od CASCADE CONSTRAINTS;
DROP TABLE Prispevok CASCADE CONSTRAINTS;
DROP TABLE Akcia CASCADE CONSTRAINTS;
DROP TABLE Uzivatel_zapisal CASCADE CONSTRAINTS;
DROP TABLE Album CASCADE CONSTRAINTS;
DROP TABLE Fotka CASCADE CONSTRAINTS;
DROP TABLE Skola CASCADE CONSTRAINTS;
DROP TABLE Studuje CASCADE CONSTRAINTS;
DROP TABLE Vztah CASCADE CONSTRAINTS;
DROP TABLE Rodina CASCADE CONSTRAINTS;
DROP TABLE Vytvoril_vztah_typu CASCADE CONSTRAINTS;


---------------------------------- Creating tables ---------------------------------------------------

CREATE TABLE Zamestnanie (
    zamestnanie_id INT NOT NULL PRIMARY KEY,
    Pozicia VARCHAR(50) NOT NULL,
    Zamestnavatel VARCHAR(50) NOT NULL,
    Datum_od DATE NOT NULL,
    Ulica VARCHAR(50) NOT NULL,
    Mesto VARCHAR(50) NOT NULL,
    Krajina VARCHAR(50) NOT NULL
);

CREATE TABLE Uzivatel (
    rodne_cislo VARCHAR(50) NOT NULL PRIMARY KEY,
    Meno VARCHAR(50) NOT NULL,
    Heslo VARCHAR(50) NOT NULL UNIQUE,
    Ulica VARCHAR(50) NOT NULL,
    Mesto VARCHAR(50) NOT NULL,
    Krajina VARCHAR(50) NOT NULL,
    Telefon VARCHAR(50) NOT NULL,
    Email VARCHAR(50) NOT NULL UNIQUE
        CHECK(REGEXP_LIKE(Email, '^(\S+)\@(\S+)\.(\S+)$')),
    Admin VARCHAR(50) NOT NULL,
    zamestnanie_id_FK INT NULL,
    FOREIGN KEY(zamestnanie_id_FK) REFERENCES Zamestnanie(zamestnanie_id) on DELETE SET NULL 
);

CREATE TABLE Je_priatel_s (
    rodne_cislo_1_FK VARCHAR(50) NOT NULL,
    rodne_cislo_2_FK VARCHAR(50) NOT NULL,
    PRIMARY KEY(rodne_cislo_1_FK, rodne_cislo_2_FK),
    FOREIGN KEY(rodne_cislo_1_FK) REFERENCES UZIVATEL(rodne_cislo) ON DELETE CASCADE,
    FOREIGN KEY(rodne_cislo_2_FK) REFERENCES UZIVATEL(rodne_cislo) ON DELETE CASCADE,
    Datum_od DATE NOT NULL
);

CREATE TABLE Konverzacia (
    konverzacia_id INT NOT NULL PRIMARY KEY,
    Nazov VARCHAR(50) NULL
);

CREATE TABLE Sprava (
    sprava_id INT NOT NULL PRIMARY KEY,
    Obsah CLOB NOT NULL,
    Datum DATE NOT NULL,
    Miesto VARCHAR(50) NOT NULL,
    rodne_cislo_FK VARCHAR(50) NOT NULL,
    konverzacia_id_FK INT NOT NULL,
    FOREIGN KEY(rodne_cislo_FK) REFERENCES Uzivatel(rodne_cislo) ON DELETE CASCADE,
    FOREIGN KEY(konverzacia_id_FK) REFERENCES Konverzacia(konverzacia_id) ON DELETE CASCADE
);

CREATE TABLE Uzivatel_prislucha_od (
    rodne_cislo_FK VARCHAR(50) NOT NULL,
    konverzacia_id_FK INT NOT NULL,
    PRIMARY KEY(rodne_cislo_FK, konverzacia_id_FK),
    FOREIGN KEY(rodne_cislo_FK) REFERENCES Uzivatel(rodne_cislo) on DELETE CASCADE,
    FOREIGN KEY(konverzacia_id_FK) REFERENCES Konverzacia(konverzacia_id) on DELETE CASCADE,
    Datum_od DATE NOT NULL
);

CREATE TABLE Prispevok (
    prispevok_id INT NOT NULL PRIMARY KEY,
    Obsah CLOB NOT NULL,
    Datum TIMESTAMP NOT NULL,
    Miesto VARCHAR(50) NOT NULL,
    rodne_cislo_FK VARCHAR(50) NOT NULL,
    FOREIGN KEY(rodne_cislo_FK) REFERENCES Uzivatel(rodne_cislo) on DELETE CASCADE
);

CREATE TABLE Akcia (
    akcia_id INT NULL PRIMARY KEY,          -- Demonštrácia auto_incrementu cez trigger
    Miesto_konania VARCHAR(50) NOT NULL,
    Datum DATE NOT NULL,
    Popis CLOB NOT NULL,
    rodne_cislo_FK VARCHAR(50) NOT NULL,
    FOREIGN KEY(rodne_cislo_FK) REFERENCES Uzivatel(rodne_cislo) on DELETE SET NULL    
);

CREATE TABLE Uzivatel_zapisal (
    rodne_cislo_FK VARCHAR(50) NOT NULL,
    akcia_id_FK INT NOT NULL,
    PRIMARY KEY(rodne_cislo_FK, akcia_id_FK),
    FOREIGN KEY(rodne_cislo_FK) REFERENCES Uzivatel(rodne_cislo) on DELETE CASCADE,
    FOREIGN KEY(akcia_id_FK) REFERENCES Akcia(akcia_id) on DELETE CASCADE,
    Datum_zapisania DATE NOT NULL
);

CREATE TABLE Album (
    album_id INT NOT NULL PRIMARY KEY,
    Datum_vytvorenia DATE NOT NULL,
    Nazov VARCHAR(50) NOT NULL,
    Viditelnost VARCHAR(20) NOT NULL,
    Popis CLOB NOT NULL,
    Nahlad_umiestnenie VARCHAR(50) NOT NULL,
    rodne_cislo_FK VARCHAR(50) NOT NULL,
    FOREIGN KEY(rodne_cislo_FK) REFERENCES Uzivatel(rodne_cislo) on DELETE CASCADE 
);

CREATE TABLE Fotka (
    fotka_id INT NOT NULL PRIMARY KEY,
    Umiestnenie VARCHAR(50) NOT NULL,
    Datum DATE NOT NULL,
    Miesto VARCHAR(50) NOT NULL,
    album_id_FK INT NOT NULL,
    akcia_id_FK INT NULL,
    FOREIGN KEY(album_id_FK) REFERENCES Album(album_id) on DELETE CASCADE,
    FOREIGN KEY(akcia_id_FK) REFERENCES Akcia(akcia_id) on DELETE SET NULL
);

CREATE TABLE Skola (
    skola_id INT NOT NULL PRIMARY KEY,
    Nazov VARCHAR(50) NOT NULL,
    Ulica VARCHAR(50) NOT NULL,
    Mesto VARCHAR(50) NOT NULL,
    Krajina VARCHAR(50) NOT NULL
);

CREATE TABLE Studuje (
    rodne_cislo_FK VARCHAR(50) NOT NULL,
    skola_id_FK INT NOT NULL,
    PRIMARY KEY(rodne_cislo_FK, skola_id_FK),
    FOREIGN KEY(rodne_cislo_FK) REFERENCES Uzivatel(rodne_cislo) on DELETE CASCADE,
    FOREIGN KEY(skola_id_FK) REFERENCES Skola(skola_id) on DELETE CASCADE,
    Datum_od DATE NOT NULL,
    Datum_do DATE NOT NULL
);

CREATE TABLE Vztah (
    vztah_id INT NOT NULL,
    uzivatel_id VARCHAR(50) NOT NULL,
    PRIMARY KEY(vztah_id, uzivatel_id),
    FOREIGN KEY(uzivatel_id) REFERENCES Uzivatel(rodne_cislo) on DELETE CASCADE,
    Datum_od DATE NOT NULL
);

CREATE TABLE Rodina (
    vztah_id_FK INT NOT NULL,
    uzivatel_id_FK VARCHAR(50) NOT NULL,
    PRIMARY KEY(vztah_id_FK, uzivatel_id_FK),
    FOREIGN KEY(vztah_id_FK, uzivatel_id_FK) REFERENCES Vztah(vztah_id, uzivatel_id) on DELETE CASCADE,
    Rodnne_meno VARCHAR(50) NULL
);

CREATE TABLE Vytvoril_vztah_typu (
    rodne_cislo_FK VARCHAR(50) NOT NULL,
    vztah_id_FK INT NOT NULL,
    uzivatel_id_FK VARCHAR(50) NOT NULL,
    PRIMARY KEY(rodne_cislo_FK, vztah_id_FK, uzivatel_id_FK),
    FOREIGN KEY(rodne_cislo_FK) REFERENCES Uzivatel(rodne_cislo) on DELETE CASCADE,
    FOREIGN KEY(vztah_id_FK, uzivatel_id_FK) REFERENCES Vztah(vztah_id, uzivatel_id) on DELETE CASCADE,
    Typ_vztahu VARCHAR(50) NOT NULL
);

----------------------------------    TRIGGERS   -----------------------------------------------------------------

-- Trigger pre automatické generovanie primárného kľúča, pokiaľ je nová hodnota PK nedefinovaná
-- Trigger uplatňujeme na tabuľku Akcia, pre stĺpec akcia_id do ktorého vkladáme hodnoty NULL
-- Najskôr si vytvoríme sekvenciu začínajúcu číslom 1

CREATE SEQUENCE AUTO_INC
START WITH 1
INCREMENT BY 1;

CREATE OR REPLACE TRIGGER Auto_Inc_ID
	BEFORE INSERT ON Akcia
	FOR EACH ROW
BEGIN
	IF :NEW.akcia_id IS NULL THEN
		:NEW.akcia_id := AUTO_INC.NEXTVAL;
	END IF;
END Auto_Inc_ID;
/

--DROP TRIGGER Auto_Inc_ID;
--DROP SEQUENCE AUTO_INC;


-- Trigger pre kontrolu vkladaného rodného čísla
-- Uvažujeme len pre ročníky narodenia 1954 a mladší 

CREATE OR REPLACE TRIGGER Check_Born_Num
    BEFORE INSERT OR UPDATE OF rodne_cislo ON Uzivatel
	FOR EACH ROW
DECLARE
    RC Uzivatel.rodne_cislo%TYPE;
    MESIAC VARCHAR(2);
    DEN VARCHAR(2);
BEGIN
    RC := :NEW.rodne_cislo;
	MESIAC := SUBSTR(RC, 3, 2);
	DEN := SUBSTR(RC, 5, 2);

    -- Skontrolujeme či sú v rodnom čísle len čísla
    IF (LENGTH(TRIM(TRANSLATE(RC, '0123456789', ' '))) != 0) THEN
        Raise_Application_Error (-20205, 'V rodnom čísle musia byť len čísla !');
    END IF;

    -- Skontrolujeme správnu dlžku čísla, deliteľnosť 11, rozsah jednotlivých položiek dátumu narodenia
    IF (LENGTH(RC) = 10) THEN
        IF ( MOD(RC, 11) != 0) THEN
			Raise_Application_Error (-20205, 'Rodne číslo musí byť deliteľné 11!');
		END IF;

        IF (MESIAC > 50) THEN
            MESIAC := MESIAC - 50;
        END IF;

        -- Skontrolujeme rozsah mesiaca narodenia
        IF (MESIAC <= 0 OR MESIAC > 12) THEN
            Raise_Application_Error (-20205, 'Mesiac narodenia mimo rozsah!');
        END IF;

        --Skontrolujeme rozsah dňa narodenia
        IF (DEN <= 0 OR DEN > 31) THEN
            Raise_Application_Error (-20205, 'Deň narodenia mimo rozsah!');

        ELSIF ((MESIAC = 4 OR MESIAC = 6 OR MESIAC = 9 OR MESIAC = 11) AND DEN > 30) THEN
		    Raise_Application_Error (-20205, 'Deň mimo rozsah');

        ELSIF (MESIAC = 2 AND DEN > 29) THEN
            Raise_Application_Error (-20205, 'Deň mimo rozsah');
        END IF;

    ELSE
        Raise_Application_Error (-20205, 'Neplatná dĺžka čísla !');
    END IF;

END Check_Born_Num;
/

--DROP TRIGGER CHECK_Born_Num;

---------------------------------- Inserting tables ---------------------------------------------------

INSERT INTO Zamestnanie
VALUES(1, 'Admin', 'Oracle', TO_DATE('2021-03-21', 'yyyy/mm/dd'), 'Jánska', 'Brno', 'Česká republika');
INSERT INTO Zamestnanie
VALUES(2, 'Developer', 'ATNT', TO_DATE('2021-04-21', 'yyyy/mm/dd'), 'Palackého 21', 'Brno', 'Česká republika');
INSERT INTO Zamestnanie
VALUES(3, 'Tester', 'IBM', TO_DATE('2021-10-01', 'yyyy/mm/dd'), 'Krížna', 'Bratislava', 'Slovensko');
INSERT INTO Zamestnanie
VALUES(4, 'Tester', 'Eset', TO_DATE('2021-12-04', 'yyyy/mm/dd'), 'Paláriková', 'Bratislava', 'Slovensko');
INSERT INTO Zamestnanie
VALUES(5, 'Internista', 'Nemocnica Praha', TO_DATE('2021-05-21', 'yyyy/mm/dd'), 'Rudá', 'Praha', 'Česká republika');

INSERT INTO Uzivatel
VALUES('9904274952', 'Marek Miček', 'hlavnezesmezdravy123', 'Hviezdoslavová 278', 'Beluša', 'Slovensko', '0907596242', 'marek.micek@azet.sk', 'Michal Kmeť', 1);
INSERT INTO Uzivatel
VALUES('0007237659', 'Matej Jurík', 'tojtak', 'Lednica 552', 'Lednica', 'Slovensko', '0950715776', 'wimko.king@gmail.com', 'Michal Kmeť', 2);
INSERT INTO Uzivatel
VALUES('0008236569', 'Peter Ruček', 'mamkaForever', 'Hloža 312', 'Hloža', 'Slovensko', '0948031871', 'pepo.rucek@gmail.com', 'Vitek Cestovateľ', 3);
INSERT INTO Uzivatel
VALUES('9956247565', 'Rebeka Černianská', 'lavalgrinda', 'Boca 12', 'Boca', 'Slovensko', '0908746235', 'rebe.beka@azet.sk', 'Peter Ruček', 4);
INSERT INTO Uzivatel
VALUES('9908082998', 'Peter Bajza', 'andulky02', 'Trenčianska 145', 'Beluša', 'Slovensko', '0904594242', 'pebaj.bajpe@gmai.com', 'Jaroslav Jurenka', 5);

INSERT INTO Je_priatel_s
VALUES('9904274952', '0007237659', TO_DATE('2019-08-09', 'yyyy/mm/dd'));
INSERT INTO Je_priatel_s
VALUES('9904274952', '0008236569', TO_DATE('2019-08-10', 'yyyy/mm/dd'));
INSERT INTO Je_priatel_s
VALUES('9904274952', '9956247565', TO_DATE('2019-11-17', 'yyyy/mm/dd'));
INSERT INTO Je_priatel_s
VALUES('9908082998', '0007237659', TO_DATE('2014-03-09', 'yyyy/mm/dd'));
INSERT INTO Je_priatel_s
VALUES('9908082998', '0008236569', TO_DATE('2012-05-11', 'yyyy/mm/dd'));

INSERT INTO Konverzacia
VALUES(1, 'Kvášov');
INSERT INTO Konverzacia
VALUES(2, 'FIT');
INSERT INTO Konverzacia
VALUES(3, NULL);
INSERT INTO Konverzacia
VALUES(4, NULL);
INSERT INTO Konverzacia
VALUES(5, 'Kryptošlechta');

INSERT INTO Sprava
VALUES(1, 'nazdar moj', TO_DATE('2019-03-09', 'yyyy/mm/dd'), 'Beluša', '9904274952', 1);
INSERT INTO Sprava
VALUES(2, 'pome cojko', TO_DATE('2020-03-09', 'yyyy/mm/dd'), 'Lednica', '0007237659', 3);
INSERT INTO Sprava
VALUES(3, 'uz si robil izg ?', TO_DATE('2018-03-09', 'yyyy/mm/dd'), 'Lednica', '0007237659', 1);

INSERT INTO Vztah
VALUES(1, '9956247565', TO_DATE('2021-02-09', 'yyyy/mm/dd'));
INSERT INTO Vztah
VALUES(2, '9904274952', TO_DATE('2019-10-09', 'yyyy/mm/dd'));
INSERT INTO Vztah
VALUES(2, '0008236569', TO_DATE('2019-10-09', 'yyyy/mm/dd'));
INSERT INTO Vztah
VALUES(2, '9908082998', TO_DATE('2019-10-09', 'yyyy/mm/dd'));

INSERT INTO Vytvoril_vztah_typu
VALUES('0008236569', 1, '9956247565', 'Partneri');
INSERT INTO Vytvoril_vztah_typu
VALUES('0007237659', 2, '9904274952', 'Rodina');
INSERT INTO Vytvoril_vztah_typu
VALUES('0007237659', 2, '0008236569', 'Rodina');
INSERT INTO Vytvoril_vztah_typu
VALUES('0007237659', 2, '9908082998', 'Rodina');

INSERT INTO Rodina
VALUES(1, '9956247565', NULL);
INSERT INTO Rodina
VALUES(2, '9904274952', 'Jaríci');
INSERT INTO Rodina
VALUES(2, '0008236569', 'Jaríci');
INSERT INTO Rodina
VALUES(2, '9908082998', 'Jaríci');

INSERT INTO Prispevok
VALUES(1, 'Noooo nist sa mi nechce', TO_TIMESTAMP('2020-12-04 12:01:12', 'YYYY-MM-DD HH24:MI:SS'), 'Tatry', '9904274952');
INSERT INTO Prispevok
VALUES(2, 'Goool ?', TO_TIMESTAMP('2019-02-14 09:42:25', 'YYYY-MM-DD HH24:MI:SS'), 'Barcelona', '0007237659');
INSERT INTO Prispevok
VALUES(3, 'Doobre moj tak zas zajtra.', TO_TIMESTAMP('2021-08-08 15:25:44', 'YYYY-MM-DD HH24:MI:SS'), 'Rimini', '0007237659');
INSERT INTO Prispevok
VALUES(4, 'Kde je zas ten Petrik?', TO_TIMESTAMP('2021-12-04 17:50:04', 'YYYY-MM-DD HH24:MI:SS'), 'Kriváň', '9908082998');
INSERT INTO Prispevok
VALUES(5, 'Naporiadku', TO_TIMESTAMP('2017-10-04 22:14:05', 'YYYY-MM-DD HH24:MI:SS'), 'Zadar', '9908082998');

INSERT INTO Akcia
VALUES(NULL, 'Brno', TO_DATE('2020-08-07', 'yyyy/mm/dd'), 'Fleda diskopárty', '9904274952');
INSERT INTO Akcia
VALUES(NULL, 'Piešťany', TO_DATE('2019-08-07', 'yyyy/mm/dd'), 'Grape festival', '0007237659');
INSERT INTO Akcia
VALUES(NULL, 'Bratislava', TO_DATE('2021-01-07', 'yyyy/mm/dd'), 'Havana Bratislava', '9904274952');
INSERT INTO Akcia
VALUES(NULL, 'Praha', TO_DATE('2021-01-07', 'yyyy/mm/dd'), 'Noc medikov', '9908082998');
INSERT INTO Akcia
VALUES(NULL, 'Bratislava', TO_DATE('2020-01-07', 'yyyy/mm/dd'), 'Havana Bratislava', '9956247565');
INSERT INTO Akcia
VALUES(NULL, 'Bratislava', TO_DATE('2021-01-03', 'yyyy/mm/dd'), 'Casey Ibiza párty', '9956247565');
INSERT INTO Akcia
VALUES(NULL, 'Praha', TO_DATE('2020-07-07', 'yyyy/mm/dd'), 'O2 aréna Nik Tendo', '9904274952');

INSERT INTO Uzivatel_zapisal
VALUES('9904274952', 1, TO_DATE('2020-08-07', 'yyyy/mm/dd'));
INSERT INTO Uzivatel_zapisal
VALUES('0007237659', 2, TO_DATE('2019-08-07', 'yyyy/mm/dd'));
INSERT INTO Uzivatel_zapisal
VALUES('9904274952', 3, TO_DATE('2021-01-07', 'yyyy/mm/dd'));
INSERT INTO Uzivatel_zapisal
VALUES('9908082998', 4, TO_DATE('2021-01-07', 'yyyy/mm/dd'));
INSERT INTO Uzivatel_zapisal
VALUES('9956247565', 5, TO_DATE('2020-08-07', 'yyyy/mm/dd'));
INSERT INTO Uzivatel_zapisal
VALUES('9904274952', 5, TO_DATE('2020-06-07', 'yyyy/mm/dd'));
INSERT INTO Uzivatel_zapisal
VALUES('9908082998', 6, TO_DATE('2019-04-17', 'yyyy/mm/dd'));
INSERT INTO Uzivatel_zapisal
VALUES('0007237659', 6, TO_DATE('2019-01-24', 'yyyy/mm/dd'));

INSERT INTO Album
VALUES(1, TO_DATE('2017-08-07', 'yyyy/mm/dd'), 'Deň študentov', 'view 1', 'Galéria fotiek zo dňa študentov 2017', 'Titulná fotka', '9904274952');
INSERT INTO Album
VALUES(2, TO_DATE('2019-01-04', 'yyyy/mm/dd'), 'Deň študentov', 'view 1', 'Galéria fotiek zo dňa študentov 2018', 'Titulná fotka', '9904274952');
INSERT INTO Album
VALUES(3, TO_DATE('2021-02-07', 'yyyy/mm/dd'), 'Túra Tatry 2020', 'view 1', 'Túra Tatry 2020', 'Titulná fotka', '0007237659');
INSERT INTO Album
VALUES(4, TO_DATE('2020-08-10', 'yyyy/mm/dd'), 'Dovolenka Zadar 2019', 'view 1', 'Najkrajšie momenty Zadar 2019', 'Titulná fotka', '9956247565');
INSERT INTO Album
VALUES(5, TO_DATE('2020-08-11', 'yyyy/mm/dd'), 'Lyžovačka Boca 2019', 'view 1', 'Best lyžovačka ever', 'Titulná fotka', '9956247565');

INSERT INTO Fotka
VALUES(1, 'view 1', TO_DATE('2017-08-07', 'yyyy/mm/dd'), 'Púchov', 1, NULL);
INSERT INTO Fotka
VALUES(2, 'view 2', TO_DATE('2017-08-07', 'yyyy/mm/dd'), 'Púchov', 1, NULL);
INSERT INTO Fotka
VALUES(3, 'view 1', TO_DATE('2020-07-07', 'yyyy/mm/dd'), 'Lomnický štít', 3, NULL);
INSERT INTO Fotka
VALUES(4, 'view 1', TO_DATE('2020-05-07', 'yyyy/mm/dd'), 'Štrbské pleso', 3, NULL);
INSERT INTO Fotka
VALUES(5, 'view 1', TO_DATE('2019-08-09', 'yyyy/mm/dd'), 'Zadar', 5, NULL);

INSERT INTO Skola
VALUES(1, 'FIT VUT Brno', 'Božetechova 1/2', 'Brno-Královo Pole', 'Česká republika');
INSERT INTO Skola
VALUES(2, 'FI Masarykova univerzita', 'Botanická 68A', 'Brno-Královo Pole-Ponava', 'Česká republika');
INSERT INTO Skola
VALUES(3, 'Gymnázium Púchov', 'Ulica 1. mája ', 'Púchov', 'Slovenská republika');
INSERT INTO Skola
VALUES(4, 'Gymnazium Bratislava', 'Sládkovičová 6', 'Bratislava-Ružinov', 'Slovenská republika');

INSERT INTO Studuje
VALUES('9904274952', 1, TO_DATE('2019-06-21', 'yyyy/mm/dd'), TO_DATE('2022-06-30', 'yyyy/mm/dd'));
INSERT INTO Studuje
VALUES('9908082998', 3, TO_DATE('2020-06-02', 'yyyy/mm/dd'), TO_DATE('2026-07-01', 'yyyy/mm/dd'));
INSERT INTO Studuje
VALUES('0007237659', 2, TO_DATE('2019-06-28', 'yyyy/mm/dd'), TO_DATE('2022-06-14', 'yyyy/mm/dd'));
INSERT INTO Studuje
VALUES('9956247565', 4, TO_DATE('2013-09-1', 'yyyy/mm/dd'), TO_DATE('2019-05-30', 'yyyy/mm/dd'));

----------------------------------    SELECT   -----------------------------------------------------------------

-- Klauzula GROUP BY s použitím agregačnej funkcie
-- Dotazujeme sa na celkový počet akcii v každom mieste konania akcie
SELECT COUNT(Akcia.akcia_id) AS Pocet, Akcia.Miesto_konania AS Mesto
FROM Akcia
GROUP BY Akcia.Miesto_konania
ORDER BY Pocet;

-- Klauzula GROUP BY s použitím agregačnej funkcie
-- Dotazujeme sa na mená všetkých užívatelov, ktorý majú viac ako jedného priateľa
-- Používame UNION, lebo sa musíme dotazovať na oba stlpce s cudzími kľúčami v tabuľke "Je_priatel_s"
SELECT Uzivatel.Meno AS Meno, COUNT(Je_priatel_s.rodne_cislo_2_FK) AS Priatelia
FROM Uzivatel
JOIN Je_priatel_s ON Uzivatel.rodne_cislo = Je_priatel_s.rodne_cislo_1_FK
HAVING COUNT(Je_priatel_s.rodne_cislo_2_FK) > 1
GROUP BY Uzivatel.Meno
UNION
SELECT Uzivatel.Meno AS Meno, COUNT(Je_priatel_s.rodne_cislo_1_FK) AS Priatelia
FROM Uzivatel
JOIN Je_priatel_s ON Uzivatel.rodne_cislo = Je_priatel_s.rodne_cislo_2_FK
HAVING COUNT(Je_priatel_s.rodne_cislo_1_FK) > 1
GROUP BY Uzivatel.Meno
ORDER BY Meno;

-- Predikát Exists
-- Dotazujeme sa na mená všetkých užívateľov, ktorý neštudujú na FIT VUT Brno
SELECT Uzivatel.Meno AS Meno
FROM Uzivatel
JOIN Studuje ON Uzivatel.rodne_cislo = Studuje.rodne_cislo_FK
WHERE NOT EXISTS (
    SELECT *
    FROM Skola
    WHERE Skola.skola_id = Studuje.skola_id_FK AND Skola.Nazov = 'FIT VUT Brno'
)
ORDER BY Meno;

-- Predikát IN
-- Dotazujeme sa na mená všetkých užívateľov, ktorý sa zapísali aspoň na jednu akciu
SELECT Uzivatel.Meno AS Meno
FROM Uzivatel
WHERE Uzivatel.rodne_cislo IN (
    SELECT DISTINCT Uzivatel_zapisal.rodne_cislo_FK
    FROM Uzivatel_zapisal
)
ORDER BY Meno;

-- Dotaz využívajúci spojenie dvoch tabuliek
-- Dotazujeme sa na názov, popis a dátum vytvorenia albumov konkrétnych užívateľov
SELECT
    Uzivatel.Meno as Meno
    ,Album.Nazov as Nazov
    ,Album.Popis as Popis
    ,Album.Datum_vytvorenia as Datum
FROM Uzivatel
JOIN Album
    ON Uzivatel.rodne_cislo = Album.rodne_cislo_fk
ORDER BY Uzivatel.Meno;

-- Dotaz využívajúci spojenie dvoch tabuliek
-- Dotazujeme sa na všetky príspevky každého užívateľa a ich prislúchajúce detaily
SELECT 
    Uzivatel.Meno as Meno
    ,Prispevok.Obsah as Prispevok
    ,Prispevok.Miesto as Miesto
    ,Prispevok.Datum as Datum
FROM Uzivatel
JOIN Prispevok on Uzivatel.rodne_cislo = Prispevok.rodne_cislo_fk
ORDER BY Uzivatel.Meno;

-- Dotaz využívajúci spojenie troch tabuliek
-- Dotazujeme sa na skolu konkrétnych uživateľov a dátumy udávajúce nástup uživateľa na danú školu
SELECT
    Uzivatel.Meno as Meno
    ,Skola.Nazov as Skola
    ,Studuje.Datum_od as Studuje_od
FROM Uzivatel
JOIN Studuje
    ON Uzivatel.rodne_cislo = Studuje.rodne_cislo_FK
JOIN Skola
    ON Skola.skola_id = Studuje.skola_id_FK
ORDER BY Uzivatel.Meno;

--------------------------------------- PERFORM TRIGGERS --------------------------------------------------------

-- Predvedenie auto inkrementu pomocou triggeru
-- Do tabuľky Akcia sme vložili 7 položiek
-- Mali by tam byť akcie s primárnými kľúčmi (akcia_id) od 1-7
SELECT akcia_id FROM Akcia;

-- Predvedenie kontroly vkladaného rodného čísla pomocou triggeru
-- Pripomíname, že sa omedzujeme na ročníky 1954 a mladšie
-- Všetky nasledujúce pokusy by mali vyhodiť výnimku

-- Vkladáme číslo nedeliteľné 11
INSERT INTO Uzivatel
VALUES('9904274951', 'Marek Miček', 'hlavnezesmezdravy123', 'Hviezdoslavová 278', 'Beluša', 'Slovensko', '0907596242', 'marek.micek@azet.sk', 'Michal Kmeť', 1);

-- Vkladáme číslo s písmenami
INSERT INTO Uzivatel
VALUES('ab07237659', 'Matej Jurík', 'tojtak', 'Lednica 552', 'Lednica', 'Slovensko', '0950715776', 'wimko.king@gmail.com', 'Michal Kmeť', 2);

-- Vkladáme číslo s mesiacom 13
INSERT INTO Uzivatel
VALUES('0013239490', 'Peter Ruček', 'mamkaForever', 'Hloža 312', 'Hloža', 'Slovensko', '0948031871', 'pepo.rucek@gmail.com', 'Vitek Cestovateľ', 3);

-- Vkladáme číslo s dňom 31 pre jún (jún ma 30 dní)
INSERT INTO Uzivatel
VALUES('9956317261', 'Rebeka Černianská', 'lavalgrinda', 'Boca 12', 'Boca', 'Slovensko', '0908746235', 'rebe.beka@azet.sk', 'Peter Ruček', 4);

-- Vkladáme moc dlhé číslo
INSERT INTO Uzivatel
VALUES('99080829987', 'Peter Bajza', 'andulky02', 'Trenčianska 145', 'Beluša', 'Slovensko', '0904594242', 'pebaj.bajpe@gmai.com', 'Jaroslav Jurenka', 5);

--------------------------------------- PRIVILEGES --------------------------------------------------------

GRANT ALL ON Zamestnanie TO xjurik12;
GRANT ALL ON Je_priatel_s TO xjurik12;
GRANT ALL ON Konverzacia TO xjurik12;
GRANT ALL ON Sprava TO xjurik12;
GRANT ALL ON Uzivatel_prislucha_od TO xjurik12;
GRANT ALL ON Prispevok TO xjurik12;
GRANT ALL ON Akcia TO xjurik12;
GRANT ALL ON Uzivatel_zapisal TO xjurik12;
GRANT ALL ON Album TO xjurik12;
GRANT ALL ON Fotka TO xjurik12;
GRANT ALL ON Skola TO xjurik12;
GRANT ALL ON Studuje TO xjurik12;
GRANT ALL ON Vztah TO xjurik12;
GRANT ALL ON Rodina TO xjurik12;
GRANT ALL ON Vytvoril_vztah_typu  TO xjurik12;

--------------------------------------- MATERIALIZED VIEW --------------------------------------------------------

-- Urobíme pohľad na všetkých užívateľov a ich zamestannie
CREATE MATERIALIZED VIEW JOBS AS
SELECT
    Uzivatel.Meno,
    Zamestnanie.Pozicia,
    Zamestnanie.Zamestnavatel
FROM Uzivatel
JOIN Zamestnanie
ON Uzivatel.zamestnanie_id_FK = Zamestnanie.zamestnanie_id
GROUP BY Uzivatel.Meno, Zamestnanie.Pozicia, Zamestnanie.Zamestnavatel;

GRANT ALL ON JOBS TO xjurik12;  -- pridelenie privilégii druhemu užívateľovi
SELECT * FROM JOBS;             -- výpis materializováneho pohľadu

UPDATE Zamestnanie
SET Zamestnanie.Pozicia = 'Developer'
WHERE Zamestnanie.zamestnanie_id = 3;   -- zmeníme pozíciu pre Petra Rúčka

SELECT * FROM JOBS;             -- dáta v materializovanom pohľade sa nezmenili

--DROP MATERIALIZED VIEW JOBS;



---------------------------------- PROCEDURES -----------------------------------------------------------------
-- Procedúra vypíše koľko užívateľov niekde študuje a zároveň percento
-- študujúcich uživateľov oproti celkovému počtu užívateľov
CREATE OR REPLACE PROCEDURE PERCENTO_STUDUJUCICH_UZIVATELOV
AS
    "UZIVATELIA_CELKOVO" NUMBER;
    "UZIVATELIA_STUDENTI" NUMBER;
    "PERCENTO_STUDUJUCICH" NUMBER;
BEGIN
    SELECT COUNT(*)
        INTO "UZIVATELIA_CELKOVO"
    FROM Uzivatel;
    
    SELECT COUNT(Uzivatel.Meno)
        INTO "UZIVATELIA_STUDENTI"
    FROM Uzivatel
    JOIN Studuje
        ON Uzivatel.rodne_cislo = Studuje.rodne_cislo_fk;

    "PERCENTO_STUDUJUCICH" := "UZIVATELIA_STUDENTI" / "UZIVATELIA_CELKOVO" * 100;

    DBMS_OUTPUT.put_line(
        'Celkovy pocet studujucich uzivatelov je ' 
        || "UZIVATELIA_STUDENTI" 
        || ' z ' 
        || "UZIVATELIA_CELKOVO"
    );

    DBMS_OUTPUT.put_line(
        "PERCENTO_STUDUJUCICH" || '% uzivatelov su studenti.'
    );

    EXCEPTION WHEN ZERO_DIVIDE THEN
    BEGIN
        IF "UZIVATELIA_CELKOVO" = 0 THEN
            DBMS_OUTPUT.put_line('Nenasli sa ziadni uzivatelia!');
        END IF;
    END;

END PERCENTO_STUDUJUCICH_UZIVATELOV;
/


-- Procedúra zistí počet akcií konajúcich sa na zadanom mieste
CREATE OR REPLACE PROCEDURE POCET_AKCII_NA_MIESTE
	(CIELOVE_MIESTO IN VARCHAR)
AS
	CIELOVY_POCET NUMBER;
    CELKOVY_POCET NUMBER;
    AKTUALNE_MIESTO Akcia.Miesto_konania%TYPE;
	CURSOR CUR_AKCIE IS SELECT Miesto_konania FROM Akcia;
BEGIN

    SELECT COUNT(*)
        INTO CELKOVY_POCET
    FROM Akcia;

	CIELOVY_POCET := 0;

	OPEN CUR_AKCIE;
	LOOP
		FETCH CUR_AKCIE INTO AKTUALNE_MIESTO;

		EXIT WHEN CUR_AKCIE%NOTFOUND;

		IF AKTUALNE_MIESTO = CIELOVE_MIESTO THEN
			CIELOVY_POCET := CIELOVY_POCET + 1;
		END IF;
	END LOOP;
	CLOSE CUR_AKCIE;

	DBMS_OUTPUT.put_line(
		'Na mieste ' 
        || CIELOVE_MIESTO
        || ' sa koná '
        || CIELOVY_POCET 
        || ' akcií z celkovo ' 
        || CELKOVY_POCET 
        || ' akcií.'
	);

	EXCEPTION WHEN NO_DATA_FOUND THEN
	BEGIN
		DBMS_OUTPUT.put_line(
			'Žiadne akcie sa na mieste ' || CIELOVE_MIESTO || ' nekonajú!'
		);
	END;

END;
/


-- Ukážka spustenia procedúry PERCENTO_STUDUJUCICH_UZIVATELOV
BEGIN 
    PERCENTO_STUDUJUCICH_UZIVATELOV; 
END;
/
-- Ukážka spustenia procedúry POCET_AKCII_NA_MIESTE
BEGIN
    POCET_AKCII_NA_MIESTE('Praha');
END;
/

---------------------------------- EXPLAIN PLAN -----------------------------------------------------------------

-- Zoznam užívateľov, ktorích administrátor je jeden z Kmeťov a majú viac ako jeden
-- príspevok

EXPLAIN PLAN FOR
SELECT
	Uzivatel.Meno AS Meno
	,COUNT(Prispevok.rodne_cislo_Fk) AS Pocet_Prispevkov
FROM Uzivatel
JOIN Prispevok
    ON Uzivatel.rodne_cislo = Prispevok.rodne_cislo_FK
WHERE Uzivatel.Admin LIKE '%Kmeť'
GROUP BY Uzivatel.Meno
    HAVING COUNT(Prispevok.rodne_cislo_FK) > 1
ORDER BY Uzivatel.Meno;

SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);


-- Zoskupenie Uzivatelov podla ich administrátora
CREATE INDEX Admini ON Uzivatel (Admin);

-- Po pridaní indexu
EXPLAIN PLAN FOR
SELECT
	Uzivatel.Meno AS Meno,
	COUNT(Prispevok.rodne_cislo_FK) AS Pocet_Prispevkov
FROM Uzivatel
JOIN Prispevok 
    ON Uzivatel.rodne_cislo = Prispevok.rodne_cislo_FK
WHERE Uzivatel.Admin LIKE '%Kmeť'
GROUP BY Uzivatel.Meno
    HAVING COUNT(Prispevok.rodne_cislo_FK) > 1
ORDER BY Uzivatel.Meno;

SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);
