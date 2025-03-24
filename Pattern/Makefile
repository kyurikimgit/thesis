CXX = g++
CXXFLAGS = -I/opt/ibm/ILOG/CPLEX_Studio2211/cplex/include \
           -I/opt/ibm/ILOG/CPLEX_Studio2211/concert/include \
           -DIL_STD

LDFLAGS = -L/opt/ibm/ILOG/CPLEX_Studio2211/cplex/lib/x86-64_linux/static_pic \
          -L/opt/ibm/ILOG/CPLEX_Studio2211/concert/lib/x86-64_linux/static_pic

LIBS = -lilocplex -lconcert -lcplex -lm -lpthread

all: pattern

pattern: pattern.o
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

pattern.o: pattern.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f pattern pattern.o
