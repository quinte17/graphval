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
#include "pdfa.h"

int dfa_start(t_graphnode *graph, int argc, char *argv[])
{	
	int erg = -1;
	char* parameter_one;	/* first invoke parameter */
			
	t_list* ids;			/* list of all automaton states */
	t_list* dids;			/* list of all single transitions including information about source and destination*/
	t_list* labels;			/* list of all valid automaton transitions */
	t_matrix* state_matrix;	/* describes a transition-state-table */



	/**************************************************************************/
	fprintf(stderr,"...libdfa started...\n");
	/**************************************************************************/
	    

	ids = get_list_instance();
	labels = get_list_instance();
	dids= get_list_instance();
    
	//fill did, ids and labels
	fill_did_list(graph, dids, ids, labels);
		
	//build transition state matrix
	state_matrix= fill_state_matrix(dids, ids, labels);
	
	
	if(argc > 0)
	{	parameter_one = argv[0];

		// parameter "-i" -> interpretation of input from stdin
		if(strncmp(parameter_one, "-i", 3) == 0)
		{
			if(is_single_symbol_dfa(labels))
			{
				erg =interpret_input_char(stdin, dids, state_matrix);
			}
			else
			{
				erg =interpret_input_string(stdin, dids, state_matrix);
			}			

			if(erg == 1)
			{
				printf("\n---------------------\nEingabe akzeptiert\n---------------------\n\n");
				erg = EXIT_SUCCESS;
			}
			else if( erg == 0)
			{
				printf("!!-------------------\nEingabe nicht akzeptiert\n!!-------------------\n\n");
				erg = EXIT_FAILURE;
			}
			else
				printf("Fehler beim interpretieren\n\n");	
		}
		// parameter "-dot" -> dot-file output to stdout
		else if(strncmp(parameter_one, "-dot", 5) == 0)
		{
			generate_dfa_dot_code(graph->name, ids, dids, state_matrix);
		}
		// parameter "-c" -> c-file output to stdout
		else if(strncmp(parameter_one, "-c", 3) == 0)
		{
			generate_c_code(ids, labels, state_matrix);
		}
	}	


  
	delete_instance(ids);
	delete_instance(labels);
	delete_instance(dids);
	free_matrix(state_matrix);
	/**************************************************************************/
	fprintf(stderr,"...libdfa ended...\n");
	/**************************************************************************/
	return erg;
}

/**
* Gibt die Adresse des Property-Knotens zurück, falls die Eigenschaft auf
* einen Endzustand hinweist, andernfalls 0
*/
t_graphnode* has_end_property( t_graphnode *node)
{    
	t_graphnode *rechter_ast;
	t_graphnode *h;

	if( node == NULL) return NULL;

	rechter_ast = node;  
      
	//Ist aktueller Knoten auch Knoten im Automaten?
	if( rechter_ast !=NULL && (rechter_ast->type == ID) ){
    
		//Haengt an diesem Knoten eine Liste mit Eigenschaften?
		if(rechter_ast->left != NULL && (rechter_ast->left->type == LIST)){ 
         
			h = rechter_ast->left;
         
			while ( h != NULL){
               
				// Ist Knoten vom Typ Property und hat den Wert für 
				// einen Endzustand?
				if ( h->right->type == PROP 
					&& strncmp(h->right->name, "end", STRINGMAX) == 0
					&& strncmp(h->right->value, "yes", STRINGMAX)== 0 
				){
					return h->right;
				}
				h= h->left;
			}

		}
	}

	return NULL;    
}

/**
* Gibt die Adresse des Property-Knotens zurück, falls die Eigenschaft Name auf
* einen Startzustand hinweist, andernfalls 0
*/
t_graphnode* has_start_property( t_graphnode *node){
	t_graphnode *rechter_ast;
	t_graphnode *h;
    
	if( node == NULL) 
		return NULL;

	rechter_ast = node;    

	//Ist aktueller Knoten auch Knoten im Automaten?
	if( rechter_ast !=NULL && (rechter_ast->type == ID) )
	{    
		//Haengt an diesem Knoten eine Liste mit Eigenschaften?
		if(rechter_ast->left != NULL && (rechter_ast->left->type == LIST))
		{ 
			h = rechter_ast->left;
			while ( h != NULL){
				// Ist Knoten vom Typ Property und hat den Wert für 
				// einen Startzustand?
				if( h->right->type == PROP 
				&& strncmp(h->right->name, "start", STRINGMAX) == 0
				&& strncmp(h->right->value, "yes", STRINGMAX)== 0 
				)
				{
					return h->right;
				}
				h= h->left;
			}           
		}
	}    
	return NULL;    
}

