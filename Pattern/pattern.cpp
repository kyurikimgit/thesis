#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
ILOSTLBEGIN

#include <sstream>

int getColumnCount(const std::string& filePath, char delimiter = ',') {
	// Open the CSV file
	std::ifstream inputFile(filePath);
	if (!inputFile.is_open()) {
		std::cerr << "Error opening the file." << std::endl;
		return -1; // Return -1 to indicate an error
	}

	// Read the first line of the CSV file
	std::string firstLine;
	if (std::getline(inputFile, firstLine)) {
		// Create a string stream from the first line
		std::istringstream lineStream(firstLine);

		// Tokenize the first line by the delimiter
		std::string cell;
		std::vector<std::string> tokens;
		while (std::getline(lineStream, cell, delimiter)) {
			tokens.push_back(cell);
		}

		// Close the file
		inputFile.close();

		// Return the number of columns (size of tokens vector)
		return static_cast<int>(tokens.size());
	} else {
		// Failed to read the first line
		std::cerr << "Error reading the first line of the file." << std::endl;
		// Close the file
		inputFile.close();
		return -1; // Return -1 to indicate an error
	}
}

int main() {
	IloEnv env;

	try {
		IloModel model(env);
		IloCplex cplex(model);
		const double t = 1; // Threshold
				    // Open CSV file and determine its dimensions
		std::string filePath = "output.csv";
		//std::string filePath = "patterns.csv";
		std::ifstream inputFile(filePath); // Replace "your_file.csv" with your actual file name
		if (!inputFile.is_open()) {
			std::cerr << "Error opening the file." << std::endl;
			return 1;
		}
		
		int numVectors = 0;
		int n = getColumnCount(filePath);
		printf("column is %d\n",n);

		std::string line;
		while (std::getline(inputFile, line)) {
			++numVectors;
			std::istringstream stream(line);
		}
		// Rewind to the beginning of the file
		inputFile.clear();
		inputFile.seekg(0, std::ios::beg);
		// Dynamically allocate memory for S_data based on file dimensions
		double** S_data = new double*[numVectors];
		for (int i = 0; i < numVectors; ++i) {
			S_data[i] = new double[n];
		}


		// Read data from the CSV file
		for (int i = 0; i < numVectors; ++i) {
			std::string line;
			if (std::getline(inputFile, line)) {
				std::istringstream lineStream(line);

				for (int j = 0; j < n; ++j) {
					char comma; // to read and discard the comma
					if (!(lineStream >> S_data[i][j] >> comma)) {
						std::cerr << "111 Read load data to S_data Error reading data from the file."<<"i is "<<i<<"j is "<<j << std::endl;
						inputFile.close();

						// Deallocate memory before returning
						for (int k = 0; k < numVectors; ++k) {
							delete[] S_data[k];
						}
						delete[] S_data;

						return 1;
					}
				}
			} else {
				std::cerr << "Error reading a line from the file." << std::endl;
				inputFile.close();

				// Deallocate memory before returning
				for (int k = 0; k < numVectors; ++k) {
					delete[] S_data[k];
				}
				delete[] S_data;

				return 1;
			}
		}
		for(unsigned i = 0; i < numVectors; i++){
			if(S_data[i][6] == 1){
				cout <<"111111111---------"<< i <<endl;
				break;
			}
			
		}
		// Close the file
		inputFile.close();

		// Variables
		IloArray<IloBoolVar> x(env, numVectors);
		for (int i = 0; i < numVectors; ++i) {
			x[i] = IloBoolVar(env);
		}

		// Objective: minimize the sum of selected vectors
		IloObjective objective = IloMinimize(env);
		for (int i = 0; i < numVectors; ++i) {
			objective.setLinearCoef(x[i], 1.0);
		}

 		model.add(objective);
		// Constraints: sum of selected vectors must be greater than t in each component
		for (int j = 0; j < n; ++j) {
			IloExpr constraintExpr(env);
			for (int i = 0; i < numVectors; ++i) {
				constraintExpr += x[i] * S_data[i][j];
			}
			model.add(constraintExpr >=t);
			constraintExpr.end();
		}
		// Set upper bound
		    // Open the CSV file
    std::ifstream file("UB.csv");
    
    // Check if the file is opened successfully
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }
    
    // Read the numeric value from each line and display it
    double upperBound;
    while (file >> upperBound) {
        std::cout << "Value: " << upperBound << std::endl;
    }
		cplex.setParam(IloCplex::Param::MIP::Tolerances::UpperCutoff, upperBound);
		double timeLimit = 3600.0;
		cplex.setParam(IloCplex::Param::MIP::Strategy::VariableSelect, 3);
		cplex.setParam(IloCplex::Param::TimeLimit, timeLimit);
		// Solve the ILP
		cplex.setParam(IloCplex::Param::Threads, 1);
		double start = cplex.getCplexTime();
		cplex.solve();
		double runtime = cplex.getCplexTime() - start;

		        std::string output_file = "LSPruntime.csv";
        std::ofstream f(output_file, std::ios_base::app);
        if (!f.is_open())
        {
            std::string s("Cannot write solution to file " + output_file);
            throw std::runtime_error(s);
        }
        f <<runtime<<","<<"\n";
        f.close();

		int bin = 0;
		int optimalSolution = 0;
		// Output the result
		if (cplex.getStatus() == IloAlgorithm::Optimal) {
            		std::ofstream outPatternFile("optimalPatterns.csv");
			cout << "Optimal Solution Found:" <<cplex.getObjValue()<< endl;
			for (int i = 0; i < numVectors; ++i) {
				if (cplex.getValue(x[i]) > 0.5) {
					if (outPatternFile.is_open()) {
						outPatternFile << i+1<<"\n";
					}
					bin++;
					//cout<< "Bin " << bin <<" is: "<<i+1<<endl;						}
			}
			}
			optimalSolution = bin;
			outPatternFile.close();
                }else{
                	std::cout << "Time limit reached. Best known solution: " << cplex.getObjValue() << std::endl;
			 optimalSolution = cplex.getObjValue();
		}
		 // Save the optimal solution to a CSV file
            std::ofstream outFile("bestKnownResults.csv");
            if (outFile.is_open()) {
                outFile << "OptimalSolution\n";
                outFile << optimalSolution << "\n";
                outFile.close();
                std::cout << "Optimal solution saved to bestKnownResults.csv" << std::endl;
            } else {
                std::cerr << "Error: Unable to open bestKnownResults.csv for writing." << std::endl;
            }
	} catch (IloException& ex) {
		cerr << "Error: " << ex << endl;
	}

	env.end();
	return 0;
}

