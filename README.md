__Introduction:__


This is the VSSimulator, a simulator developed for vector scheduling (VS) and vector bin packing (VBP) problems.

It includes components for implementing and evaluating your own algorithms with a 2D benchmark (CT01), and a 20D benchmark (BP16). 

Additionally, it contains the impelementation of a wide range of algorithms from literature for users to play around with. 

These include bin-centric local search (LS), pattern heuristic, first-fit (FF), best-fit (BF), best-fit decreasing (BFD), first-fit decreasing (FFD), DotProduct, Simulated Evolution, etc. 

A summary of some key files for understanding and utilising the simulators are listed below:

	VSSimKernel.cpp : The main file of the simulator, responsible for loading benchmarks, running algorithms on the benchmarks, storing results.

	Job.h : Encodes items.

	Machine.h : Encodes bins.

	MathFunctions.h, Sorting.h : A suite of vector calculations, cost functions, and norms.

	Pani.h : Implementation of Bin-centric Heuristic, Hybrid Heuristic, and MBP Heuristic (the VS version).

	Kou.h : Implementation of Item-centric Heuristic (FF, BF, FFD, BFD).

	LS.h : Implementatiion of Bin-centric Local Search Heuristic. 

	ArcFlow.h : An adaption of the ArcFlow algorithm into the VSSimulator.

	*Loader.cpp : Loaders of benchmarks in the Benchmarks folder.

	algorithmNames.cpp : Loaders of algorithms.



__Usages:__

make -f Makefile.Libraries

make -f Makefile.VSSimKernel

Notice that Gurobi and Cplex are required to build the exact and the Pattern Heuristic.

Inside the TestRunner, scripts to run experiments are provided.

__Benchmarks:__

With the default CXLoader, item vectors can be generated uniformly in the given range.

Well-known benchmarks CT01, that introduced by Spieksma, Caprara and Toth, and BP16 which is introduced by Brandao and Pedroso are provided in the Benchmarks folder. 