/**
*  Gibt die Adresse des Property-Knotens zurück, falls die Eigenschaft label auf 
*  einen gueltigen Zustandsübergang hinweist, andernfalls NULL
*/
t_graphnode* has_did_label_property(t_graphnode* node)
{    
	t_graphnode *rechter_ast, *h;
        
	if( node == NULL ) 
		return NULL;
    
	rechter_ast = node;
	
	if(rechter_ast != NULL && (rechter_ast->type == DID))
	{    
		if(rechter_ast->left != NULL && (rechter_ast->left->type == LIST))
		{
			h = rechter_ast->left;
			while ( h != NULL)
			{
				if(h->right->type == PROP
				&& strncmp (h->right->name, "label", STRINGMAX) == 0
				&& strnlen(h->right->value,STRINGMAX) > 0
				)
				{
					return h->right;
				}
				h = h->left;
			}
		}
	}
	return NULL;            
}

/**
* Returns the pointer to the inserted element.
* If the element already exists (compared by name), no new element is created and the pointer to 
* the existing element is returned. 
*/
t_element* insert_once_id(t_graphnode* graph, t_list* ids, char* name, char* label)
{
	static int index_count= 0;	
	t_element* h = ids->first;
	
	//search for an existing element
	while(h != NULL)
	{
		if(strncmp(h->name, name, STRINGMAX) == 0)
			return h;
		h = h->next;
	}
	h = generate_id_element(name, is_start_state(graph, name), is_end_state(graph, name) ,label, index_count);
	
	put(ids, h);

	index_count++;
	return h;	
}

/**
* Returns a pointer to the inserted element.
* If the element already exists (compared by label), no new element is created and the pointer to 
* the existing element is returned. 
*/
t_element* insert_once_label(t_list* labels, char* label)
{
	
	static int index_count= 0;	
	t_element* h = labels->first;
	
	//search for an existing element
	while(h != NULL)
	{
		if(strncmp(h->label, label, STRINGMAX) == 0)
			return h;

		h = h->next;
	}
	h = generate_label_element(label, index_count);
    
	put (labels, h);
	index_count++;
	return h;
}

/**
* Fills the lists needed to build the transition-state-matrix
*/
void fill_did_list(t_graphnode* graph, t_list* dids, t_list* ids, t_list* labels)
{
	t_graphnode* h = graph->left;
	t_graphnode* label_prop ;
	t_element* elem;

	t_element* source;
	t_element* dest;
	t_element* label;

	int index=0;
	while( h != NULL)
	{
		if (h->right->type == DID)
		{	
			label_prop=has_did_label_property(h->right);
			
			if( label_prop != NULL)
			{
				source = insert_once_id(graph,ids, h->right->name, NULL);
				dest   = insert_once_id(graph,ids, h->right->value, NULL);
				label  = insert_once_label(labels, label_prop->value);

				elem= generate_did_element(	label_prop->value, source, dest, label, index);
				put(dids, elem);
				index++;
			}	
				
		}
		h = h->left;
	}

}

/**
*Builds the transition-state-matrix
*/
t_matrix* fill_state_matrix(t_list* dids, t_list* ids, t_list* labels)
{
	t_element* elem;
	t_matrix* state_matrix;
	//initialize state matrix
	state_matrix = create_matrix(ids->size, labels->size);
	elem = dids->first;
	while(elem != NULL)
 	{
		state_matrix->values[elem->source->index][elem->transition->index].dest_state = elem->destination;
		state_matrix->values[elem->source->index][elem->transition->index].source_state = elem->source;
		state_matrix->values[elem->source->index][elem->transition->index].name = elem->transition->label;
		elem = elem->next;
	}
	return state_matrix;
}

/**
* Returns the id-node with the given name.
* If no node exists, this function will return NULL.
*/
t_graphnode* get_id_by_name(t_graphnode* graph,char* name)
{
	t_graphnode* h = graph->left;
	while(h != NULL)
	{
		if(h->right->type == ID)
		{	
			
			if(strncmp(h->right->name, name, STRINGMAX) == 0)
			{
				return h->right;
			}
		}
		h = h->left;
	}
	return NULL;
}


/**
* Returns 1 if the given node has an end property.
* Otherwise 0.
*/
int is_end_state(t_graphnode* graph, char* name)
{
	if(has_end_property(get_id_by_name(graph, name)) != NULL)
		return 1;
	return 0;
}
/**
* Returns 1 if the given node has a start property.
* Otherwise 0.
*/
int is_start_state(t_graphnode* graph, char* name)
{
	if(has_start_property(get_id_by_name(graph, name)) != NULL)
		return 1;
	return 0;
}

//------------------------------------------------------------------------------
// INTERPRETATION:

