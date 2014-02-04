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
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "graphval.h"

/** This struct is needed to localize the lexeme.
 *  With that syntax errors have better outputs.
 */
typedef struct myltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  unsigned int linenr;
  char *lexeme;
} myltype;
#define YYLTYPE myltype

/** Generates node for tree
 */
t_graphnode* generate_node(int type, char *name, char *value, char *arrow, t_graphnode *left, t_graphnode *right);

typedef int (*f_plugin) (t_graphnode *, int argc, char *argv[]);

#endif
