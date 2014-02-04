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

#ifndef FIFO_LISTE_H
#define FIFO_LISTE_H
#include <graphval.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct element{
	//used if list is used for saving IDs
	char* name;
	int is_start;
	int is_end;

	//used if list is used for saving DIDs
	struct element* source;
	struct element* destination;
	struct element* transition;
	//used in both cases
	char* label;	
	int index;	
	struct element* next;
}t_element;

typedef struct list{
	t_element* first;
	t_element* last;
	int size ;
}t_list;

/** 
* Returns a pointer to a initialized list structure
* Gibt einen Zeiger auf eine vorinitialisierte Listenstruktur zurück
*/
t_list* get_list_instance();

/** Frees the memory of the list instance */
void delete_instance( t_list* list);

/** Generates a ID list element with given Parameters */
t_element* generate_id_element(char* name, int is_start, int is_end, char* label, int index);

/** Generates a DID list element with given Parameters */
t_element* generate_did_element(char* label, t_element* source, t_element* destination, t_element* transition, int index);

/** Generatars a label list element with given parameters*/
t_element* generate_label_element(char* label, int index);
 
/**
 * Inserts a list element at the end of the given list
 */
void put(t_list* list, t_element*  val);

/**
 * Returns the value of the first list element and removes it from the list 
 * WARNING: The element has to be freed externally!
*/
t_element* get(t_list* list);

/**
* Returns an element with the given name if it exists.
* Else returns NULL
*/
t_element* get_by_name(char* name, t_list* list);

void print_list(t_list* list);

#endif
