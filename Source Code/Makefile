# Makefile
# SIMFECompiler
# Created by Kaê Angeli Coutinho and Ricardo Oliete Ogata
# GNU GPL V2

# Constants

PROJECT_DIRECTORY_PATH := ${CURDIR}
MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PPMACROS = -DPROJECT_DIRECTORY_PATH='"$(PROJECT_DIRECTORY_PATH)"' -DMAKEFILE_PATH='"$(MAKEFILE_PATH)"'
OUTPUT = SIMFECompiler
OBJECTS = $(OUTPUT).o SIMFECompilerFunctions.o DFA.o DFABuilder.o State.o StateBuilder.o Transition.o
CC = g++
CFLAGS = -w ${PPMACROS} -I/usr/local/Cellar/boost/1.58.0/include/ -L/usr/local/Cellar/boost/1.58.0/lib/
LDFLAGS = -lboost_system -lboost_filesystem

# Default target

all: libraries
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTPUT) $(LDFLAGS)

# Libraries target

libraries: $(OBJECTS)

# File targets

# Object files

$(OUTPUT).o: $(OUTPUT).cpp
	$(CC) $(CFLAGS) -c $<

SIMFECompilerFunctions.o: SIMFECompilerFunctions.cpp SIMFECompilerFunctions.h SIMFECompilerDefinitions.h SIMFECompilerDependencies.h
	$(CC) $(CFLAGS) -c $<

DFA.o: DFA/DFA.cpp DFA/DFA.h
	$(CC) $(CFLAGS) -c $<

DFABuilder.o: DFA/DFABuilder.cpp DFA/DFABuilder.h
	$(CC) $(CFLAGS) -c $<

State.o: DFA/State.cpp DFA/State.h
	$(CC) $(CFLAGS) -c $<

StateBuilder.o: DFA/StateBuilder.cpp DFA/StateBuilder.h
	$(CC) $(CFLAGS) -c $<

Transition.o: DFA/Transition.cpp DFA/Transition.h
	$(CC) $(CFLAGS) -c $<

# Clean targets

clean:
	rm -rf $(OBJECTS)

cleanall:
	rm -rf $(OBJECTS) $(OUTPUT)