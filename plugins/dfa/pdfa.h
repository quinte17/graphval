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
#ifndef PDFA_H
#define PDFA_H

#define _GNU_SOURCE

#include <graphval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FIFO_list.h"
#include "t_node_matrix.h"
#include "dot_generator.h"
#include "c_generator.h"

t_graphnode* has_start_property(t_graphnode* node);
t_graphnode* has_end_property(t_graphnode* node);
t_graphnode* has_did_label_property(t_graphnode* node);

/*
* Returns a pointer to the inserted element.
* If the element already exists (compared by name), no new element is created and the pointer to 
* the existing element is returned. 
*/
t_element* insert_once_id(t_graphnode* graph, t_list* ids, char* name, char* label);

/*
* Returns a pointer to the inserted element.
* If the element already exists (compared by label), no new element is created and the pointer to 
* the existing element is returned. 
*/
t_element* insert_once_label(t_list* labels, char* label);

/*
* Fills the lists needed to build the transition-state-matrix
*/
void fill_did_list(t_graphnode* graph, t_list* dids, t_list* ids, t_list* labels);

/*
*Builds the transition-state-matrix
*/
t_matrix* fill_state_matrix(t_list* dids, t_list* ids, t_list* labels);

/* 
* Returns the id-node with the given name.
* If no node exists, this function will return NULL.
*/
t_graphnode* get_id_by_name(t_graphnode* graph,char* name);

/*
* Returns 1 if the given node has a start property.
* Otherwise 0.
*/
int is_start_state(t_graphnode* graph, char* name);

/*
* Returns 1 if the given node has an end property.
* Otherwise 0.
*/
int is_end_state(t_graphnode* graph, char* name);

/**
* Interprets a given input with the dfa.
* Transition labels are handled as single characters.
*
* Returns 1 if the input is 1 accepted by the dfa.
* Otherwise 0; 
*/
int interpret_input_char(FILE* input, t_list* dids, t_matrix* state_matrix);

/**
* Interprets a given input with the dfa.
* Transition labels are handled as char*-strings.
*
* Returns 1 if the input is 1 accepted by the dfa.
* Otherwise 0; 
*/
int interpret_input_string(FILE* input, t_list* dids, t_matrix* state_matrix);

/**
* Searches for a suitable transition over the did-list indirectly in the
* labels-list. Input is a valid char symbol.
* In case of success this function returns a pointer to the t_element
* representing the suitable label. Otherwise NULL.
*/
t_element* get_transition_by_label(char* label, t_list* dids);

/**
*Gets the start state from the did-list
*/
t_element* find_start_state(t_list* dids);

/**
* gets the start state from the list of all states
*/
t_element* get_start_state(t_list* states);

/**
* Inserts copies of the end_states to a list which will be returned
*/
t_list* get_end_states(t_list* states);

/**
* removes all new_line symbols of a given string with maximum size of STRINGMAX
*/
char* no_new_line(const char*s);

/**
* Checks if the alphabet of the automaton consists of single symbols
* Returns   1 if true,
* otherwise 0.
*/
int is_single_symbol_dfa(t_list* labels);
#endif
