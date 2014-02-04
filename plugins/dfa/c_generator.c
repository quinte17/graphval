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

#include "c_generator.h"

//private prototypes
void write_declaration(t_list* states, t_list* transitions);
void write_table(t_matrix* matrix);

/**
* Generates the C code of the given automaton.
* The generated code needs the DFA-Framework to run
*/
void generate_c_code(t_list* states, t_list* transitions, t_matrix* matrix)
{
	write_declaration(states, transitions);
	write_table(matrix);
}

void write_declaration(t_list* states, t_list* transitions)
{
	t_element* h;
	t_list* end_states;

	end_states = get_end_states(states);
	h= end_states->first;

	fprintf(stdout,"/* Generated code by graphval dfa plugin */\n");
	fprintf(stdout,"\n#include \"dfa.h\"\n\n");

	fprintf(stdout,"int Anzahl_Zustaende = %i;\n", states->size); 
	fprintf(stdout,"int Anzahl_Endzustaende = %i;\n", end_states->size);
	fprintf(stdout,"int Startzustand = %i;\n", get_start_state(states)->index);	

	fprintf(stdout,"int Endzustaende[] = {");
	
	while(h != NULL)
	{
		if(h == end_states->last)
			fprintf(stdout,"%i", h->index);
		else
			fprintf(stdout,"%i, ", h->index);
		
		h = h->next;
	}
	
	fprintf(stdout,"};\n");


	fprintf(stdout,"char *Alphabet[STRMAX] = {\n");
	
	h = transitions->first;
	while(h != NULL)
	{
		if(h == transitions->last)
			fprintf(stdout,"\t\"%s\"\n", h->label);
		else
			fprintf(stdout,"\t\"%s\",\n", h->label);
		
		h = h->next;
	}
	fprintf(stdout,"};\n");
  
	delete_instance(end_states);
}

void write_table(t_matrix* matrix)
{
	int i,j;
	t_element* h;
	
	
	fprintf(stdout, "\nstruct Tabelle_Eintrag Tabelle[%i][%i]={\n", matrix->rows, matrix->columns);
	for(i = 0; i< matrix->rows; i++)
	{
		fprintf(stdout,"{ ");
		
		for(j = 0; j < matrix->columns; j++)
		{
			h = matrix->values[i][j].dest_state;
			fprintf(stdout," {");
			
			if(h == NULL)
			{
				fprintf(stdout,"0,Fehler");
			}
			else
			{
				fprintf(stdout,"%i,Aktion", h->index);
			}


			if(j < matrix->columns -1)
				fprintf(stdout,"},");
			else 
				fprintf(stdout,"}");
		}
		if(i < matrix->rows -1)
			fprintf(stdout," },\n");
		else 
			fprintf(stdout," }\n");
	}
	fprintf(stdout,"};\n");
}
