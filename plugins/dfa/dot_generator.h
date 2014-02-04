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

#ifndef DOT_GENERATOR_H
#define DOT_GENERATOR_H

#include "pdfa.h"

/**
*Prints a valid *.dot-file generated with given automaton information
*/
void generate_dfa_dot_code(char* automaton_name, t_list* states, t_list* dids, t_matrix* state_matrix);

#endif
