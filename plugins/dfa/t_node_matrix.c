/*
	Graph Evaluation

	Copyright (C) 2009 Ludwig Frank
	Copyright (C) 2009 Stephan Sperber
	Copyright (C) 2009 Jakob Schäffler

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
#include "t_node_matrix.h"

t_matrix* create_matrix( int rows, int columns) 
{	
 	int i,j;
	if(rows <= 0 || columns <= 0)
		return NULL;
	
	//allocate and initialize memory for the new matrix    
	t_matrix* new;
	new = (t_matrix*) malloc(sizeof(t_matrix));
	new->rows=rows;
	new->columns = columns;

	//allocate memory for the rowpointer
	new->values = (t_ma_element**) malloc(rows * sizeof(t_ma_element*));
	for(i = 0; i < rows ; i++)
	{
		//allocate memory for every row with a specified length
		new->values[i] = (t_ma_element*) malloc(columns * sizeof(t_ma_element));
	}
    
	// initialize with NULL
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < columns; j++)
		{
			new->values[i][j].source_state = NULL;
			new->values[i][j].dest_state = NULL;
			new->values[i][j].name = NULL;
		}
	
	}
	
	return new;
}


void free_matrix(t_matrix* m)
{
	int i;	
	if(m == NULL) return;


	for (i=0; i< m->rows; i++)
	{
		free(m->values[i]);
	}
	free(m->values);
	free(m);
}

void print_matrix(t_matrix* m)
{
	int i,j;
	t_element* row;
	char* labels[m->columns];
	printf("\n");
	for(i=0; i < m->rows; i++)
	{   
		for(j=0; j< m->columns; j++)
		{	
			if(m->values[i][j].dest_state != NULL){
				printf("%s ", m->values[i][j].dest_state->name);
				row = m->values[i][j].source_state;
				labels[j] = m->values[i][j].name;
			}
			else 
				printf("--");
			
	
		}
		printf(" | %i, %s\n", row->index, row->name);
	}
	for(j=0; j< m->columns; j++)
	{	
		printf("_");
	}
	printf("\n");	
 	for(j=0; j< m->columns; j++)
	{	
		printf(" %s ", labels[j]);		
	}
	printf("\n");	
}
