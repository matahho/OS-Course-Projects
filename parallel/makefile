PATH_SRC  = ./src
PATH_INC  = $(PATH_SRC)/include
PATH_OBJ  = ./obj
BMP_FORMAT = *.bmp

CXX       = g++
CXXFLAGS += -g
CPPFLAGS += -std=c++11 -I$(PATH_INC)

OUTPUT = Parallel.out

# ----------------------------------------------------------------------------------------

vpath %.cpp $(PATH_SRC)
vpath %.hpp $(PATH_INC)

OBJS = main.o bmp.o filters.o

# ----------------------------------------------------------------------------------------

all: $(PATH_OBJ) $(OUTPUT)

$(OUTPUT): $(addprefix $(PATH_OBJ)/, $(OBJS))
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

$(PATH_OBJ)/main.o: main.cpp $(PATH_INC)/bmp.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(PATH_OBJ)/bmp.o: bmp.cpp $(PATH_INC)/bmp.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(PATH_OBJ)/filters.o: filters.cpp $(PATH_INC)/filters.h $(PATH_INC)/bmp.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

# ----------------------------------------------------------------------------------------

$(PATH_OBJ): ; mkdir -p $@

.PHONY: all clean run

run: ./$(OUTPUT) $(ARGS)

clean:
	rm -rf $(PATH_OBJ) $(OUTPUT)
	rm -rf $(BMP_FORMAT)
