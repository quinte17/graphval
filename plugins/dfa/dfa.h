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

#ifndef DFA_H
#define DFA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximal 200 Woerter erlauben
#define WORTMAX 200

// Stringlaenge beschraenken
#define STRMAX 256

#define TRUE 1
#define FALSE 0

/*  Baumstruktur fuer Alphabet (Woerter)
 */ struct alphabaum {
	int idx;
	char *id;
	struct alphabaum *links;
	struct alphabaum *rechts;
};

/*  Funktionszeiger
 */ typedef void (*Uebergang) (int Zustand, char *Eingabe);

struct Tabelle_Eintrag {
	int Folgezustand;
	Uebergang Funktion;
};

/*  Globale Variablen
 */
int Anzahl_Zeichen;
int Anzahl_Zustaende;
int Anzahl_Endzustaende;
int Startzustand;

struct alphabaum *woerter;
struct Tabelle_Eintrag** Tabelle_intern;

int akt_Zustand;
char *Zeichen;

/*  Falls 1 werden einzelzeichen gelesen, ansonsten ganze Woerter
 */ int zeicheneingabe;

/*  Grafische Darstellung eines Uebergangs
 */ void Aktion(int Zustand, char *Zeichen);

/*  Fehler, wenn Zeichen nicht akzeptiert wird
 */ void Fehler(int Zustand, char *Zeichen);

/*  Algorithmus
 */ void Automat(void);

/*  Initialisiert interne Tabelle
 */ int malloc_Tabelle(int Anzahl_Zeichen, int Anzahl_Zustaende);

/*  Wenn ganze Zeichenkette akzeptiert
 */ void ok(int Zustand);

/*  Lesen von Einzelzeichen
 */ char* lesen_eingabe();

/*  Legt ein Array mit dem zu akzeptierenden Alphabet an
 */ void setze_alphabet();

/*  Prueft ob aktuelles Wort im Alphabet und gibt die Position zurueck.
 *  Bei Fehler: -1
 */ int eingabe_ok(char *id);

/*  Fuegt elemente zum Baum hinzu
 */ void wort_hinzufuegen(char *id, int idx);

#endif
