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
%{
#include "interface.h"

t_graphnode *treeroot;

%}
%locations
%pure_parser

%union {
	char id[STRINGMAX];
	t_graphnode *x;
}

%token<id> yID yPFEIL yGRAPH

%token yDIGRAPH ySUBGRAPH yGKA yGKZ yEKA yEKZ yASGNOP

%type<x> dfa cmpoundstmt stmtliste stmt attrliste attcmpound attr

%%

dfa
	: yDIGRAPH yID cmpoundstmt { treeroot=$$=generate_node(DIGRAPH, $2, NULL, NULL, $3, NULL); }
	| yGRAPH yID cmpoundstmt { treeroot=$$=generate_node(GRAPH, $2, NULL, NULL, $3, NULL); }
	| error { fprintf(stderr,"Syntaxerror in line %d at '%s'.\n",yylloc.linenr,yylloc.lexeme); exit(EXIT_FAILURE); }
	;

cmpoundstmt
	: yGKA stmtliste yGKZ { $$=$2; }
	;

stmtliste
	: stmtliste stmt { $$=generate_node(LIST, NULL, NULL, NULL, $1, $2); }
	| { $$=NULL; }
	;

stmt
	: yID yPFEIL yID attcmpound { $$=generate_node(DID, $1, $3, $2, $4, NULL); }		// DID
	| yID attcmpound { $$=generate_node(ID, $1, NULL, NULL, $2, NULL); }			// ID
	| yID yASGNOP yID { $$=generate_node(PROP, $1, $3, NULL, NULL, NULL); }			// PROP
	| yGRAPH attcmpound { $$=generate_node(ID, $1, NULL, NULL, $2, NULL); }			// GRAPH treated as ID
	| ySUBGRAPH yID cmpoundstmt { $$=generate_node(SUBGRAPH, $2, NULL, NULL, $3, NULL); }	// SUBGRAPH
	;

attcmpound
	: yEKA attrliste yEKZ { $$=$2; }		// LIST of attributes
	| { $$=NULL; }					// without attributes
	;

attrliste
	: attrliste attr { $$=generate_node(LIST, NULL, NULL, NULL, $1, $2); }
	| { $$=NULL; }
	;

attr
	: yID yASGNOP yID { $$=generate_node(PROP, $1, $3, NULL, NULL, NULL); }
	;

%%

t_graphnode* generate_node(int type, char *name, char *value, char *arrow, t_graphnode *left, t_graphnode *right)
{
	t_graphnode *tmp;
	tmp = (t_graphnode *) malloc(sizeof(t_graphnode));
	if(tmp == NULL)
	{
		fprintf(stderr, "Not enough memory for malloc!\n");
		exit(EXIT_FAILURE);
	}

	tmp->type  = type;
	tmp->left  = left;
	tmp->right = right;

	if(name != NULL)
		strncpy(tmp->name, name, STRINGMAX);

	if(value != NULL)
		strncpy(tmp->value, value, STRINGMAX);

	if(arrow != NULL)
		strncpy(tmp->arrow, arrow, ARROWMAX);

	return tmp;
}
