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
#include "markov.h"
#include <stdarg.h>

/** Vertauscht a und b mithilfe eines dreitauschs.
 *  Optimierungsmoeglichkeit mithilfe von Bitoperationen.
 */
#define SWAP(a,b) { int xtmp=(a); (a)=(b); (b)=xtmp; }

/** \internal Struktur fuer die "Garbage Collection"
 */
struct xgarbage {
	struct xgarbage *next;		/**< Pointer auf naechstes Listenelement. */
	Matrix *m;			/**< Adresse der Matrix fuer spaeteren free() */
	Nodenames *n;			/**< Adresse der Nodenames fuer spaeteren free() */
};
/** \internal Globale Variable fuer Matrix Strukturen.
 */
struct xgarbage *garbage_matrix = NULL;
/** \internal Globale Variable fuer Nodenames Strukturen.
 */
struct xgarbage *garbage_nodes = NULL;

void matrix_error(char *s, Matrix *a, Matrix *b)
{
	fprintf(stderr,"--- Fehler:\n");
	fprintf(stderr,"%s\n",s);
	if(a != NULL)
		fprintf(stderr,"Matrix a: %dx%d\n",a->number_rows,a->number_columns);
	if(b != NULL)
		fprintf(stderr,"Matrix b: %dx%d\n",b->number_rows,b->number_columns);
	fprintf(stderr,"---\n");
	exit(1);
}

Matrix* matrix_new(unsigned int i, unsigned int j)
{
	Matrix* tmp;
	struct xgarbage *tgar;
	unsigned int x,y;
	unsigned int k;

	if(i <= 0 || j <= 0)
		matrix_error("matrix_new:\nDimension der Matrix muss groesser 0 sein",NULL,NULL);
	
	tmp = (Matrix *) malloc(sizeof(*tmp));

	if(tmp == NULL)
		matrix_error("matrix_new:\nmalloc gab NULL zurueck.",NULL,NULL);
	
	tgar = (struct xgarbage*) malloc(sizeof(*tgar));
	if(tgar == NULL)
		matrix_error("matrix_new:\nmalloc gab NULL zurueck.",NULL,NULL);
	
	// Einordnen in die Liste der Garbagecollection
	tgar->next = garbage_matrix;
	tgar->m = tmp;
	garbage_matrix = tgar;

	tmp->number_rows = i;
	tmp->number_columns = j;
	
	// Platz fuer Matrix  erstellen und organisieren

	tmp->m = (float**) malloc(i*sizeof(float*));
	if(tmp->m == NULL)
		matrix_error("matrix_new:\nmalloc gab NULL zurueck.",NULL,NULL);
	tmp->m--;	// Zaehlanfang bei 1

	tmp->m[1] = (float*) malloc(i*j*sizeof(float));
	if(tmp->m[1] == NULL)
		matrix_error("matrix_new:\nmalloc gab NULL zurueck.",NULL,NULL);
	tmp->m[1]--;	// Zaehlanfang bei 1

	for(k = 2; k <= i; k++)
		tmp->m[k] = tmp->m[k-1]+j;

	// Matrix Initialisieren
	for(x = 1; x <= tmp->number_rows; x++)
		for(y = 1; y <= tmp->number_columns; y++)
			tmp->m[x][y] = 0.0f;

	return tmp;
}

Matrix* matrix_identity(unsigned int i)
{
	unsigned int j;
	Matrix *tmp;

	if(i <= 0)
		matrix_error("matrix_identity:\nDimension der Matrix muss groesser 0 sein.",NULL,NULL);

	tmp = matrix_new(i,i);
	
	// Diagonale mit 1.0 vorbesetzen
	for(j = 1; j <= tmp->number_rows; j++)
		tmp->m[j][j] = 1.0f;

	return tmp;
}

void matrix_print(const char *heading, Matrix *m)
{
	unsigned int i,j;

	if(m == NULL)
		return ;
	
	if(heading != NULL)
		fprintf(stdout,"%s\n",heading);

	for(i=1; i <= m->number_rows; i++)
	{
		for(j=1; j <= m->number_columns; j++)
			fprintf(stdout,"%7.3f\t",m->m[i][j]);
		fprintf(stdout,"\n");
	}
}

