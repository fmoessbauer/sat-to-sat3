CXX=g++
CXX_FLAGS=-std=c++11
SOURCES=main.cpp
OUTPUT=sat3_converter
USE_OMP=no
ifeq ($(USE_OMP),yes)
	CXX_FLAGS+= -fopenmp
endif

release:
	$(CXX) $(CXX_FLAGS) -O3 -Ofast $(SOURCES) -o $(OUTPUT)

debug:
	$(CXX) $(CXX_FLAGS) -g $(SOURCES) -o $(OUTPUT)

prof:
	$(CXX) $(CXX_FLAGS) -no-pie -O3 -pg $(SOURCES) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

all: release
