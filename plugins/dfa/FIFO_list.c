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

#include "FIFO_list.h"

t_list* get_list_instance()
{
	t_list* ins;
	ins = (t_list*) malloc(sizeof(t_list));
	ins->size = 0;
	ins->first = NULL;
	ins->last = NULL;

	return ins; 	
}
 
void delete_instance( t_list* list)
{
	t_element* h = list->first;

	while(h != NULL)
	{
		list->first= h->next;
		free(h);
		h= list->first;
	}

	free(list); 	
}
 
void put(t_list* list, t_element* val)
{    
	if(list->last == NULL)
	{
		list->last = val;
	}
	else{
		//Element hinten anfuegen
		list->last->next = val;
		list->last = val;
	}
	//Wenn liste vorher leer war, erstes Element gleich first und last
	if(list->first == NULL)
		list->first = list->last;   

	list->size++;    
}
 
/* This function returns the last element of the list*/ 
t_element*  get( t_list* list)
{
	t_element* ret;
	t_element* h;

	// Ist Liste gueltig?
	if(list == NULL)
		return NULL;
	if(list->first == NULL)
		return NULL;
    
	//Dann hole Wert des ersten Listenelements und haenge es aus 
	h = list->first;
	list->first = h->next;  
	ret = h; 

	list->size--;
	return ret;   	
}
 
void print_list(t_list* list){
	t_element* h;
	h = list->first;

	if( list == NULL)
		return;
	if(list->first == NULL || list->last == NULL)
		return; 
 		
 	printf("PRINT LIST :\n");
 	while(h != list->last){
 		printf("Name:%s, Label:%s\n", h->name, h->label);
 		h = h->next;
 	}
 	printf("Name:%s, Label:%s\n", list->last->name, list->last->label);;
}
 
/* Generates a ID list element with given parameters */
t_element* generate_id_element(char* name, int is_start, int is_end, char* label, int index)
{
	t_element* neu = NULL;
    
	// Neues Element anlegen
	neu = (t_element*) malloc(sizeof(*neu));	
	neu->name     = name;
	neu->is_start = is_start;
	neu->is_end   = is_end;
	neu->label    = label;
	neu->index    = index;
    
	return neu;
}

/* Generatars a label list element with given parameters*/
t_element* generate_label_element(char* label, int index)
{
	t_element* new = NULL;
	new = (t_element*) malloc(sizeof(*new));
	new->label = label;
	new->index = index;
	
	return new;	
}

/* Generates a DID list element with given parameters */
t_element* generate_did_element(char* label, t_element* source, t_element* destination, t_element* transition, int index)
{
	t_element* neu = NULL;
    
 	// Neues Element anlegen
 	neu = (t_element*) malloc(sizeof(*neu));	
	neu->label		= label;
	neu->source		= source;
	neu->destination	= destination;
	neu->transition		= transition;
	neu->index		= index;

	return neu;
}

