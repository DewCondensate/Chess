Exec = main
OPTS = -O3 -pthread -std=c++11 -g

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

GXX = g++

.PHONY: all
all: $(Exec)

.PHONY: check
check:
	echo Objects are $(objects)

$(objects): %.o: %.cpp *.hpp
	$(GXX) -c $(OPTS) $< -o $@

$(Exec): $(objects)
	$(GXX) $(OPTS) $(objects) $(cu_objects) -o $(Exec)
	echo $(GXX) $(OPTS) $(objects) -o $(Exec)

.PHONY: clean
clean:
	-rm *.o $(Exec)