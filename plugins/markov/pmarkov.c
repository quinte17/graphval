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
#include "pmarkov.h"

static t_graphnode* get_property(t_graphnode *list, char *value)
{
	t_graphnode *tmp;

	tmp = list;
	while(tmp)
	{
		if(tmp->right && tmp->right->type == PROP)
		{
			if(strncmp(value, tmp->right->name, STRINGMAX) == 0)
				return tmp->right;
		}
		tmp = tmp->left;
	}

	return NULL;
}

static t_list* create_list_element(t_list *src, t_list *dest, char *name, char *label, int index, int used, t_graphnode *subgraph, t_list *next)
{
	t_list *tmp;

	tmp = malloc(sizeof(t_list));
	if(!tmp)
	{
		fprintf(stderr,"gv_markov: could not allocate memory for list-element.\n");
		exit(1);
	}

	tmp->src      = src;
	tmp->dest     = dest;
	tmp->name     = name;
	tmp->label    = label;
	tmp->index    = index;
	tmp->used     = used;
	tmp->subgraph = subgraph;
	tmp->next     = next;

	return tmp;
}

static t_list* create_id_element(char *name, char *label, int used, t_graphnode *subgraph)
{
	t_list *tmp = id_elements;

	// search name if node is already known
	while(tmp)
	{
		if(strncmp(tmp->name,name,STRINGMAX) == 0)
		{
			// name is known, refresh relevant fields and return
			// if label is not used yet
			if(!tmp->label)
				tmp->label = label;
			// if node is not used yet
			if(tmp->used == 0)
				tmp->used = used;
			// if subgraph is not used yet
			if(!tmp->subgraph)
				tmp->subgraph = subgraph;

			return tmp;
		}
		tmp = tmp->next;
	}

	// node is not known
	// create one
	id_elements = create_list_element(NULL, NULL, name, label, 0, used, subgraph, id_elements);

	return id_elements;
}

static void create_cname_element(char *label)
{
	t_list *tmp = c_names;
	char *ctmp;

	// search name if name is already known
	while(tmp)
	{
		if(strncmp(tmp->name,label,STRINGMAX) == 0)
			return;

		tmp = tmp->next;
	}

	// name is not know, so create one
	ctmp = malloc(sizeof(char)*STRINGMAX);
	strncpy(ctmp,label,STRINGMAX);

	tmp = malloc(sizeof(*tmp));

	// get it into the list
	tmp->next = c_names;
	tmp->name = ctmp;
	c_names = tmp;
}

static void scan_label(char *label)
{
	char tmp[STRINGMAX];
	int i;
	int start = -1;		// startindex
	int end = -1;		// endindex
	int t;

	if(label == NULL)
		return;

	// scan label for c-names and remember them if needed.
	// they have to apear in the ouput file as extern float.
	t = strlen(label);
	for(i = 0; i < t; i++)
	{
		if(
			(
				('a' <= label[i] && label[i] <= 'z') ||
				('A' <= label[i] && label[i] <= 'Z') ||
				'_' == label[i]
			) &&
			start == -1
		)
		{
			// found start of c-name
			start = i;
		}

		if(
			!(
				('a' <= label[i] && label[i] <= 'z') ||
				('A' <= label[i] && label[i] <= 'Z') ||
				('0' <= label[i] && label[i] <= '9') ||
				'_' == label[i]
			) &&
			start > -1
		)
		{
			// found end of c-name, so remember it
			end = i-1;
			strncpy(tmp, label+start, end-start + 1);
			tmp[end-start+1] = '\0';
			create_cname_element(tmp);

			start = -1;
			end = -1;
		}

		if(
			i+1 == t && start > -1
		)
		{
			// reached end of string.
			// if we found a starting c-name then remember it
			end = i;
			strncpy(tmp, label+start, end-start + 1);
			tmp[end-start+1] = '\0';
			create_cname_element(tmp);

			start = -1;
			end = -1;
		}
	}
}

static void create_did_element(t_list *src, t_list *dest, char *label)
{
	// scan label for c-names
	scan_label(label);

	// create list element
	// and put it ontop of the list
	did_elements = create_list_element(src, dest, NULL, label, 0, 0, NULL, did_elements);
}

