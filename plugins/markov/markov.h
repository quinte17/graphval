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
#ifndef MARKOV_H
#define MARKOV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/** \file markov.h
 *  Hier sind die notwendigen Funktionen zum rechnen mit Markovketten
 */

/** Maximale Stringlaenge zur Compilezeit anzugeben.
 */
#define STRMAX 256

/** Allgemeine Struktur einer Matrix mit 2 Dimensionen.
 */
typedef struct m_matrix {
	unsigned int number_rows;	/**< Anzahl der Zeilen */
	unsigned int number_columns;	/**< Anzahl der Spalten */

	float **m;			/**< Daten der Matrix. Zaehlanfang liegt hier bei 1 anstatt bei 0. */
} Matrix;
/** Alias der Elemente der Struktur Matrix.
 */
#define rows number_rows
/** Alias der Elemente der Struktur Matrix.
 */
#define cols number_columns

/** Struktur um die Namen der Markovkette aufzunehmen
 */
typedef struct m_names {
	unsigned int number_names;	/**< Anzahl der Namen */

	char **n;			/**< Stringarray der Daten. Zaehlanfang liegt hier bei 1 anstatt bei 0. */
} Nodenames;

/** \brief Gibt Fehlermeldung und die Dimensionen der angegebenen Matrizen aus.
 *
 *  Diese Funktion beendet das Program mit exit(1)
 *
 *  \param s Auszugebende Fehlermeldung
 *  \param a darf NULL sein
 *  \param b darf NULL sein
 */
void matrix_error(char *s, Matrix *a, Matrix *b);
/** Alias von matrix_error()
 */
#define m_err matrix_error

/** \brief Gibt die Werte der Matrix aus.
 *
 *  \param heading Ueberschrift der Ausgabe, darf NULL sein.
 *  \param m Auszugebende Matrix
 */
void matrix_print(const char *heading, Matrix *m);
/** Alias von matrix_print()
 */
#define m_print matrix_print

/** \brief Gibt die ueber dot generierte Matrix zurueck
 */
Matrix* matrix_transition();
/** Alias von matrix_transition()
 */
#define m_transit matrix_transition
/** Makro um bequem die Adjazenzmatrix zu bekommen.
 */
#define MATRIX_TRANSITION(a) Matrix *a = matrix_transition();

/** Erzeugt eine Matrix und initialisiere sie durchgehend mit 0.0
 *
 *  \param i Anzahl der Zeilen
 *  \param j Anzahl der Spalten
 *  \return Matrix der Dimension (i)x(j)
 */
Matrix* matrix_new(unsigned int i, unsigned int j);
/** Alias von matrix_new()
 */
#define m_new matrix_new
/** Makro um eine Variable des Typs Matrix zu definieren und zugleich
 *  eine neue Matrix (i)x(j) zuzuweisen.
 */
#define MATRIX(x,i,j) Matrix *x = matrix_new(i,j);

/** Initialisiere eine quadratische Einheitsmatrix.
 *
 *  \param i Anzahl der Zeilen und Spalten
 *  \return Matrix der Dimension (i)x(i)
 */
Matrix* matrix_identity(unsigned int i);
/** Alias von matrix_identity()
 */
#define m_ident matrix_identity

/** \brief Addiert Matrix a und Matrix b und gibt
 *  eine neue Matrix zurueck.
 *
 *  Matrix a und Matrix b muessen die gleiche Dimension haben.
 *  \f$ c = a + b \f$
 *
 *  \param a Matrix als erster Summand
 *  \param b Matrix als zweiter Summand
 *  \return Matrix als Wert der Summe
 */
Matrix* matrix_add(Matrix *a, Matrix *b);
/** Alias von matrix_add()
 */
#define m_add matrix_add

/** \brief Subtrahiert Matrix b von Matrix a und gibt
 *  eine neue Matrix zurueck.
 *
 *  Matrix a und Matrix b muessen die gleiche Dimension haben.
 *  \f$ c = a - b \f$
 *
 *  \param a Matrix als Minuend
 *  \param b Matrix als Subtrahend
 *  \return Matrix als Wert der Differenz
 */
Matrix* matrix_sub(Matrix *a, Matrix *b);
/** Alias von matrix_sub()
 */
#define m_sub matrix_sub

/** Multipliziert die Matrix a mit Matrix b und
 *  gibt eine neue Matrix (a.number_rows)x(b.number_columns) zurueck
 *  \f$ c = a * b \f$
 *
 *  \param a Matrix als Faktor
 *  \param b Matrix als Faktor
 *  \return Matrix als Wert des Produkts
 */
