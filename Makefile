
################################################################################################
#
#                                cfglp : A CFG Language Processor
#                                --------------------------------
#
#               About:
#               Implemented by Uday Khedker (http://www.cse.iitb.ac.in/~uday) for 
#               the courses cs302+cs306: Language Processors (theory and lab) at
#               IIT Bombay. Release date Jan 6, 2013. Copyrights reserved by Uday
#               Khedker. This implemenation has been made available purely for
#               academic purposes without any warranty of any kind.
#
#               Please see the README file for some details. A doxygen generated
#               documentation can be found at http://www.cse.iitb.ac.in/~uday/cfglp
#
#
################################################################################################


#### The files are divided in the following groups
#    Files dealing with AST object
#    Files dealing with PROGRAM object
#    Files dealing with SYMTAB object
#    Core files which include the generated object files too.

AST_OBJ = ast-build.o ast-print.o ast-typecheck.o ast-evaluate.o ast-compile.o
PROG_OBJ = program-build.o program-print.o program-evaluate.o program-compile.o
SYMTAB_OBJ = symtab-build.o symtab-print.o symtab-analyse-scope.o
TARGET_OBJ = reg-alloc.o icode-build.o icode-print.o 
OBJ_CORE = support.o main.o options.o evaluate.o 
OBJ_GEN = parse.tab.o scan.o 
HEADERS_CORE = common-classes.hh ast.hh support.hh cfglp-ctx.hh symtab.hh \
               program.hh options.hh evaluate.hh reg-alloc.hh icode.hh
HEADERS_GEN = parse.tab.hh location.hh position.hh stack.hh
OBJ = $(OBJ_GEN) $(OBJ_CORE) $(AST_OBJ) $(PROG_OBJ) $(SYMTAB_OBJ) $(TARGET_OBJ)
HEADERS = $(HEADERS_CORE)

CC = g++
BISON = bison
FLEX = flex
TGT = cfglp

CFLAGS = -g 

$(TGT): $(OBJ) 
	$(CC) $(OBJ) -o $(TGT)

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $<

scan.cc: scan.ll
	$(FLEX) -oscan.cc scan.ll

parse.tab.cc parse.tab.hh: parse.yy
	$(BISON) -vd parse.yy

clean :
	rm -rf *.o cfglp parse.tab.* scan.cc *.output location.hh position.hh stack.hh
	rm -rf *.sym *.tok *.ast *.eval *.spim *.prog cscope.out tags
	rm -rf *~