static void get_relevant_data(t_graphnode *graph)
{
	t_graphnode *subgraph;
	t_graphnode *tmp;
	t_graphnode *tmp2;

	// get subgraph information of the selected node
	if(graph->type == SUBGRAPH)
		subgraph = graph;
	else
		subgraph = NULL;
	
	// there are only list elements to the left
	tmp = graph->left;
	while(tmp)
	{
		if(tmp->right)
		{
			if(tmp->right->type == ID)
			{
				// remember ID as not used
				tmp2 = get_property(tmp->right->left,"label");
				if(tmp2 && strnlen(tmp2->value,STRINGMAX) > 0 && strncmp(tmp2->value,"\\N",STRINGMAX) != 0)
					create_id_element(tmp->right->name, tmp2->value, 0, subgraph);
				else
					create_id_element(tmp->right->name, NULL, 0, subgraph);
			}
			else if(tmp->right->type == DID)
			{
				// remember both parts of the transition
				// if no label is given, then mark both nodes as used
				tmp2 = get_property(tmp->right->left,"label");
				if(tmp2)
				{
					create_did_element(
						create_id_element(tmp->right->name, NULL, 1, NULL),
						create_id_element(tmp->right->value, NULL, 1, NULL),
						tmp2->value
					);
				}
				else
				{
					// mark both nodes as used
					create_id_element(tmp->right->name, NULL, 1, NULL);
					create_id_element(tmp->right->value, NULL, 1, NULL);
				}
			}
			else if(tmp->right->type == SUBGRAPH)
			{
				get_relevant_data(tmp->right);
			}
		}
		tmp = tmp->left;
	}
}

static void free_unused_ids()
{
	t_list *tmp;
	t_list *del;

	// delete first elements if unused
	while(id_elements && id_elements->used == 0)
	{
		del = id_elements;
		id_elements = id_elements->next;
		free(del);
	}

	// delete unused elements
	// works only if unused element is not the first
	tmp = id_elements;
	while(tmp && tmp->next)
	{
		if(tmp->next->used == 0)
		{
			del = tmp->next;
			tmp->next = tmp->next->next;
			free(del);
		}
		tmp = tmp->next;
	}
}

static void free_all_lists()
{
	t_list *tmp;

	// free all id list-elements
	while(id_elements)
	{
		tmp = id_elements;
		id_elements = id_elements->next;
		free(tmp);
	}

	// free all did list-elements
	while(did_elements)
	{
		tmp = did_elements;
		did_elements = did_elements->next;
		free(tmp);
	}

	while(c_names)
	{
		tmp = c_names;
		c_names = c_names->next;
		free(tmp->name);
		free(tmp);
	}
}

static void update_index()
{
	t_list *tmp = id_elements;
	int i = 0;
	while(tmp)
	{
		tmp->index = i;
		i++;

		if(tmp->subgraph != NULL)
			sub_count++;

		tmp = tmp->next;
	}
	id_count = i;
}