Matrix* matrix_add(Matrix *a, Matrix *b)
{
	unsigned int i,j;
	Matrix *tmp;
	if(a->number_rows != b->number_rows || a->number_columns != b->number_columns)
		matrix_error("matrix_add:\nMatrizen muessen die gleiche Dimension haben",a,b);
	
	tmp = matrix_new(a->number_rows, a->number_columns);
	
	for(i = 1; i <= a->number_rows; i++)
		for(j = 1; j <= a->number_columns; j++)
			tmp->m[i][j] = a->m[i][j] + b->m[i][j];
	
	return tmp;
}

Matrix* matrix_sub(Matrix *a, Matrix *b)
{
	unsigned int i,j;
	Matrix *tmp;
	if(a->number_rows != b->number_rows || a->number_columns != b->number_columns)
		matrix_error("matrix_sub:\nMatrizen muessen die gleiche Dimension haben",a,b);
	
	tmp = matrix_new(a->number_rows,a->number_columns);
	
	for(i = 1; i <= a->number_rows; i++)
		for(j = 1; j <= a->number_columns; j++)
			tmp->m[i][j] = a->m[i][j] - b->m[i][j];
	
	return tmp;
}

Matrix* matrix_mult(Matrix *a, Matrix *b)
{
	unsigned int i,j,k;
	Matrix *tmp;
	if(a->number_columns != b->number_rows)
		matrix_error("matrix_mult:\nMatrizen muessen kompatibel sein",a,b);
	
	tmp = matrix_new(a->number_rows,b->number_columns);
	
	for(i = 1; i <= a->number_rows; i++)
		for(j = 1; j <= b->number_columns; j++)
			for(k = 1; k <= a->number_columns; k++)
				tmp->m[i][j] += a->m[i][k] * b->m[k][j];
	
	return tmp;
}

Matrix* matrix_scalarmult(Matrix *a, float f)
{
	unsigned int i,j;
	Matrix *tmp;
	tmp = matrix_copy(a);

	for(i = 1; i <= tmp->number_rows; i++)
		for(j = 1; j <= tmp->number_columns; j++)
			tmp->m[i][j] = tmp->m[i][j] * f;
	
	return tmp;
}

Matrix* matrix_transpose(Matrix *a)
{
	unsigned int i,j;
	Matrix *tmp;
	tmp = matrix_new(a->number_columns,a->number_rows);

	for(i = 1; i <= a->number_rows; i++)
		for(j = 1; j <= a->number_columns; j++)
			tmp->m[j][i] = a->m[i][j];
	
	return tmp;
}

Matrix* matrix_gaussjordan(Matrix *a, Matrix *b)
{
	Matrix *mtmp, *rtmp;
	float **ta, **tb;
	unsigned int n;
	unsigned int *indexspalte;
	unsigned int *indexreihe;
	unsigned int *pivot;
	unsigned int i,j,k;
	unsigned int ispalte,izeile;
	float max_element;		// groestes element
	float diagkw;			// kehrwert des diagonalelements

	if(a->number_rows != a->number_columns || a->number_rows != b->number_rows || b->number_columns != 1)
		matrix_error("matrix_gaussjordan:\nMatrizen muessen kompatibel sein,\nrechte Seite darf nur eine Spalte haben.",a,b);
	
	mtmp = matrix_copy(a);
	rtmp = matrix_copy(b);

	ta = mtmp->m;
	tb = rtmp->m;
	n = mtmp->number_rows;

	/*  Hier werden gleich alle 3 temporaeren Arrays mit malloc geholt.
	 *  Zaehlanfang ist jeweils bei 1.
	 */
	indexspalte = (unsigned int*)malloc(3 * n * sizeof(unsigned int));
	if(indexspalte == NULL)
		matrix_error("matrix_gaussjordan:\nmalloc gab NULL zurueck.",NULL,NULL);
	indexspalte--;
	indexreihe = indexspalte + n;
	pivot = indexreihe + n;

	for (j = 1; j <= n; j++)
		pivot[j] = 0;

	for (i = 1; i <= n; i++)
	{
		max_element = 0.0f;

		for (j = 1; j<=n; j++)
		{
			if (pivot[j] != 1)
			{
				for (k=1; k <= n; k++)
				{
					if (pivot[k] == 0 && fabs(ta[j][k]) >= max_element)
					{
						// Suche den groesten Betrag
						max_element = fabs(ta[j][k]);
						izeile = j;
						ispalte = k;
					}
				}
			}
		}
		pivot[ispalte] = pivot[ispalte] + 1;
		if (izeile != ispalte)
		{
			for (j = 1; j <= n; j++)
				SWAP(ta[izeile][j],ta[ispalte][j])
			SWAP(tb[izeile][1],tb[ispalte][1])
		}
		indexreihe[i] = izeile;
		indexspalte[i] = ispalte;

		// Falls 0.0 auf dem Diagonalelement steht, ist die Matrix singulaer
		if (ta[ispalte][ispalte] == 0.0f)
			matrix_error("matrix_gaussjordan:\nSingulaere Matrix kann nicht geloest werden.",NULL,NULL);

		// Kehrwert des Diagonalelements Bilden und
		// gesamte Zeile Multiplizieren
		diagkw = 1.0 / ta[ispalte][ispalte];
		ta[ispalte][ispalte] = 1.0f;

		for (j = 1; j <= n; j++)
			ta[ispalte][j] *= diagkw;
		tb[ispalte][1] *= diagkw;

		for (k = 1; k <= n; k++)
		{
			if (k != ispalte)
			{
				float tmp;

				tmp = ta[k][ispalte];
				ta[k][ispalte] = 0.0f;

				for (j = 1; j <= n; j++)
					ta[k][j] -= ta[ispalte][j] * tmp;
				tb[k][1] -= tb[ispalte][1] * tmp;
			}
		}
	}
	for (j = n; j >= 1; j--)
	{
		if (indexreihe[j] != indexspalte[j])
		{
			for (k = 1; k <= n; k++)
				SWAP(ta[k][indexreihe[j]],ta[k][indexspalte[j]]);
		}
	}
	
	// Hier werden alle 3 temporaeren Arrays wieder freigegeben.
	free(indexspalte + 1);

	matrix_free(mtmp);

	return rtmp;
}

