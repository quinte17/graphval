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
#ifndef GRAPHVAL_H
#define GRAPHVAL_H

/** \file graphval.h
 *  This header defines types of plugin-API.
 */

#define STRINGMAX 256	/**< maximum length of string */
#define ARROWMAX 3	/**< maximum length of arrow */

/** Values of t_graphnode.type
 */
#define DIGRAPH 0 
#define GRAPH 1 
#define SUBGRAPH 2 
#define ID 3 
#define DID 4 
#define PROP 5 
#define LIST 6 

typedef struct graphnode{
	int type;
	char name[STRINGMAX];
	char value[STRINGMAX];
	char arrow[ARROWMAX];
	struct graphnode *left;
	struct graphnode *right;
} t_graphnode;

/** Every plugin has to implement this funtion based on the pluginname.
 *  Example follows:
 *
 *  Libname		Pluginname	Startername
 *  gv_tp.so		tp		tp_start
 *  gv_dfa.so		dfa		dfa_start
 *  gv_markov.so	markov		markov_start
 */
int plugin_start(t_graphnode *, int argc, char *argv[]);

#endif
