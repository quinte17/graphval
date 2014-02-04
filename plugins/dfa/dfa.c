/*
	Graph Evaluation

	Copyright (C) 2009 Ludwig Frank
	Copyright (C) 2009 Stephan Sperber

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dfa.h"

extern char *Alphabet[];
extern int Endzustaende[];
extern struct Tabelle_Eintrag Tabelle[];	// Trick: Tabelle hier eindimensional 
struct Tabelle_Eintrag *TabelleP;

int malloc_Tabelle(int Anzahl_Zeichen, int Anzahl_Zustaende)
{
	int i;

	if(!(Tabelle_intern = malloc(Anzahl_Zustaende * sizeof(struct Tabelle_Eintrag*))))
	{
		fprintf(stderr,"Fehler beim Allokieren der Tabelle\n");
		exit(1);
	}
	for(i = 0; i < Anzahl_Zustaende; i++)
		if(!(Tabelle_intern[i] = malloc(Anzahl_Zeichen * sizeof(struct Tabelle_Eintrag))))
		{
			fprintf(stderr,"Fehler beim Allokieren der Tabelle\n");
			exit(1);
		}

	return 0;
}

/*  Grafische Darstellung eines Uebergangs
 */ void Aktion(int Zustand, char *Zeichen)
{
	printf("(Zustand %i) ---%s-->\n",Zustand, Zeichen);
}

/*  Fehler, wenn Zeichen nicht akzeptiert wird
 */ void Fehler(int Zustand, char *Zeichen)
{
	if(Zeichen != NULL)
		fprintf(stderr,"Fehler: (Zustand %i), Zeichen '%s'. beende...\n", Zustand, Zeichen);
	exit(1);
}

/*  Wenn ganze Zeichenkette akzeptiert
 */ void ok(int Zustand)
{
	printf("(Endzustand %i), Zeichenkette korrekt!\n", Zustand);
}

/*  Lesen von Woertern
 */ char* lesen_eingabe(void)
{
	char *tmp;
	if(zeicheneingabe == 0)
	{
		tmp = (char *) malloc(sizeof(char)*STRMAX);
		if(scanf("%s",tmp) == EOF)	
			return NULL;
	}
	else
	{
		tmp = (char *) malloc(sizeof(char)*2);
		tmp[0] = getchar();
		if(tmp[0] == '\n')
			return NULL;
		tmp[1] = '\0';
	}
	return tmp;
}

/*  Legt ein Array mit dem zu akzeptierenden Alphabet an
 */ void setze_alphabet()
{
	int i;
	zeicheneingabe = 1;

	for(i = 0; i < WORTMAX && Alphabet[i] != NULL; i++)
	{
		if(zeicheneingabe == 1 && strlen(Alphabet[i]) > 1)
			zeicheneingabe = 0;
		wort_hinzufuegen(Alphabet[i], i);
	}

	Anzahl_Zeichen = i;
}

/*  Prueft ob aktuelles Wort im Alphabet
 */ int eingabe_ok(char *id)
{
	struct alphabaum *tmp;
	tmp = woerter;
	while(tmp != NULL)
	{
		if(strncmp(tmp->id,id,STRMAX) < 0)
		{
			if(tmp->links != NULL)
			{
				tmp = tmp->links;
				continue;
			}
			else
			{
				fprintf(stderr,"Wort '%s' nicht im Alphabet!\n", id);
				return -1;
			}
		}
		else if(strncmp(tmp->id,id,STRMAX) > 0)
		{
			if(tmp->rechts != NULL)
			{
				tmp = tmp->rechts;
				continue;
			}
			else
			{
				fprintf(stderr,"Wort '%s' nicht im Alphabet!\n", id);
				return -1;
			}
		}
		else
			return tmp->idx;
	}

	fprintf(stderr,"Wort '%s' nicht im Alphabet!\n", id);
	return -1;
}

/*  Fuegt elemente zum Baum hinzu
 */ void wort_hinzufuegen(char *id, int idx)
{
	struct alphabaum *tmp, *tmp2;
	tmp = (struct alphabaum *) malloc(sizeof(struct alphabaum));
	if(tmp == NULL)
	{
		fprintf(stderr,"Nicht genug Speicher verfuegbar.\n");
		exit(1);
	}

	tmp->idx = idx;
	tmp->id = id;
	tmp->links = NULL;
	tmp->rechts = NULL;

	// Baum ist leer
	if(woerter == NULL)
	{
		woerter = tmp;
		return ;
	}

	/* Baum ist nicht leer
	 * Hier wird sortiert nach einem freien Platz gesucht und eingefuegt
	 * Der Baum rotiert sich nicht und ist deshalb unter umstaenden
	 * nicht gleichmaessig. Im schlechtesten Fall ist es eine Lineare Liste.
	 */
	tmp2 = woerter;
	while(tmp2 != NULL)
	{
		if(strncmp(tmp2->id,id,STRMAX) < 0)
		{
			if(tmp2->links != NULL)
			{
				tmp2 = tmp2->links;
				continue;
			}
			else
			{
				tmp2->links = tmp;
				return ;
			}
		}
		else if(strncmp(tmp2->id,id,STRMAX) > 0)
		{
			if(tmp2->rechts != NULL)
			{
				tmp2 = tmp2->rechts;
				continue;
			}
			else
			{
				tmp2->rechts = tmp;
				return ;
			}
		}
		else
			return ;
	}
}

/*  Algorithmus
 */ void Automat()
{
	int i, wort;
	
	while((Zeichen = lesen_eingabe()) != NULL)
	{
		i = eingabe_ok(Zeichen);
		if(i == -1)
			Fehler(akt_Zustand, Zeichen);
		else
			wort = i;

 		Tabelle_intern[akt_Zustand][wort].Funktion(akt_Zustand, Zeichen);
		akt_Zustand = Tabelle_intern[akt_Zustand][wort].Folgezustand;
	}

	for(i = 0; i < Anzahl_Endzustaende; i++)
	{
		if(akt_Zustand == Endzustaende[i])
		{
			ok(akt_Zustand);
			exit(0);
		}
	}

	fprintf(stderr,"Endzustand nicht erreicht!\n");
	Fehler(akt_Zustand, Zeichen);
}

int main(int argc, char* argv[])
{
	int i,j;
	TabelleP=&Tabelle[0];		// erstes Tabellenelement fuers Kopieren
//BEGINN DER INITIALISIERUNG
	printf("Initialisiere...");
	setze_alphabet();

	//Allokieren der internen Tabelle
	malloc_Tabelle(Anzahl_Zeichen, Anzahl_Zustaende);

	//Kopiere benutzerdefinierte Tabelle in die interne Tabelle
	for(i = 0; i < Anzahl_Zustaende; i++)
		for(j = 0; j < Anzahl_Zeichen; j++)
			Tabelle_intern[i][j] = TabelleP[i*(Anzahl_Zeichen)+j];

	akt_Zustand = Startzustand;

	printf("fertig.\n\nZeichen: %i Zustände: %i Endzustände: %i Startzustand: %i\n\n", Anzahl_Zeichen, Anzahl_Zustaende, Anzahl_Endzustaende, Startzustand);
//ENDE DER INITIALISIERUNG

	if(zeicheneingabe == 1)
		printf("Bitte gewünschte Zeichenkette eingeben (Beenden mit \\n):\n");
	else
		printf("Bitte gewünschte Woerter eingeben (Beenden mit Strg + d):\n");

	Automat();

	return 0;
}
