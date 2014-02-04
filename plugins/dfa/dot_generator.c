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
#include "dot_generator.h"

//prototypes of private functions:
void write_states(t_list* states);
void write_transitions(t_list* dids);

/**
*Prints a valid *.dot-file generated with given automaton information
*/
void generate_dfa_dot_code(char* automaton_name, t_list* states, t_list* dids, t_matrix* state_matrix)
{
	fprintf(stdout,"/*\n* Generated code by graphval's dfa plugin\n*/\n");
	fprintf(stdout,"digraph %s {\n", automaton_name);
	fprintf(stdout,"\trankdir = LR\n");
	fprintf(stdout,"\tnode [ shape=circle ];\n");
	fprintf(stdout,"\t\"starter\" [");
	fprintf(stdout," label=\"\"");
	fprintf(stdout," shape=\"plaintext\"");
	fprintf(stdout," ]\n");

	//-------------------
	write_states(states);
  
	//-------------------
	write_transitions(dids);	
	
	
	fprintf(stdout,"}\n");

}
void write_states(t_list* states)
{
	char* start_state_props = "[ start=\"yes\" ]\t/* start node */";
	char* end_state_props   = "[ shape=doublecircle end=\"yes\" ]\t/* end node */";
	char* start_end_state_props   = "[ shape=doublecircle start=\"yes\" end=\"yes\" ]\t/* start and end node */";
	t_element* h = states->first;
    
	fprintf(stdout,"\n\t// states\n\n");
	while(h != NULL)
	{
		if(h->is_start != 0)
		{
     		fprintf(stdout,"\t\"%s\" %s\n", h->name, start_state_props); 
		} 
		else if (h->is_end != 0)
		{
			fprintf(stdout,"\t\"%s\" %s\n", h->name, end_state_props); 
		} 
		else if (h->is_start != 0 && h->is_end != 0)
		{
			fprintf(stdout,"\t\"%s\" %s\n", h->name, start_end_state_props); 			
		}
		else
		{
			fprintf(stdout,"\t\"%s\"\n", h->name);
		}
		h= h->next;
	}
}

void write_transitions(t_list* dids)
{
	t_element* h = dids->first;
	t_element* start_state = find_start_state(dids);

	fprintf(stdout,"\n\t//transitions\n\n");
	fprintf(stdout,"\t\"starter\" -> \"%s\"\n", start_state->name);
	
	while(h != NULL)
	{
		fprintf(stdout,"\t\"%s\" -> \"%s\" [ label=\"%s\" ]\n", h->source->name, h->destination->name, h->transition->label);	
		h = h->next;
	}	
}

