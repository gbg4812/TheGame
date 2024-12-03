# Add here any extra .o player files you want to link to the executable
EXTRA_OBJ =

# Configuration
OPTIMIZE = 3 # Optimization level    (0 to 3)
DEBUG    = 1 # Compile for debugging (0 or 1)
PROFILE  = 0 # Compile for profile   (0 or 1)

# For debugging matches
# OPTIMIZE = 0, DEBUG = 1
# You cannot use the Dummy. You have to debug using only your players

# Flags
ifeq ($(strip $(PROFILE)),1)
	PROFILEFLAGS=-pg
endif

ifeq ($(strip $(DEBUG)),1)
	DEBUGFLAGS=-g -O0 -fno-inline -D_GLIBCXX_DEBUG 
endif

CXXFLAGS = -std=c++17 -Wall -Wno-unused-variable -fPIC $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE))
LDFLAGS  = -std=c++11                            $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE))


# The following two lines will detect all your players (files matching "AI*.cc")

PLAYERS_SRC = $(wildcard AI*.cc)
PLAYERS_OBJ = $(patsubst %.cc, %.o, $(PLAYERS_SRC)) $(EXTRA_OBJ)

# Rules

OBJ = Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o Utils.o 

all: Game

clean:
	rm -rf Game  *.o *.exe Makefile.deps OUT*.txt

view:
	firefox ./Viewer/viewer.html&

test4:
	./Game Ron Ron Ron Hermione -s 1 < default.cnf 2>&1 > OUT1.txt | grep score
	./Game Ron Ron Ron Hermione -s 2 < default.cnf 2>&1 > OUT2.txt | grep score
	./Game Ron Ron Ron Hermione -s 3 < default.cnf 2>&1 > OUT3.txt | grep score
	./Game Ron Ron Ron Hermione -s 4 < default.cnf 2>&1 > OUT4.txt | grep score
test1:
	./Game Ron Ron Ron Hermione -s 1 < default.cnf > OUT.txt

Game:  $(OBJ) Game.o Main.o $(PLAYERS_OBJ) 
	$(CXX) $^ -o $@ $(LDFLAGS)

SecGame: $(OBJ) SecGame.o SecMain.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lrt

%.exe: %.o $(OBJ) SecGame.o SecMain.o 
	$(CXX) $^ -o $@ $(LDFLAGS) -lrt

Makefile.deps: *.cc
	$(CXX) $(CXXFLAGS) -MM *.cc > Makefile.deps

include Makefile.deps
