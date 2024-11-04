CC = g++ -fdiagnostics-color=always
FLAGS = -std=c++17 -g -O3 -Wall -I.
LIBS = -pthread

BUILD = build
OBJ = build/obj

MATCH = match

all : dir $(BUILD)/match

dir: $(OBJ)

$(OBJ) :
	mkdir -p $(OBJ)

#################### start ####################

$(BUILD)/match: $(OBJ)/main.o \
		$(OBJ)/graph.o \
		$(OBJ)/match.o 
	$(CC) $(FLAGS) $(OBJ)/main.o \
		$(OBJ)/graph.o \
		$(OBJ)/match.o \
		-o $(BUILD)/match $(LIBS)

$(OBJ)/main.o: $(MATCH)/main.cpp \
		$(MATCH)/graph.h \
		$(MATCH)/match.h 
	$(CC) -c $(FLAGS) $(MATCH)/main.cpp -o $(OBJ)/main.o

#################### matching ####################

$(OBJ)/graph.o: $(MATCH)/graph.cpp \
		$(MATCH)/graph.h
	$(CC) -c $(FLAGS) $(MATCH)/graph.cpp \
		-o $(OBJ)/graph.o

$(OBJ)/match.o: $(MATCH)/match.cpp \
		$(MATCH)/graph.h \
		$(MATCH)/match.h 
	$(CC) -c $(FLAGS) $(MATCH)/match.cpp \
		-o $(OBJ)/match.o

#################### end ####################

.PHONY: clean

clean: 
	rm -r ${BUILD}
