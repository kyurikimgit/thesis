#include "LS.h"
#include <thread>
#include <cstdlib>
#include <ctime>
#include <mutex> 
#include <cstdlib>  // For system function
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
extern "C"
#define THREADS 1


//#define PRINTMACHINE
int planJobs(int &counter, std::vector<Machine> **out_machines, std::vector<Job> *jq, std::vector<Machine> *mq, float _eps) {
	int dims = jq->at(0).getDimensions();
	int noNegativeReducedCost;
	int miniMQsize = INT_MAX;
	auto start = std::chrono::high_resolution_clock::now();
	//        MathFunctions::printVectors(jq);
	double optimalSolution = 0;
	std::vector<Job> *_jq[THREADS];
	for(unsigned i = 0; i < THREADS ; i++)
		_jq[i] = new std::vector<Job>; 
	MachineGenerator mach_gen(dims);
	std::vector<Machine>* _mq[THREADS];
	for(unsigned i = 0; i < THREADS ; i++)
		_mq[i] = mach_gen.generateMachines(0, 1.0);
	// 1 max, 2 L1, 3 L2

	float load[dims];
	for(unsigned i = 0; i < dims; i++)
		load[i] = 0;
	float* tmp_mv;
	std::vector<int> idVector;
	for (int i = 0; i < jq->size(); ++i) {
		idVector.push_back(i);
		tmp_mv = jq->at(i).getKPIVec();
		MathFunctions::VectorAddition(load, tmp_mv, dims);
	}
	double lowerBound = std::floor(MathFunctions::max(load,dims))+1;
	printf("lowerBounnd is %lf\n",lowerBound);

	for(unsigned i = 0; i < dims; i++){
              load[i] = 1;
              //load[i] = 1-(load[i] - lowerBound);
                printf("D %d load is %lf\n",i,load[i]);
        }
	int _jqSize = jq->size();
	int iteration = INT_MAX;
	while(iteration > 0){
	std::vector<std::vector<double>> patterns;
	std::mutex patternsMutex;
	// aPattern consists of a set of item IDs and the costFunction(RC)
	std::vector<double> aPattern;
		iteration--;
		// save job into items.csv
		std::string filename = "items.csv";
		std::ofstream outputFile(filename);
		if (!outputFile.is_open()) {
			std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
			return 0;
		}

		for (int j = 0; j < dims; ++j) {
			outputFile << 0.0;
			outputFile << ",";
		}
		outputFile << std::endl; // Move to the next row
					 // Iterate over each row and write to the CSV file
		for (int i = 0; i < jq->size(); ++i) {
			float* jp_vec = jq->at(i).getKPIVec();
			for (int j = 0; j < dims; ++j) {
				outputFile << jp_vec[j];
				// Add comma if not the last element in the row
				//			if (j < dims - 1)
				outputFile << ",";
			}
			outputFile << std::endl; // Move to the next row
		}

		outputFile.close();

		int LIMIT = std::floor(jq->size()*200/lowerBound);
		const int numThreads = THREADS;
		std::vector<std::thread> threads;
		int iteration[numThreads]={0};
		// Function to be executed by each thread
		auto threadFunction = [&](int threadId) {
			while(iteration[threadId] < LIMIT){
				iteration[threadId]++;
				// Code to clear _mq, create a copy of jq, and run LS
				_mq[threadId]->clear();
				_mq[threadId]->shrink_to_fit();

				for (unsigned i = 0; i < jq->size(); i++) {
					_jq[threadId]->push_back(jq->at(i));
				}
				if(1){
					if(iteration[threadId] < 9){
						int switcher = iteration[threadId]-1;
						// Run LS with the chosen parameters based on switcher
						switch (switcher) {
							case 0:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::L2, LS::I1, LS::T3); break;
							case 1:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::LINF, LS::I1, LS::T3); break;
							case 2:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::L1, LS::I1, LS::T3); break;
							case 3:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::L2, LS::I1, LS::T2); break;
							case 4:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::LINF, LS::I1, LS::T2); break;
							case 5:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::L1, LS::I1, LS::T2); break;
							case 6:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::L2, LS::I1, LS::T1); break;
							case 7:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::LINF, LS::I1, LS::T1); break;
							case 8:  LS::runLS(_mq[threadId], _jq[threadId], LS::DET, LS::L1, LS::I1, LS::T1); break;

						}
					}else{
						int switcher = rand() % 9;
						// Run LS with the chosen parameters based on switcher
						switch (switcher) {
							case 0:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::L2, LS::I1, LS::T3); break;
							case 1:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::LINF, LS::I1, LS::T3); break;
							case 2:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::L1, LS::I1, LS::T3); break;
							case 3:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::L2, LS::I1, LS::T2); break;
							case 4:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::LINF, LS::I1, LS::T2); break;
							case 5:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::L1, LS::I1, LS::T2); break;
							case 6:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::L2, LS::I1, LS::T1); break;
							case 7:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::LINF, LS::I1, LS::T1); break;
							case 8:  LS::runLS(_mq[threadId], _jq[threadId], LS::RAND, LS::L1, LS::I1, LS::T1); break;
						}
					}
				}
				//save patterns
				std::vector<std::vector<double>> localPatterns;
				std::vector<double> aPattern;
				localPatterns.clear();
				localPatterns.shrink_to_fit();
				if(_mq[threadId]->size() < miniMQsize)
					miniMQsize = _mq[threadId]->size();
		if(miniMQsize == lowerBound){
			printf("\n\n\n LB reached by LSI1 inside first loop\n");
			break;
		}
				for (unsigned i = 0; i < _mq[threadId]->size(); i++) {
					aPattern.clear();
					aPattern.shrink_to_fit();
					for (unsigned j = 0; j < _mq[threadId]->at(i).assignedJobs->size(); j++) {
						aPattern.push_back(_mq[threadId]->at(i).assignedJobs->at(j).getID());
					}
					tmp_mv = _mq[threadId]->at(i).getRemainingCapacity();
					aPattern.push_back(MathFunctions::costFunction(tmp_mv, 1, dims));
					if (MathFunctions::isVectorDifferencePositive(load, tmp_mv, dims)) {
						localPatterns.push_back(aPattern);
					}
				}
				std::lock_guard<std::mutex> lock(patternsMutex);
				patterns.insert(patterns.end(), localPatterns.begin(), localPatterns.end());
			}
		};

		// Create and start threads
		for (int i = 0; i < numThreads; ++i) {
			threads.emplace_back(threadFunction, i);
		}

		// Wait for all threads to finish
		for (auto& thread : threads) {
			thread.join();
		}
		if(miniMQsize == lowerBound){
			printf("\n\n\n LB reached by LSI1\n");
			break;
		}

		// the loop creates upto LIMIT of LS solutions, patterns are extracted from the LS solutions
		int numDistinctionPatterns;
		// Output file path
		std::string outputFilePath = "output.csv";
		std::string outputFilePathReducedCostCheck = "outputNew.csv";
		printf("Finished running ,load pattern to file\n");
		// Iterate over patterns and write each patternRow to the CSV file
		for (unsigned i = 0; i < patterns.size(); i++) {
			std::vector<int> patternRow(_jqSize, 0);

			for (unsigned j = 0; j < patterns[i].size() - 1; j++) {
				patternRow[static_cast<int>(patterns[i][j]) - 1] = 1;
			}
			// Write the patternRow to the CSV file
			writeCSV(outputFilePath, patternRow);
			writeCSV(outputFilePathReducedCostCheck, patternRow);
		}
		// remove repeated pattern and check reduced cost
		int noNRC = system("./checkReducedCost.sh");
		std::string inputFilePath_ = "reducedCostResult.csv";
		std::vector<float> rowData;
		rowData.clear();
		std::ifstream inputFile_(inputFilePath_);
		if (!inputFile_.is_open()) {
			std::cerr << "Error: Unable to open file " << inputFilePath_ << " for reading." << std::endl;
		}else{
			std::string line;
			while (std::getline(inputFile_, line)) {
				std::istringstream lineStream(line);
				std::string cell;
				while (std::getline(lineStream, cell, ',')){
					double value = std::stod(cell); // Convert cell string to double
					rowData.push_back(value);
				}
			}
		}

		inputFile_.close();

		if(rowData[0]){
			std::string filename = "LB.csv";
			std::ofstream outputFile(filename, std::ios_base::app);
			if (!outputFile.is_open()) {
				std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
				return 0;
			}

			outputFile<<rowData[1] << ",";
			outputFile << std::endl; // Move to the next row

			outputFile.close();
       			std::cout<<"Total unique patterns are "<<patterns.size()<<'\n';
			break;
		}
	}