static void emit_c()
{
	
	char **table;
	int i,j;
	t_list *tmp = did_elements; 
	t_list *ztmp = id_elements;

	// Create and initialise virtual table with one dimension
	table = (char **) malloc(sizeof(char*)*id_count*id_count);
	if(table == NULL)
	{
		fprintf(stderr,"gv_markov: could not allocate memory for emit table.\n");
		exit(1);
	}
	for(i=0; i < id_count; i++)
	{
		for(j=0; j < id_count; j++)
		{
			*(table+i*id_count+j) = NULL;
		}
	}

	// Get transitions into table
	while(tmp != NULL)
	{
		*(table+tmp->src->index*id_count+tmp->dest->index) = tmp->label;
		tmp = tmp->next;
	}
	
	// Output of C-code
	printf("/* C output generated by graphval's markov plugin\n */");
	printf("#include <markov.h>\n");
	printf("\n");

	// emit cnames as extern
	tmp = c_names;
	if(tmp)
	{
		while(tmp)
		{
			printf("extern float %s;\n",tmp->name);
			tmp = tmp->next;
		}
		printf("\n");
	}

	// matrix_transition
	printf("Matrix* matrix_transition()\n");
	printf("{\n");
	printf("\tMatrix *tmp;\n");
	printf("\n");
	printf("\ttmp = matrix_new(%d,%d);\n",id_count,id_count);
	printf("\n");

	// print table:
	for(i=0; i < id_count; i++)
	{
		for(j=0; j < id_count; j++)
		{
			if(*(table+i*id_count+j) != NULL)
			{
				printf("\ttmp->m[%d][%d] = %s;",i+1,j+1,*(table+i*id_count+j));
			}
		}
		printf("\n");
	}
	printf("\n");
	printf("\treturn tmp;\n");
	printf("}\n");
	printf("\n");

	// name_transition
	printf("Nodenames* name_transition()\n");
	printf("{\n");
	printf("\tNodenames *tmp;\n");
	printf("\ttmp = name_new(%d);\n",id_count);
	printf("\n");

	tmp = id_elements;
	i = 0;
	while(tmp)
	{
		if(tmp->label == NULL)
			printf("\tstrncpy(tmp->n[%d],\"%s\",STRMAX);\n",i+1,tmp->name);
		else
			printf("\tstrncpy(tmp->n[%d],\"%s\",STRMAX);\n",i+1,tmp->label);
		i++;
		tmp = tmp->next;
	}
	printf("\n");
	printf("\treturn tmp;\n");
	printf("}\n");
	printf("\n");

	// cluster_transition
	printf("Matrix* cluster_transition()\n");
	printf("{\n");

	if(sub_count == 0)
	{
		printf("\treturn NULL;\n");
	}
	else
	{
		printf("\tint i,j;\n");
		printf("\tMatrix *tmp1,*tmp2;\n");
		printf("\tint nodes[%d] = {",sub_count);
		i = 0;
		ztmp = id_elements;
		while(ztmp != NULL)
		{
			if(ztmp->subgraph != NULL)
			{
				i++;
				if(i > 1)
					printf(",");
				printf("%d",ztmp->index+1);
			}
			ztmp = ztmp->next;
		}
		printf("};\n");
		printf("\n");

		// matrix erzeugen:
		printf("\tj = %d;\n",sub_count - 1);
		printf("\ttmp1 = matrix_transition();\n");
		printf("\tfor(i = tmp1->number_rows; i > 0; i--)\n");
		printf("\t{\n");
		printf("\t\tif(j >= 0 && i == nodes[j])\n");
		printf("\t\t{\n");
		printf("\t\t\tj--;\n");
		printf("\t\t\tcontinue;\n");
		printf("\t\t}\n");
		printf("\n");
		printf("\t\ttmp2 = matrix_reduce(tmp1,i);\n");
		printf("\t\tmatrix_free(tmp1);\n");
		printf("\t\ttmp1 = tmp2;\n");
		printf("\t}\n");
		printf("\n");
		printf("\treturn tmp1;\n");
	}

	printf("}\n");
	printf("\n");

	// cluster_nametransition
	printf("Nodenames* cluster_nametransition()\n");
	printf("{\n");

	if(sub_count == 0)
	{
		printf("\treturn NULL;\n");
	}
	else
	{
		printf("\tint i,j;\n");
		printf("\tNodenames *tmp1,*tmp2;\n");
		printf("\tint nodes[%d] = {",sub_count);
		i = 0;
		ztmp = id_elements;
		while(ztmp != NULL)
		{
			if(ztmp->subgraph != NULL)
			{
				i++;
				if(i > 1)
					printf(",");
				printf("%d",ztmp->index+1);
			}
			ztmp = ztmp->next;
		}
		printf("};\n");
		printf("\n");

		// names erzeugen:
		printf("\tj = %d;\n",sub_count - 1);
		printf("\ttmp1 = name_transition();\n");
		printf("\tfor(i = tmp1->number_names; i > 0; i--)\n");
		printf("\t{\n");
		printf("\t\tif(j >= 0 && i == nodes[j])\n");
		printf("\t\t{\n");
		printf("\t\t\tj--;\n");
		printf("\t\t\tcontinue;\n");
		printf("\t\t}\n");
		printf("\n");
		printf("\t\ttmp2 = name_reduce(tmp1,i);\n");
		printf("\t\tname_free(tmp1);\n");
		printf("\t\ttmp1 = tmp2;\n");
		printf("\t}\n");
		printf("\n");
		printf("\treturn tmp1;\n");
	}

	printf("}\n");
	printf("\n");

	free(table);
}

