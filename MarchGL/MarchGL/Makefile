CXX := g++

CFLAGS := -Wall -fpermissive
CFLAGS += -std=c++17
CFLAGS += -D_POSIX_C_SOURCE

LIBS := -lGL -lGLU -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lstdc++fs -lfreetype -pthread -lm

LDFLAGS := -L/usr/local/lib

SRCDIR := src
OBJDIR := obj
BINDIR := bin
INCDIR := include
DEPDIR := deps

TARGET := MarchGL

SOURCES1 := $(wildcard $(SRCDIR)/*.cpp)
SOURCES2 := $(wildcard $(INCDIR)/cparse/*.cpp)
SOURCES3 := $(wildcard $(INCDIR)/ImGUI/*.cpp)
SOURCES  := $(SOURCES1) $(SOURCES2) $(SOURCES3)

OBJECTS1 := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES1))
OBJECTS2 := $(patsubst $(INCDIR)/cparse/%.cpp, $(OBJDIR)/%.o, $(SOURCES2))
OBJECTS3 := $(patsubst $(INCDIR)/ImGUI/%.cpp, $(OBJDIR)/%.o, $(SOURCES3))
OBJECTS  := $(OBJECTS1) $(OBJECTS2) $(OBJECTS3)

DEPS     := $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $(OBJECTS))


debug: CFLAGS += -Og -g3
debug: $(BINDIR)/$(TARGET)

release: CFLAGS += -O3 -g0
release: $(BINDIR)/$(TARGET)


$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	@$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR) $(DEPDIR)
	@$(CXX) -I$(INCDIR) -I$(INCDIR)/cparse -I$(INCDIR)/ImGUI -c $(CFLAGS) -o $@ \
	    -MT $@ -MMD -MP \
	    -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) \
	    $<

$(BINDIR) $(OBJDIR) $(DEPDIR):
	@mkdir -p $@


clean:
	@rm -rf $(OBJDIR) $(DEPDIR) $(BINDIR)/$(TARGET)

vpath %.cpp $(SRCDIR) $(INCDIR)/cparse $(INCDIR)/ImGUI

.PHONY: debug release clean

-include $(DEPS)