#ifdef DEBUG
	for (const auto& sublist : patterns) {
		for (const auto& element : sublist) {
			std::cout << element << ' ';
		}
		std::cout << '\n';
	}
#endif

#ifdef PRINTMACHINE
	for(unsigned i =0 ; i< _mq->size(); i++){
		_mq->at(i).printMachine();
		printf("\n");
	}
#endif
	// Record the end time
	auto end = std::chrono::high_resolution_clock::now();

	// Calculate the duration in microseconds
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	// Print the runtime
	std::cout << "Runtime: " << duration.count() << " microseconds" << std::endl;
	std::cout << "mqsize is  " << miniMQsize << " LB is"<<lowerBound << std::endl;
			std::string filename = "UB.csv";
			std::ofstream outputFile(filename, std::ios_base::app);
			if (!outputFile.is_open()) {
				std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
				return 0;
			}

			outputFile<<miniMQsize;
			outputFile << std::endl; // Move to the next row

			outputFile.close();

	if(miniMQsize != lowerBound){

		// start running CPLEX 
		int result = system("./run_scripts.sh");

		// Check the result of the system command
		if (result == 0) {
			// The script ran successfully
			std::cout << "Script executed successfully." << std::endl;

			// Load data from bestKnownResults.csv
			std::ifstream inFile("bestKnownResults.csv");
			if (inFile.is_open()) {
				std::string header, value;
				std::getline(inFile, header); // Read and ignore the header

				if (std::getline(inFile, value)) {
					// Process the loaded value as needed
					double loadedValue = std::stod(value);
					std::cout << "Loaded value from bestKnownResults.csv: " << loadedValue << std::endl;
					optimalSolution = loadedValue;
					// Add your operations with the loaded value here

					// Close the file
					inFile.close();

					// Delete bestKnownResults.csv
					int removeResult = std::remove("bestKnownResults.csv");
					if (removeResult == 0) {
						std::cout << "bestKnownResults.csv deleted successfully." << std::endl;
					} else {
						std::cerr << "Error: Unable to delete bestKnownResults.csv." << std::endl;
					}
				} else {
					std::cerr << "Error: Unable to read value from bestKnownResults.csv." << std::endl;
				}
			} else {
				std::cerr << "Error: Unable to open bestKnownResults.csv for reading." << std::endl;
			}
		} else {
			// An error occurred
			std::cerr << "Error: Script execution failed." << std::endl;
		}


		std::vector<Machine>* mqOptimal = mach_gen.generateMachines(optimalSolution, 1.0);
		*out_machines = mqOptimal;
		if(mqOptimal->size()>mq->size()){
			counter++;
			return 0;
		}
		return 1;
	}
	else{
		std::vector<Machine>* mqOptimal = mach_gen.generateMachines(lowerBound, 1.0);
		*out_machines = mqOptimal;
		if(mqOptimal->size()>mq->size()){
			counter++;
			return 0;
		}
		return 1;

	}
}


