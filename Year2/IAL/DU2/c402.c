
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                                 Karel Masařík, říjen 2013
**                                                 Radek Hranický 2014-2018
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/

	if (Ptr==NULL)
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;
}

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptr;
	}
}

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);
	}
	else {
		return (S->a[S->top--]);
	}
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;
}

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK)
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;
		S->a[S->top]=val;
	}
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);
	}
	else {
		return(S->a[S->top--]);
	}
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat.
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
	
	*RootPtr = NULL;

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/

	tBTNodePtr tmp = *RootPtr, leaf_pos = NULL;
	bool found = false, empty_tree = false;

	if (*RootPtr == NULL)
	{
		empty_tree = true;
	}

	while (!found && tmp != NULL)
	{
		leaf_pos = tmp;				//< pozicia aktualneho listu v pripade vkladania
		if (tmp->Cont > Content)
		{
			tmp = tmp->LPtr;
		}
		else if (tmp->Cont < Content)
		{
			tmp = tmp->RPtr;
		}
		else
		{
			found = true;
		}
	}

	if (!found)		//< vkladame len ak uzol este nie je v strome
	{
		tBTNodePtr new_item = malloc(sizeof(struct tBTNode));
		if (new_item == NULL)
		{
			return;
		}
		
		new_item->Cont = Content;
		new_item->LPtr = NULL;
		new_item->RPtr = NULL;
		if (empty_tree)				// vkladany uzol je prvy, stane sa korenom
		{
			*RootPtr = new_item;
		}
		else
		{
			if (leaf_pos->Cont > Content)	//< vkladany uzol bude lavym listom
			{
				leaf_pos->LPtr = new_item;
			}
			else							//< vkladany uzol bude pravym listom
			{
				leaf_pos->RPtr = new_item;
			}
		}
			
	}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/

	while (ptr != NULL)
	{
		SPushP(Stack, ptr);
		BTWorkOut(ptr);
		ptr = ptr->LPtr;
	}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut().
**/

	if (RootPtr != NULL)
	{
		tBTNodePtr item = RootPtr;
		tStackP stack;

		SInitP(&stack);
		Leftmost_Preorder(item, &stack);
		while (!SEmptyP(&stack))
		{
			item = STopPopP(&stack);
			Leftmost_Preorder(item->RPtr, &stack);
		}
	}
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}


/*                                  INORDER                                   */

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku.
**/

	while (ptr != NULL)
	{
		SPushP(Stack, ptr);
		ptr = ptr->LPtr;
	}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */

}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut().
**/

	if (RootPtr != NULL)
	{
		tBTNodePtr item = RootPtr;
		tStackP stack;

		SInitP(&stack);
		Leftmost_Inorder(item, &stack);
		while (!SEmptyP(&stack))
		{
			item = STopPopP(&stack);
			BTWorkOut(item);
			Leftmost_Inorder(item->RPtr, &stack);
		}
	}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

/*                                 POSTORDER                                  */

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat.
**/

	while (ptr != NULL)
	{
		SPushP(StackP, ptr);
		SPushB(StackB, true);
		ptr = ptr->LPtr;
	}
	
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut().
**/

	if (RootPtr != NULL)
	{
		tBTNodePtr item = RootPtr;
		tStackP stack;
		tStackB stack_b;
		bool left;

		SInitP(&stack);
		SInitB(&stack_b);
		Leftmost_Postorder(item, &stack, &stack_b);
		while (!SEmptyP(&stack))
		{
			item = STopPopP(&stack);
			SPushP(&stack, item);		
			left = STopPopB(&stack_b);

			if (left)		//< prichod zlava, otec sa este nespracuje
			{
				SPushB(&stack_b, false);
				Leftmost_Postorder(item->RPtr, &stack, &stack_b);
			}
			else			//< prichod sprava, otec sa spracuje
			{
				STopPopP(&stack);
				BTWorkOut(item);
			}
			
		}
	}

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/

	tStackP stack;
	SInitP(&stack);

	do {
		if (*RootPtr == NULL)
		{
			if (!SEmptyP(&stack))
			{
				*RootPtr = STopPopP(&stack);
			}
		}
		else
		{
			if ((*RootPtr)->RPtr != NULL)
			{
				SPushP(&stack, (*RootPtr)->RPtr);
			}
			tBTNodePtr tmp = *RootPtr;
			*RootPtr = (*RootPtr)->LPtr;
			free(tmp);
		}	

	} while(*RootPtr != NULL || !SEmptyP(&stack));
	

	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */
}

/* konec c402.c */