Matrix* matrix_copy(Matrix *a)
{
	Matrix *tmp;
	tmp = matrix_new(a->number_rows,a->number_columns);

	// da das erste element bei m[1][1] liegt muss hier
	// die Pointerarithmetik mit m[1]+1 verwendet werden.
	memcpy(tmp->m[1]+1,a->m[1]+1,a->number_rows * a->number_columns * sizeof(float));

	return tmp;
}

Matrix* matrix_rowreplace(Matrix *a, unsigned int i, Matrix *b)
{
	Matrix *tmp;
	unsigned int j;
	if(a->number_rows < i || b->number_columns != a->number_columns)
		matrix_error("matrix_rowreplace:\nMatrizen passen nicht zueinander oder Zeile in a nicht existent",a,b);

	tmp = matrix_copy(a);

	for(j = 1; j <= tmp->number_columns; j++)
		tmp->m[i][j] = b->m[1][j];
	
	return tmp;
}

Matrix* matrix_rowmodify(Matrix *a, unsigned int i, float f)
{
	Matrix *tmp;
	unsigned int j;
	if(a->number_rows < i)
		matrix_error("matrix_rowmodify:\nZeile ist nicht innerhalb der Matrix",a,NULL);
	
	tmp = matrix_copy(a);
	
	for(j = 1; j <= tmp->number_columns; j++)
		tmp->m[i][j] = f;
	
	return tmp;
}

Matrix* matrix_reduce(Matrix *a, unsigned int k)
{
	Matrix *tmp;
	unsigned int i,j,difi,difj;
	if( k > a->number_rows || k > a->number_columns || k <= 0 )
	{
		matrix_error("matrix_reduce:\nk nicht innerhalb der Matrix",a,NULL);
	}
	else
	{
		// zeile und spalte entfernen
		tmp = matrix_new(a->number_rows-1,a->number_columns-1);
		difi = 0;
		difj = 0;
		for(i = 1; i <= a->number_rows; i++)
		{
			if(i == k)
				difi = 1;
			else
			{

				difj = 0;
				for(j = 1; j <= a->number_columns; j++)
				{
					if(j == k)
						difj = 1;
					else
						tmp->m[i-difi][j-difj] = a->m[i][j];
				}
			}
		}

		return tmp;
	}
}

unsigned int matrix_ones(Matrix *a, unsigned int from)
{
	return matrix_diagvalue(a,from,1.0f);
}