Matrix* matrix_mult(Matrix *a, Matrix *b);
/** Alias von matrix_mult()
 */
#define m_mult matrix_mult

/** Multipliziert die Matrix a mit dem Wert in f und
 *  gibt eine neue Matrix der gleichen Dimension zurueck.
 *
 *  \param a Matrix als Faktor
 *  \param f Wert als Faktor
 *  \return Matrix als Wert des Produkts
 */
Matrix* matrix_scalarmult(Matrix *a, float f);
/** Alias von matrix_scalarmult()
 */
#define m_smult matrix_scalarmult

/** Transponiert eine Matrix
 */
Matrix* matrix_transpose(Matrix *a);
/** Alias von matrix_transpose()
 */
#define m_transp matrix_transpose

/** Loese lineares Gleichungssystem mittels Gauss-Jordan-Algorithmus.
 *  Gibt die Loesungsmatrix zurueck.
 *  \f$ a * x = b \f$
 *
 *  \param a Matrix als linke Seite
 *  \param b Matrix als rechte Seite
 */
Matrix* matrix_gaussjordan(Matrix *a, Matrix *b);
/** Alias von matrix_gaussjordan()
 */
#define solve matrix_gaussjordan

/** Dupliziert die Matrix
 *  \f$ b = a \f$
 */
Matrix* matrix_copy(Matrix *a);
/** Alias von matrix_copy()
 */
#define m_copy matrix_copy

/** Ersetzt die Zeile i der Matrix a (1..m x 1..n)
 *  durch die Zeilenmatrix b (1 x 1..n) und gibt
 *  eine neue Matrix zurueck.
 *
 *  \param a Matrix in der eine Zeile ersetzt werden soll.
 *  \param i Die zu ersetzende Zeile der Matrix a.
 *  \param b Zeilenmatrix als Ersatz der Zeile.
 *  \return Matrix mit veraenderter Zeile i.
 */
Matrix* matrix_rowreplace(Matrix *a, unsigned int i, Matrix *b);
/** Alias von matrix_rowreplace()
 */
#define m_rrepl matrix_rowreplace

/** Modifiziert die Zeile i der Matrix
 *  und setzt die Werte auf den Wert von f und gibt
 *  eine neue Matrix zurueck.
 *
 *  \param a Matrix in der eine Zeile ersetzt werden soll.
 *  \param i Die zu ersetzende Zeile der Matrix a.
 *  \param f Konstante Zahl als Ersatz der Zeile.
 *  \return Matrix mit veraenderter Zeile i.
 */
Matrix* matrix_rowmodify(Matrix *a, unsigned int i, float f);
/** Alias von matrix_rowmodify()
 */
#define m_rowmodify matrix_rowmodify

/** Entfernt Spalte und Zeile k der Matrix a
 *  und gibt eine neue Matrix zurueck
 *
 *  \param a Zu verringernde Matrix.
 *  \param k Zeile und Spalte die von der Matrix a entfernt werden soll.
 *  \return Matrix ohne Zeile und Spalte k.
 */
Matrix* matrix_reduce(Matrix *a, unsigned int k);
/** Alias von matrix_reduce()
 */
#define m_reduce matrix_reduce

/** \brief Durchsuche die Matrix nach 1.0 auf der Diagonalen
 *
 *  Durchsucht die Diagonale der Matrix a nach dem Wert 1.0
 *  und gibt den Index zurueck. Die Suche startet bei from.
 *  Falls der Wert nicht gefunden wird, wird 0 zurueckgegeben.
 *
 *  \param a Zu durchsuchende Matrix
 *  \param from Startzeile und Startspalte der Suche.
 *  \return Gibt den Index zurueck.
 *  \retval 0 Es wurde keine 1.0 mehr gefunden.
 */
unsigned int matrix_ones(Matrix *a, unsigned int from);
/** Alias von matrix_ones()
 */
#define m_ones matrix_ones

/** \brief Durchsuche die Matrix nach value auf der Diagonalen
 *
 *  Durchsucht die Diagonale der Matrix a nach dem Wert in value
 *  und gibt den Index zurueck. Die Suche startet bei from.
 *  Falls der Wert nicht gefunden wird, wird 0 zurueckgegeben.
 *
 *  \param a Zu durchsuchende Matrix
 *  \param from Startzeile und Startspalte der Suche.
 *  \param value Zu suchender Wert
 *  \return Gibt den Index zurueck
 *  \retval 0 Der Wert in value wurde nicht gefunden.
 */