static void emit_octave()
{
	
	char **table;
	int i,j;
	t_list *tmp = did_elements; 
	t_list *ztmp = id_elements;

	// Create and initialise virtual table with one dimension
	table = (char **) malloc(sizeof(char*)*id_count*id_count);
	if(table == NULL)
	{
		fprintf(stderr,"gv_markov: could not allocate memory for emit table.\n");
		exit(1);
	}
	for(i=0; i < id_count; i++)
	{
		for(j=0; j < id_count; j++)
		{
			*(table+i*id_count+j) = NULL;
		}
	}

	// Get transitions into table
	while(tmp != NULL)
	{
		*(table+tmp->src->index*id_count+tmp->dest->index) = tmp->label;
		tmp = tmp->next;
	}
	
	// Output of Octave-Code
	printf("# Octave output generated by graphval's markov plugin\n");
	printf("\n");

	// Matrix:
	printf("M = [\n");

	for(i=0; i < id_count; i++)
	{
		printf("\t");
		for(j=0; j < id_count; j++)
		{
			if(*(table+i*id_count+j) != NULL)
			{
				printf(" %s",*(table+i*id_count+j));
			}
			else
				printf(" 0.0");
		}
		if(id_count - 1 != i)
			printf(";\n");
		else
			printf("\n");
	}
	printf("]\n");
	printf("\n");

	// Name Matrix
	printf("MName = [\n");
	tmp = id_elements;
	while(tmp)
	{
		if(tmp->label == NULL)
			printf("\t\"%s\";\n",tmp->name);
		else
			printf("\t\"%s\";\n",tmp->label);
		tmp = tmp->next;
	}
	printf("]\n");
	printf("\n");

	// cluster matrix
	if(sub_count != 0)
	{
		printf("MCluster = [\n");
		i = 0;
		ztmp = id_elements;
		while(ztmp != NULL)
		{
			if(ztmp->subgraph != NULL)
			{
				i++;
				if(i > 1)
					printf(";\n");
				printf("\tM(%d,:)",ztmp->index+1);
			}
			ztmp = ztmp->next;
		}
		printf("\n");
		printf("];\n");

		printf("MCluster = [\n");
		i = 0;
		ztmp = id_elements;
		while(ztmp != NULL)
		{
			if(ztmp->subgraph != NULL)
			{
				i++;
				if(i > 1)
					printf(", ");
				printf("\tMCluster(:,%d)",ztmp->index+1);
			}
			ztmp = ztmp->next;
		}
		printf("\n");
		printf("]\n");
	}
	else
	{
		printf("MCluster = []\n");
	}
	printf("\n");

	// clustername matrix
	if(sub_count != 0)
	{
		printf("MNameCluster = [\n");
		i = 0;
		ztmp = id_elements;
		while(ztmp != NULL)
		{
			if(ztmp->subgraph != NULL)
			{
				i++;
				if(i > 1)
					printf(";\n");
				printf("\tMName(%d,:)",ztmp->index+1);
			}
			ztmp = ztmp->next;
		}
		printf("\n");

		printf("]\n");
		printf("\n");
	}
	else
	{
		printf("MNameCluster = []\n");
	}
	printf("\n");

	free(table);
}

int markov_start(t_graphnode *graph, int argc, char *argv[])
{
	/** switch_output has several meanings:
	 *  0:	Default output. In this Case C.
	 *  1:	C
	 *  2:	Octave
	 */
	int switch_output = 0;
	int i;

	if(argc > 0)
	{
		for(i = 0; i < argc; i++)
		{
			if(strcmp(argv[i],"-o") == 0)
			{
				switch_output = 2;
			}
			else if(strcmp(argv[i],"-c") == 0)
			{
				switch_output = 1;
			}
		}
	}
	// set default value if not found or no parameter given
	if(switch_output == 0)
		switch_output = 1;

	fprintf(stderr,"...gv_markov started...\n");

	fprintf(stderr,"...gv_markov searching relevant data...");
	get_relevant_data(graph);
	free_unused_ids();
	update_index();
	fprintf(stderr,"done\n");

	fprintf(stderr,"...gv_markov emitting ");
	switch(switch_output)
	{
		case 1:	
			fprintf(stderr,"c-code");
			emit_c();
			break;
		case 2:	
			fprintf(stderr,"octave-code");
			emit_octave();
			break;
		default:
			fprintf(stderr,"\n\n serious error %d \n\n",switch_output);
			free_all_lists();
			return EXIT_FAILURE;
	}
	fprintf(stderr," done\n");

	fprintf(stderr,"...gv_markov cleaning up...");
	free_all_lists();
	fprintf(stderr,"done\n");

	fprintf(stderr,"...gv_markov ended...\n");

	return EXIT_SUCCESS;
}
