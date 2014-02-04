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
#include <dlfcn.h>
#include "interface.h"
#include "gram.tab.h"
#include "graphval.h"

int linenr = 1;
extern t_graphnode *treeroot;

void skip_comment();

%}

%option bison-bridge bison-locations
%option noyywrap

%%
[ \t\r]				; /* ignore whitespaces */
"\n"				{ linenr++;  }
"/*"				{ skip_comment(); }
"//".*"\n"			; /* ignore comments */
"digraph"			{ yylloc->linenr = linenr; yylloc->lexeme = yytext; return yDIGRAPH;  }
"graph"				{ yylloc->linenr = linenr; yylloc->lexeme = yytext; strncpy(yylval->id,yytext,STRINGMAX); return yGRAPH;    }
"subgraph"			{ yylloc->linenr = linenr; yylloc->lexeme = yytext; return ySUBGRAPH; }
"{"				{ yylloc->linenr = linenr; yylloc->lexeme = yytext; return yGKA;      }
"}"				{ yylloc->linenr = linenr; yylloc->lexeme = yytext; return yGKZ;      }
"["				{ yylloc->linenr = linenr; yylloc->lexeme = yytext; return yEKA;      }
"]"				{ yylloc->linenr = linenr; yylloc->lexeme = yytext; return yEKZ;      }
"="				{ yylloc->linenr = linenr; yylloc->lexeme = yytext; return yASGNOP;   }
"->"				{ yylloc->linenr = linenr; yylloc->lexeme = yytext; strncpy(yylval->id,yytext,ARROWMAX); return yPFEIL;    }
"--"				{ yylloc->linenr = linenr; yylloc->lexeme = yytext; strncpy(yylval->id,yytext,ARROWMAX); return yPFEIL;    }
";"				; /* ignore semicolon */
"\""[^\"]*"\""			{ yylloc->linenr = linenr; yylloc->lexeme = yytext; strncpy(yylval->id,(yytext+1),STRINGMAX); yylval->id[strlen(yytext)-2] = '\0';  return yID;  }
[a-zA-Z][a-zA-Z0-9_]*		{ yylloc->linenr = linenr; yylloc->lexeme = yytext; strncpy(yylval->id,yytext,STRINGMAX); return yID; }
"0"|[1-9][0-9]*			{ yylloc->linenr = linenr; yylloc->lexeme = yytext; strncpy(yylval->id,yytext,STRINGMAX); return yID; }
("0"|[1-9][0-9]*)\.[0-9]*	{ yylloc->linenr = linenr; yylloc->lexeme = yytext; strncpy(yylval->id,yytext,STRINGMAX); return yID; }
.				{ fprintf(stderr,"Lexikal error in line %d at '%s'\n",linenr, yytext); exit(EXIT_FAILURE); }

%%

void skip_comment()
{
	char c = input();
	while(1)
	{
		if(c == '*')
		{
			c = input();
			if(c == '/')
				return ;
		}
		else if(c == '\n')
		{
			linenr++;
			c = input();
		}
		else
			c = input();
	}
}

int yyerror(char *s)
{
	return 1;
}

void helptext(int argc, char *argv[])
{
	fprintf(stderr,"%s usage:\n",argv[0]);
	fprintf(stderr,"\t%s PLUGIN INPUTFILE [PLUGINPARAMETER]\n",argv[0]);
	fprintf(stderr,"\n");
	fprintf(stderr,"INPUTFILE:\tfilename or -\n");
	fprintf(stderr,"\tif - then stdin is used as input.\n");
	exit(EXIT_FAILURE);
}

void freetree(t_graphnode *node)
{
	if(!node)
		return ;
	freetree(node->left);
	freetree(node->right);

	free(node);
}

int main(int argc, char *argv[])
{
	char libname[STRINGMAX];
	char pluginstarter[STRINGMAX];
	char *liberror;
	void *module;
	int retcode = EXIT_FAILURE;
	int closefile = 0;
	f_plugin plugin;

	libname[0] = '\0';
	pluginstarter[0] = '\0';

	if(argc >= 3)
	{

		if(strncmp(argv[2],"-",2) == 0)
		{
			yyin = stdin;
		}
		else
		{
			yyin = fopen(argv[2],"r");

			if(yyin == NULL)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			closefile = 1;
		}

		fprintf(stderr,"Start Parser...\n");
		yyparse();
		if(treeroot == NULL)
		{
			fprintf(stderr,"...empty tree...\n");
		}
		else
		{
			/** libname has to be of the form
			 *  	'gv_' argv[1] '.so'
			 *  	like:
			 *  	- gv_markov.so
			 *  	- gv_dfa.so
			 */
			strncat(libname,"gv_",STRINGMAX);
			strncat(libname,argv[1],STRINGMAX);
			strncat(libname,".so",STRINGMAX);

			strncat(pluginstarter,argv[1],STRINGMAX);
			strncat(pluginstarter,"_start",STRINGMAX);

			fprintf(stderr,"...search plugin '%s'...\n",libname);
			module = dlopen(libname, RTLD_LAZY);
			if (!module)
			{
				fprintf(stderr, "Couldn't open lib: %s\n",dlerror());
				exit(EXIT_FAILURE);
			}
			fprintf(stderr,"...search pluginstarter...\n");
			liberror = dlerror();			// call dlerror, to clean previous errors
			plugin = dlsym(module, pluginstarter);	// try to load pluginfunction
			liberror = dlerror();			// check if dlsym ended in an error
			if (liberror)
			{
				fprintf(stderr, "Couldn't load function: %s\n",liberror);
				dlclose(module);
				exit(EXIT_FAILURE);
			}
			fprintf(stderr,"...start plugin...\n");

			retcode = (*plugin)(treeroot, argc-3, &argv[3]);

			dlclose(module);
		}
		
		freetree(treeroot);
		treeroot = NULL;

		if(closefile == 1)
			fclose(yyin);

		fprintf(stderr,"finished\n");

		return retcode;
	}

	helptext(argc,argv);

	return EXIT_SUCCESS;
}