unsigned int matrix_diagvalue(Matrix *a, unsigned int from, float value);
/** Alias von matrix_diagvalue()
 */
#define m_diagval matrix_diagvalue

/** Zaehlt alle Werte ungleich 0.0f in einer Zeile und gibt
 *  die Anzahl zurueck.
 */
unsigned int matrix_nonzero_row(Matrix *a,unsigned int row);
/** Alias von matrix_nonzero_row()
 */
#define m_nz_row matrix_nonzero_row

/** Zaehlt alle Werte ungleich 0.0f in einer Spalte und gibt
 *  die Anzahl zurueck.
 */
unsigned int matrix_nonzero_column(Matrix *a,unsigned int column);
/** Alias von matrix_nonzero_column()
 */
#define m_nz_col matrix_nonzero_column

/** Gibt die Dimension einer quadratischen Matrix a zurueck.
 *
 *  \return Dimension der quadratischen Matrix a
 */
unsigned int matrix_dimension(Matrix *a);
/** Alias von matrix_dimension()
 */
#define m_dim matrix_dimension

/** Gibt n Nachfolgende Matrizen des Typs Matrix* wieder frei.
 */
void matrix_nfree(unsigned int n, ...);
/** Alias von matrix_nfree()
 */
#define m_nfree matrix_nfree

/** Gibt eine Matrix wieder frei.
 */
void matrix_free(Matrix *a);
/** Alias von matrix_free()
 */
#define m_free matrix_free

/** Gibt alle noch im Speicher liegenden Matrizen wieder frei.
 */
void matrix_freeall();
/** Alias von matrix_freeall()
 */
#define m_freeall matrix_freeall

/** Gibt die ueber dot generierten Namen der Matrix zurueck
 *  Der Zaehlanfang des Arrays liegt bei 1 anstatt bei 0.
 */
Nodenames* name_transition();
/** Alias von name_transition()
 */
#define n_transit name_transition
#define NODENAMES(n) Nodenames *n = name_transition();

/** Erzeugt ein neues Namensarray mit i Elementen.
 *  Der Zaehlanfang des Arrays liegt bei 1 anstatt bei 0.
 *  Die Elemente sind schon mit STRMAX grossem Speicher
 *  vorbesetzt.
 */
Nodenames* name_new(unsigned int i);
/** Alias von name_new()
 */
#define n_new name_new
#define NAMES(n,i) Nodenames *n = name_new(i);

/** Loescht den Namenseintrag i.
 */
Nodenames* name_reduce(Nodenames *n, unsigned int i);
/** Alias von name_reduce()
 */
#define n_reduce name_reduce

/** Gibt die Namen und die Matrix aus.
 *  
 *  \param heading Ueberschrift der Ausgabe. Darf NULL sein.
 *  \param n Namen der Zeilen. Wenn NULL, so wird matrix_print() aufgerufen.
 *  \param m Auszugebende Matrix. Wenn NULL, so werden nur die Namen ausgegeben.
 */
void name_print(const char *heading, Nodenames *n, Matrix *m);
/** Alias von name_print()
 */
#define n_print name_print

/** Gibt die Namen wieder frei.
 */
void name_free(Nodenames *n);
/** Alias von name_free()
 */
#define n_free name_free

/** Gibt alle noch im Speicher liegenden Namen wieder frei.
 */
void name_freeall();
/** Alias von name_freeall()
 */
#define n_freeall name_freeall;

/** Erzeugt eine Kopie der Namen n.
 */
Nodenames* name_copy(Nodenames *n);
/** Alias von name_copy()
 */
#define n_copy name_copy

/** Sucht den Knotennamen name und gibt den Index zurueck.
 *  
 *  \param n Zu durchsuchende Knotennamen
 *  \param name Zu suchender String
 *  \retval 0 name wurde in n nicht gefunden
 */
unsigned int name_index(Nodenames *n, char name[]);
/** Alias von name_index()
 */
#define n_index name_index

/** Gibt den Cluster als Matrix zurueck.
 */
Matrix* cluster_transition();
/** Alias von cluster_transition()
 */
#define c_transit cluster_transition

/** Gibt die Namen des Clusters zurueck.
 */
Nodenames* cluster_nametransition();
/** Alias von cluster_nametransition()
 */
#define c_ntransit cluster_nametransition

#endif
