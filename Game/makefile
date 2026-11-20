Exec = main
OPTS = -O3 -pthread -std=c++11 -g

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

GXX = g++

DEFINES = -DEFFICIENT_UPDATE_HEURISTIC

.PHONY: all
all: $(Exec)

.PHONY: check
check:
	echo Objects are $(objects)

$(objects): %.o: %.cpp *.hpp
	$(GXX) -c $(OPTS) $(DEFINES) $(EXTRA_DEFINE) $< -o $@

$(Exec): $(objects)
	$(GXX) $(OPTS) $(DEFINES) $(EXTRA_DEFINE) $(objects) $(cu_objects) -o $(Exec)
	echo $(GXX) $(OPTS) $(DEFINES) $(EXTRA_DEFINE) $(objects) -o $(Exec)

.PHONY: clean
clean:
	-rm *.o $(Exec)