/**
* Interprets a given input with the dfa.
* Transition labels are handled a single characters.
* Returns 1 if the input is 1 accepted by the dfa.
* Otherwise 0; 
*/
int interpret_input_char(FILE* input, t_list* dids, t_matrix* state_matrix)
{
	t_element* state;
	t_element* transition;
	int input_symbol;
	char inputch[2];
	inputch[1]='\0';
	
	//size_t is_eof;
	state = find_start_state(dids);

	//go trough the transition-state-matrix
	
	input_symbol = fgetc(input);
	//is_eof = fread(&input_symbol, 1,1,stdin);
	
	while(input_symbol != EOF && input_symbol != '\n')
	{
		//printf("\nfgetc: %i", input_symbol);
		inputch[0] = (char) input_symbol;
				
		transition = get_transition_by_label(inputch, dids);
		if(state != NULL && transition != NULL)
		{
			state = state_matrix->values[state->index][transition->index].dest_state;
			if(state == NULL)
				return 0;	
		}
		else
		{		
			return 0;
		}		
		//is_eof = fread(&input_symbol, 1,1,stdin);	
		input_symbol = fgetc(input);
		
	}

	//Have an end-state been achieved?
	if(state->is_end != 0)
		return 1;
	else
		return 0;
}

/**
* Interprets a given input with the dfa.
* Transition labels are char*-strings no single characters.
* Returns 1 if the input is 1 accepted by the dfa.
* Otherwise 0; 
*/
int interpret_input_string(FILE* input, t_list* dids, t_matrix* state_matrix)
{
	t_element* state;
	t_element* transition;
	char* return_code;
	char input_string[STRINGMAX];

	state = find_start_state(dids);

	print_list(dids);



	//go trough the transition-state-matrix
	
	return_code = fgets(input_string, STRINGMAX, input);

	
	while(return_code != NULL)
	{

		
		if(strncmp(input_string, "\n", STRINGMAX) == 0)
			break;			
	
		transition = get_transition_by_label(input_string, dids);
		if(state != NULL && transition != NULL)
		{
			state = state_matrix->values[state->index][transition->index].dest_state;
			if(state == NULL)
			{
				return 0;			
			}
		}
		else
		{
			return 0;
		}		
		return_code = fgets(input_string, STRINGMAX, input);
		
	}
	
	
	//Has an end-state been achieved?
	if(state->is_end != 0)
		return 1;
	else
		return 0;

}

t_element* find_start_state(t_list* dids)
{
	t_element* h = dids->first;
	
	while(h != NULL)
	{
		if(h->source->is_start != 0)
			return h->source;

		h = h->next;	
	}
	return NULL;
}

/**
* Searches for a suitable transition over the did-list indirectly in the
* labels-list. Input is a valid char symbol.
* In case of success this function returns a pointer to the t_element
* representing the suitable label. Otherwise NULL.
*/
t_element* get_transition_by_label(char* label, t_list* dids)
{	
	char* p_label;
	t_element* h = dids->first;
	p_label = no_new_line(label);

	while(h != NULL)
	{

		if( strncmp( h->transition->label, p_label , STRINGMAX ) == 0)
			return h->transition;

		h = h->next;	
	}
	
	// free string memory allocated by no_new_line
	free(p_label);
	return NULL;
}

/**
* gets the start state from the list of all states
*/
t_element* get_start_state(t_list* states)
{
	t_element* h;
	h= states->first;
	
	while( h != NULL)
	{
		if(h->is_start != 0){
			return h;
		}
		h = h->next;
	}

	return NULL;
}

/**
* Inserts copies of the end_states to a list which will be returned
*/
t_list* get_end_states(t_list* states)
{
	t_list* end_states;
	t_element* h;
	t_element* copy;

	end_states = get_list_instance();
	h= states->first;
	
	while( h != NULL)
	{
		if(h->is_end != 0){
			copy = malloc(sizeof(t_element));
			
			// copy values			
			copy->name = h->name;
			copy->is_start = h->is_start;
			copy->is_end = h->is_end;
			copy->index = h->index;
	
			put(end_states, copy);
		}
		h = h->next;
	}

	return end_states;
}

/**
* Removes all '\n' symbols of a given string with maximum size of STRINGMAX.
* A new string will be returned. 
*/
char * no_new_line(const char*s)
{
	int i;
	int j=0;
	char* new = NULL;
	if(strnlen(s, STRINGMAX) >0){
		new	= (char*) malloc (STRINGMAX * sizeof(char));	
	
		for(i = 0 ; i < STRINGMAX; i++)
		{		
			if(s[i] != '\n')
			{		
				new[j] = s[i];
				if(s[i]=='\0')
					break;
				j++;
			}
		}
	}
	return new;
}

/**
* Checks if the alphabet of the automaton consists of single symbols
* Returns   1 if true,
* otherwise 0.
*/
int is_single_symbol_dfa(t_list* labels)
{
	t_element* h = labels->first;
	while(h != NULL)
	{
		if(strnlen(h->label, 2) > 1)
			return 0;
		h = h->next;
	}
	return 1;
}
