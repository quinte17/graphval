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
#ifndef PMARKOV_H
#define PMARKOV_H

#define _GNU_SOURCE

#include <graphval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Information that need to be remembered:
 *  	- all DID elements
 *  	- ID objects _with_ PROP 'label'
 *
 *  Following usecases for the list:
 *  	- Save DIDs
 *  	  used fields:
 *  	  	o src		source of transition
 *  	  	o dest		destination of transition
 *  	  	o label		transition
 *  	- Save IDs
 *  	  used fields:
 *  	  	o name		name of node
 *  	  	o label		alias (optional)
 *  	  	o index		give used nodes an index
 *  	  	o used		1 if the node is used in transitions
 *  	  	o subgraph	if the node is a member of a subgraph then remember it
 */
typedef struct List {
	struct List *src;
	struct List *dest;
	char *name;
	char *label;
	int index;
	int used;
	t_graphnode *subgraph;
	struct List *next;
} t_list;

/** Global pointer to remember lists
 */
static t_list *id_elements  = NULL;
static t_list *did_elements = NULL;
static t_list *c_names = NULL;

static int id_count  = 0;
static int sub_count = 0;

/** Create List element and return its pointer
 */
static t_list* create_list_element(t_list *src, t_list *dest, char *name, char *label, int index, int used, t_graphnode *subgraph, t_list *next);

/** Recursivly get all relevant DID and ID objects.
 *  Also handle SUBGRAPHs.
 */
static void get_relevant_data(t_graphnode *graph);

/** Search through LIST elements and find PROP element named *value.
 *  This function returns pointer to PROP element or NULL if not found.
 *
 *  If graph is NULL then NULL is returned.
 */
static t_graphnode* get_property(t_graphnode *list, char *value);

/** Remember ID elements, make them unique
 *  	name	: name of node
 *  	label	: alias for the nodename (optional)
 *  	used	: mark if this node is used in transitions, not used items are irrelevant
 *  	subgraph: remember the subgraph, also for future use
 *
 *  	return	: t_list* element
 */
static t_list* create_id_element(char *name, char *label, int used, t_graphnode *subgraph);

/** Remember DID elements
 *  	src	: source node
 *  	dest	: destination node
 *  	label	: value of the transition which is saved in PROP 'label'
 *
 *  	return	: void
 */
static void create_did_element(t_list *src, t_list *dest, char *label); 

/** Create cname elements an copy string
 *  make sure that all cnames are unique
 */
static void create_cname_element(char *label);

/** Scan label for c-names and remember them.
 */
static void scan_label(char *label);

/** Delete all unused ids
 */
static void free_unused_ids();

/** Delete all list contents to free not used memory
 */
static void free_all_lists();

/** Update index of id-list
 */
static void update_index();

/** Emit C-Code based on found information to stdout
 */
static void emit_c();

/** Emit Octave-Code based on found information to stdout
 */
static void emit_octave();
#endif