unsigned int matrix_diagvalue(Matrix *a, unsigned int from, float value)
{
	unsigned int i,min;

	if(a->number_rows < a->number_columns)
		min = a->number_rows;
	else
		min = a->number_columns;
	
	if(from <= 0 || min < from)
		matrix_error("matrix_diagvalue:\nfrom nicht im gueltigen Bereich.",a,NULL);

	for(i = from; i <= min; i++)
		if(a->m[i][i] == value)
		{
			return i;
		}

	return 0;
}

unsigned int matrix_nonzero_row(Matrix *a,unsigned int row)
{
	unsigned int i,count=0;
	if(a == NULL)
		return 0;
	
	if(a->number_rows < row)
		matrix_error("matrix_nonzero_row:\nrow nicht im gueltigen Bereich",a,NULL);
	
	for(i = 1; i <= a->number_columns; i++)
		if(a->m[row][i] != 0.0f)
			count++;
	
	return count;
}

unsigned int matrix_nonzero_column(Matrix *a,unsigned int column)
{
	unsigned int i,count=0;
	if(a == NULL)
		return 0;
	
	if(a->number_columns < column)
		matrix_error("matrix_nonzero_column:\ncolumn nicht im gueltigen Bereich",a,NULL);
	
	for(i = 1; i <= a->number_rows; i++)
		if(a->m[i][column] != 0.0f)
			count++;
	
	return count;
}

unsigned int matrix_dimension(Matrix *a)
{
	if(a->number_rows != a->number_columns)
		matrix_error("matrix_dimension:\nMatrix muss quadratisch sein.",NULL,NULL);
	
	return a->number_rows;
}

void matrix_nfree(unsigned int n, ...)
{
	va_list params;
	Matrix *tmp;

	va_start(params,n);

	while(n-- > 0)
	{
		tmp = va_arg(params,Matrix *);
		matrix_free(tmp);
	}
	va_end(params);
}

void matrix_free(Matrix *a)
{
	struct xgarbage *tmp,*freer;
	tmp = garbage_matrix;

	if(a == NULL)
		return ;
	// Keine matrix mehr in Liste
	if(tmp == NULL)
		return;
	
	if(tmp->m != a)
	{
		while(tmp->next != NULL && tmp->next->m != a)
			tmp = tmp->next;

		if(tmp->next == NULL)
			return ;

		// hier element ausgliedern
		freer = tmp->next;
		tmp->next = tmp->next->next;
		free(freer);
	}
	else
	{
		// element ist das erste element
		freer = tmp;
		garbage_matrix = tmp->next;
		free(freer);
	}
	free(a->m[1]+1);	// Da Zaehlanfang bei 1
	free(a->m+1);		// Da Zaehlanfang bei 1
	free(a);
}

void matrix_freeall()
{
	struct xgarbage *tmp,*freer;
	Matrix *a;
	tmp = garbage_matrix;

	while(tmp != NULL)
	{
		a = tmp->m;
		free(a->m[1]+1);	// Da Zaehlanfang bei 1
		free(a->m+1);		// Da Zaehlanfang bei 1
		free(a);
		tmp = tmp->next;
	}
	tmp = garbage_matrix;
	while(tmp != NULL)
	{
		freer = tmp;
		tmp = tmp->next;
		free(freer);
	}
	garbage_matrix = NULL;
}

Nodenames* name_new(unsigned int i)
{
	Nodenames *tmp = NULL;
	struct xgarbage *tgar;
	unsigned int j;

	if(i <= 0)
		matrix_error("name_new:\ni nicht im gueltigen Bereich.",NULL,NULL);
	
	tmp = (Nodenames*) malloc(sizeof(*tmp));
	if(tmp == NULL)
		matrix_error("name_new:\nNicht genug Speicher.",NULL,NULL);

	tgar = (struct xgarbage*) malloc(sizeof(*tgar));
	if(tgar == NULL)
		matrix_error("name_new:\nmalloc gab NULL zurueck.",NULL,NULL);
	
	// Einordnen in die Liste
	tgar->next = garbage_nodes;
	tgar->n = tmp;
	garbage_nodes = tgar;

	tmp->n = (char**) malloc(sizeof(char*) * i);
	if(tmp->n == NULL)
		matrix_error("name_new:\nNicht genug Speicher.",NULL,NULL);

	tmp->n--;	// Zaehlanfang bei 1
	tmp->number_names = i;

	for(j = 1; j <= i; j++)
	{
		tmp->n[j] = (char *) malloc(sizeof(char) * STRMAX);
		if(tmp->n[j] == NULL)
			matrix_error("name_new:\nNicht genug Speicher.",NULL,NULL);
	}

	return tmp;
}

