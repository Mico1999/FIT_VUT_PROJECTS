
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

 L->Act = NULL;
 L->First = NULL;
 L->Last = NULL;   
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/

 tDLElemPtr temp_elem;

 while (L->First != NULL)       /* pokial nedojdeme na koniec zoznamu*/
 {
    temp_elem = L->First;
    L->First = L->First->rptr;
    free(temp_elem);
 }

 L->Act = NULL;
 L->Last = NULL;

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

 tDLElemPtr new_item = malloc(sizeof(struct tDLElem));
 if (new_item != NULL)
 {
     new_item->data = val;
     new_item->lptr = NULL;
     new_item->rptr = L->First;

     if (L->First != NULL)      /*ak sa pridava aspon 2.prvok tak zmenim ukaz. na predchodcu, inak zostane NULL*/
        L->First->lptr = new_item;

     L->First = new_item;

     if (L->First->rptr == NULL)    /*ak sa jedna o prvy vlozeny=> je to zaroven posledny*/
        L->Last = L->First;
 }
 else
 {
     DLError();
 }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	

 tDLElemPtr new_item = malloc(sizeof(struct tDLElem));
 if (new_item != NULL)
 {
     new_item->data = val;
     new_item->rptr = NULL;
     new_item->lptr = L->Last;
     
     if (L->Last != NULL)       /*ak sa pridava na koniec aspon 2.prvok tak zmenim naslednika, inak zostane NULL*/ 
        L->Last->rptr = new_item;
     
     L->Last = new_item;

     if (L->Last->lptr == NULL)     /* ak posledny prvok je zaroven jediny => tak je aj prvy*/
        L->First = L->Last;
 } 	
 else
 {
     DLError();
 }
 
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
 L->Act = L->First;

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
 L->Act = L->Last;

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

 if (L->First != NULL)      /* volanie funkcie neskonci chybou len ak je zoznam neprazdny */
 {
     *val = L->First->data;
 }
 else
 {
     DLError();
 }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

 if (L->First != NULL )     /* volanie funkcie neskonci chybou len ak je zoznam neprazdny */
 {
     *val = L->Last->data;
 }
 else
 {
     DLError();
 }
 		
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

 if (L->First != NULL)      /* funkcia sa vykona len ak je zoznam neprazdny */
 {
     if (L->First == L->Act)
        L->Act = NULL;
    
     tDLElemPtr temp_elem = L->First;
     L->First = L->First->rptr;

     if (L->First == NULL)      /*ak bol v zozname len 1 prvok => ukaz. na posledny je NULL*/
     {   
        L->Last = NULL;
     }
     else                       /* inak mozme pristupit k ukaz. na pred. prvok */
     {
        L->First->lptr = NULL;
     }
    
     free(temp_elem); 
 }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L.
** Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/ 
	
 if (L->First != NULL)      /* funkcia sa vykona len ak je zoznam neprazdny */
 {
     if (L->Last == L->Act)
        L->Act = NULL;
    
     tDLElemPtr temp_elem = L->Last;
     L->Last = L->Last->lptr;

     if (L->Last == NULL)      /*ak bol v zozname len 1 prvok => ukaz. na prvy je NULL*/
     {   
        L->First = NULL;
     }
     else                       /* inak mozme pristupit k ukaz. na nasled. prvok */
     {
        L->Last->rptr = NULL;
     }
    
     free(temp_elem); 
 }	

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/

 if (L->Act != NULL && L->Act->rptr != NULL)        /* funkcia vykona len ak je zoznam aktivny a nie je to posledny prvok */
 {
    tDLElemPtr temp_elem = L->Act->rptr;
    if (temp_elem == L->Last)               /* ak vymazavame posledny prvok */
    {
        L->Last = L->Act;
        L->Act->rptr = NULL; 
    }
    else
    { 
        L->Act->rptr = temp_elem->rptr;
        temp_elem->rptr->lptr = L->Act;
    }

    free(temp_elem);
 }	
		
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/

 if (L->Act != NULL && L->Act->lptr != NULL)        /* funkcia vykona len ak je zoznam aktivny a nie je to prvy prvok */	
 {
     tDLElemPtr temp_elem = L->Act->lptr;
     if (temp_elem == L->First)                 /* ak vymazeme prvy prvok */
     {
         L->First = L->Act;
         L->Act->lptr = NULL;
     }
     else
     {
         L->Act->lptr = temp_elem->rptr;
         temp_elem->lptr->rptr = L->Act;
     }
     
     free(temp_elem);
 }

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

 if (L->Act != NULL)        /* len ak je zoznam aktivny */
 {
     tDLElemPtr new_item = malloc(sizeof(struct tDLElem));
     if (new_item != NULL)
     {
         if (L->Act == L->Last)     /*ak vkladame za posledny prvok*/
         {
            new_item->rptr = NULL;
            L->Last = new_item;
         }
         else
         {
             new_item->rptr = L->Act->rptr;
             new_item->rptr->lptr = new_item;
         }

         new_item->data = val;
         new_item->lptr = L->Act;
         L->Act->rptr = new_item;   

     }
     else 
     {
         DLError();
     }
 }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

if (L->Act != NULL)     /*len ak je zoznam aktivny */
 {
     tDLElemPtr new_item = malloc(sizeof(struct tDLElem));
     if (new_item != NULL)
     {
         if (L->Act == L->First)     /*ak vkladame pred prvy prvok*/
         {
            new_item->lptr = NULL;
            L->First = new_item;
         }
         else
         {
             new_item->lptr = L->Act->lptr;
             new_item->lptr->rptr = new_item;
         }

         new_item->data = val;
         new_item->rptr = L->Act;
         L->Act->lptr = new_item;   

     }
     else 
     {
         DLError();
     }
 }	
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

 if (L->Act != NULL)        /* zoznam musi byt aktivny */ 
 {
     *val = L->Act->data;
 }
 else
 {
     DLError();
 }

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/

 if (L->Act != NULL)       /* zoznam musi byt aktivny */
 {
     L->Act->data = val;
 }	
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/

 if (L->Act != NULL)        /* zoznam musi byt aktivny */
 {
     L->Act = L->Act->rptr;
 }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/

 if (L->Act != NULL)        /* zoznam musi byt aktivny */
 {
     L->Act = L->Act->lptr;
 }	
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	
 return ((L->Act != NULL) ? 1 : 0);
 	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

/* Konec c206.c*/
