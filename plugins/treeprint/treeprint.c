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
#include "treeprint.h"

void printtabs(int x)
{
	int i;
	for(i = 0; i < x; i++)
	{
		fprintf(stdout,"\t");
	}
}

void printtree_rec(t_graphnode *graph, int x)
{
	if(!graph)
		return ;

	printtree_rec(graph->right,x+1);
	printtabs(x);
	switch(graph->type)
	{
		case DIGRAPH:
			fprintf(stdout,"DIGRAPH %s\n",graph->name);
			break;
		case GRAPH:
			fprintf(stdout,"GRAPH %s\n",graph->name);
			break;
		case SUBGRAPH:
			fprintf(stdout,"SUBGRAPH %s\n",graph->name);
			break;
		case ID:
			fprintf(stdout,"ID %s\n",graph->name);
			break;
		case DID:
			fprintf(stdout,"DID %s %s %s\n",graph->name,graph->arrow,graph->value);
			break;
		case PROP:
			fprintf(stdout,"PROP %s = %s\n",graph->name,graph->value);
			break;
		case LIST:
			fprintf(stdout,"LIST\n");
			break;
		default:
			fprintf(stderr,"!!! UNKNOWN TREENODETYPE !!!\n");
			exit(EXIT_FAILURE);
	}
	printtree_rec(graph->left,x+1);
}

int tp_start(t_graphnode *graph, int argc, char *argv[])
{
	fprintf(stderr,"...gv_tp started...\n");
	printtree_rec(graph,0);
	fprintf(stderr,"...gv_tp ended...\n");

	return EXIT_SUCCESS;
}