Nodenames* name_reduce(Nodenames *n, unsigned int i)
{
	Nodenames *tmp;
	unsigned int j;
	if(i > n->number_names || i <= 0)
		matrix_error("name_reduce:\ni nicht im gueltigen Bereich.",NULL,NULL);
	
	tmp = name_new(n->number_names - 1);
	for(j = 1; j <= n->number_names; j++)
	{
		if(j < i)
		{
			strncpy(tmp->n[j],n->n[j],STRMAX);
		}
		else if(j > i)
		{
			strncpy(tmp->n[j-1],n->n[j],STRMAX);
		}
	}

	return tmp;
}

void name_print(const char *heading, Nodenames *n, Matrix *m)
{
	unsigned int i,j;
	unsigned int max, tmp;

	if(n == NULL || n->number_names <= 0)
	{
		matrix_print(heading, m);
		return ;
	}

	max = strlen(n->n[1]);
	for(i = 1; i <= n->number_names; i++)
	{
		tmp = strlen(n->n[i]);
		if(max < tmp)
			max = tmp;
	}
	
	if(m == NULL)
	{
		if(heading != NULL)
			fprintf(stdout,"%s\n",heading);

		for(i = 1; i <= n->number_names; i++)
		{
			fprintf(stdout,"%-*s\n",max,n->n[i]);
		}

		return ;
	}

	if(m->number_rows == n->number_names)
	{
		if(heading != NULL)
			fprintf(stdout,"%s\n",heading);

		for(i = 1; i <= m->number_rows; i++)
		{
			fprintf(stdout,"%-*s ",max,n->n[i]);
			for(j=1; j <= m->number_columns; j++)
				fprintf(stdout,"%7.3f\t",m->m[i][j]);
			fprintf(stdout,"\n");
		}
	}
	else
		matrix_error("name_print:\nMatrix und Namen sind nicht kompatibel.",m,NULL);
}

void name_free(Nodenames *n)
{
	struct xgarbage *tmp,*freer;
	unsigned int i;
	tmp = garbage_nodes;

	if(n == NULL)
		return ;
	// Keine namen mehr in Liste
	if(tmp == NULL)
		return;
	
	if(tmp->n != n)
	{
		while(tmp->next != NULL && tmp->next->n != n)
			tmp = tmp->next;

		if(tmp->next == NULL)
			return ;

		// hier element ausgliedern
		freer = tmp->next;
		tmp->next = tmp->next->next;
		free(freer);
	}
	else
	{
		// element ist das erste element
		freer = tmp;
		garbage_nodes = tmp->next;
		free(freer);
	}
	for(i = 1; i <= n->number_names; i++)
		free(n->n[i]);
	
	free(n->n+1);
	free(n);
}

void name_freeall()
{
	struct xgarbage *tmp,*freer;
	Nodenames *n;
	unsigned int i;
	tmp = garbage_nodes;

	while(tmp != NULL)
	{
		n = tmp->n;

		for(i = 1; i <= n->number_names; i++)
			free(n->n[i]);
		
		free(n->n+1);
		free(n);

		tmp = tmp->next;
	}
	tmp = garbage_nodes;
	while(tmp != NULL)
	{
		freer = tmp;
		tmp = tmp->next;
		free(freer);
	}
	garbage_nodes = NULL;
}

Nodenames* name_copy(Nodenames *n)
{
	Nodenames *tmp;
	unsigned int i;
	if(n == NULL)
		matrix_error("name_copy:\nEs wurde kein Name angegeben.",NULL,NULL);
	
	tmp = name_new(n->number_names);

	for(i = 1; i <= n->number_names; i++)
	{
		strncpy(tmp->n[i],n->n[i],STRMAX);
	}

	return tmp;
}

unsigned int name_index(Nodenames *n, char name[])
{
	unsigned int i;

	if(n == NULL || name == NULL)
		return 0;
	
	for(i = 1; i <= n->number_names; i++)
		if(strncmp(n->n[i],name,STRMAX) == 0)
			return i;

	return 0;
}
