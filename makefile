#
# Makefile for `Super Ultra Mambo Tango Foxtrot Cannon Wars'
# 	Tommy Reddad
# 	260403466

export PROJ	= sumtf-cannonwars
export CPP	= g++
export CFLAGS	= -Wall
#export LFLAGS 	= -Wl,-rpath,lib -m32 -Wall -lGL -lsfml-graphics -lsfml-window -lsfml-system
#export LFLAGS 	= -Wl,-rpath,lib -Wall -lGL -lsfml-graphics -lsfml-window -lsfml-system
export LFLAGS 	= -Wall -lGL -lsfml-graphics -lsfml-window -lsfml-system


DIRS 		= . perlin physics
SRCDIR 		= src
BUILDDIR 	= build
SRC		= $(foreach DIR,$(DIRS),$(wildcard $(SRCDIR)/$(DIR)/*.cpp))
OBJ 		= $(patsubst $(SRCDIR)%,$(BUILDDIR)%,$(SRC:.cpp=.o))

$(PROJ): $(OBJ)
	$(CPP) -o $(PROJ) $(OBJ) $(LFLAGS)

$(OBJ): $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.hpp
	$(CPP) -o $@ -c $< $(CFLAGS)

clean:
	rm -f $(OBJ) $(PROJ)

