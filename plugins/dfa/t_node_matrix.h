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
#ifndef t_node_matrix_h
#define t_node_matrix_h

# include "FIFO_list.h"
# include <stdio.h>
# include <stdlib.h>

typedef struct ma_element{
	t_element* dest_state;
	t_element* source_state;
	char* name;
} t_ma_element;
typedef struct matrix{
	t_ma_element** values;
	int rows;
	int columns;
}t_matrix;

t_matrix* create_matrix(int rows, int columns);
void free_matrix(t_matrix* m);
void print_matrix(t_matrix* m);




#endif
