CC = g++
FLAGS = -g -c -std=c++11

SOURCEDIR = src
BUILDDIR = build
RANDDIR = randomizer
ADDRDIR = addresser

EXECUTABLE = sort
SOURCES = $(wildcard $(SOURCEDIR)/*.cpp) $(wildcard $(SOURCEDIR)/$(ADDRDIR)/*.cpp) $(wildcard $(SOURCEDIR)/$(RANDDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

all: dir ini_files $(BUILDDIR)/$(EXECUTABLE)

dir:
	mkdir -p $(BUILDDIR) $(BUILDDIR)/$(ADDRDIR) $(BUILDDIR)/$(RANDDIR)

ini_files:
	for x in ini/*.ini ; do cp "$$x" $(BUILDDIR) ; done

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CC) $(FLAGS) $< -o $@

clean:
	rm -f $(BUILDDIR)/$(RANDDIR)/*o $(BUILDDIR)/$(ADDRDIR)/*o $(BUILDDIR)/*o $(BUILDDIR)/$(EXECUTABLE)
