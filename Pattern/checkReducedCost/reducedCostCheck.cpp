#include <ilcplex/ilocplex.h>
#include <iostream>

#include <fstream>
#include <vector>
#include <string>
ILOSTLBEGIN

#include <sstream>
// #columns = #items
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

bool loadPattern(const std::string& filePath, std::vector<std::vector<int>>* data){
	std::ifstream inputFile(filePath); // Replace "your_file.csv" with your actual file name
	if (!inputFile.is_open()) {
		std::cerr << "Error opening the file." << std::endl;
		return 1;
	}
	int numVectors = 0;
	int n = getColumnCount(filePath);
	std::string line;
	//get #rows
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
	// Close the file
	inputFile.close();
	// Push the data from S_data to data
	for (int i = 0; i < numVectors; ++i) {
		std::vector<int> row;
		for (int j = 0; j < n; ++j) {
			row.push_back(S_data[i][j]);
		}
		data->push_back(row);
	}
	printf("data size is %ld\n",data->size());
	// Free the memory allocated for S_data
	for (int i = 0; i < numVectors; ++i) {
		delete[] S_data[i];
	}
	delete[] S_data;
	return 1;
}

double continuousKnapsack(double* duals, const std::string& filePath){
	// load items from vbp file for the constraints
	IloEnv env;
	double obj;
	double obj_opt;
	try {
		IloModel model_KS(env);
		IloCplex cplex_KS(model_KS);

		// Load data from the CSV file
		std::ifstream inputFile(filePath);

		if (!inputFile.is_open()) {
			std::cerr << "!!!Error opening the file." << std::endl;
		}

		int numVectors_KS = 0;
		//get the numer of dimensions
		int n = getColumnCount(filePath);
		printf("# dims is %d\n",n);
		std::vector<std::vector<double>> item;

		std::string line;
		bool firstLine = true;
		double gap=0;
		while (std::getline(inputFile, line)) {
			// the first line collect the gap to terminate;
			if(firstLine){
				firstLine=false;
				std::istringstream lineStream(line);
				std::string cell;
				while (std::getline(lineStream, cell, ',')){
					gap = std::stod(cell); // Convert cell string to double

				}
			}
			else{
				++numVectors_KS;
				std::istringstream lineStream(line);
				std::vector<double> rowData;
				std::string cell;
				//			std::cout<< "Cell "<<numVectors_KS<<std::endl;
				while (std::getline(lineStream, cell, ',')){
					double value = std::stod(cell); // Convert cell string to double
					rowData.push_back(value);
					//			std::cout<< "is " << value<<std::endl; 	
				}
				item.push_back(rowData);
			}
		}

		// Close the file
		inputFile.close();
		// Variables
		IloNumVarArray x_KS(env,numVectors_KS,0,1,ILOFLOAT);
		IloExpr objective_KS(env);
		// Objective: minimize the sum of selected vectors
		for (int i = 0; i < numVectors_KS; ++i) {
			objective_KS += duals[i] * x_KS[i];
		}
        // Add a constraint enforcing the objective to be greater than 1
       // 	model_KS.add(objective_KS >= 1);
		
		model_KS.add(IloMaximize(env,objective_KS));

		// #constraints = #dims
		for(unsigned c = 0; c < n; c++){
			IloExpr constraints_KS(env);
			for (int i = 0; i < numVectors_KS; ++i) {
				constraints_KS += x_KS[i] * item[i][c];
			}
			model_KS.add(constraints_KS <= 1);
		}

		// Solve the ILP
		cplex_KS.setParam(IloCplex::Param::Threads, 1);
		cplex_KS.setParam(IloCplex::Param::MIP::Strategy::VariableSelect, 3);
		//double timeLimit = 60.0;
                //cplex_KS.setParam(IloCplex::Param::TimeLimit, timeLimit);
//		printf("\n\n\n Gap is %lf\n",gap);
//		 cplex_KS.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, gap);
 		//cplex_KS.setParam(IloCplex::Param::Barrier::Limits::ObjRange, 1.00001); // Set the tolerance as needed
			


		if(!cplex_KS.solve()){
			env.error() << "Failed to optimize the Master Problem"<<endl;
			throw(-1);
		}
		// output the solution of KS
		std::cout<<std::endl;
		obj = cplex_KS.getObjValue();
		std::cout<<"Obj is :"<<" "<<obj<<std::endl;

	} catch (IloException& ex) {
		std::cerr << "Error: " << ex << std::endl;
	}
	env.end();
	obj = obj-1.00001;
	return obj;



	// 


}
// duals are the values of the items
// newPatern is the solution of the pricing problem with the most reduced cost
double* knapsack(double* duals,int* newPattern, const std::string& filePath, double gap){
	// load items from vbp file for the constraints
	IloEnv env;
	double obj;
	double obj_opt;
	try {
		IloModel model_KS(env);
		IloCplex cplex_KS(model_KS);

		// Load data from the CSV file
		std::ifstream inputFile(filePath);

		if (!inputFile.is_open()) {
			std::cerr << "!!!Error opening the file." << std::endl;
		}

		int numVectors_KS = 0;
		//get the numer of dimensions
		int n = getColumnCount(filePath);
		printf("# dims is %d\n",n);
		std::vector<std::vector<double>> item;

		std::string line;
		bool firstLine = true;
		while (std::getline(inputFile, line)) {
			// the first line collect the gap to terminate;
			if(firstLine){
				firstLine=false;
			}
			else{
				++numVectors_KS;
				std::istringstream lineStream(line);
				std::vector<double> rowData;
				std::string cell;
				//			std::cout<< "Cell "<<numVectors_KS<<std::endl;
				while (std::getline(lineStream, cell, ',')){
					double value = std::stod(cell); // Convert cell string to double
					rowData.push_back(value);
					//			std::cout<< "is " << value<<std::endl; 	
				}
				item.push_back(rowData);
			}
		}

		// Close the file
		inputFile.close();
		// Variables
		IloNumVarArray x_KS(env,numVectors_KS,0,1,ILOINT);
		IloExpr objective_KS(env);
		// Objective: minimize the sum of selected vectors
		for (int i = 0; i < numVectors_KS; ++i) {
			objective_KS += duals[i] * x_KS[i];
		}
        // Add a constraint enforcing the objective to be greater than 1
       // 	model_KS.add(objective_KS >= 1);
		
		model_KS.add(IloMinimize(env,objective_KS));

		// #constraints = #dims
		for(unsigned c = 0; c < n; c++){
			IloExpr constraints_KS(env);
			for (int i = 0; i < numVectors_KS; ++i) {
				constraints_KS += x_KS[i] * item[i][c];
			}
			model_KS.add(constraints_KS <= 1);
		}
        	model_KS.add(objective_KS >= 1.00001);

		// Solve the ILP
		cplex_KS.setParam(IloCplex::Param::Threads, 1);
		cplex_KS.setParam(IloCplex::Param::MIP::Strategy::VariableSelect, 3);
		double timeLimit = 600.0;
                //cplex_KS.setParam(IloCplex::Param::TimeLimit, timeLimit);
		printf("\n\n\n Gap is %lf\n",gap);
		 cplex_KS.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, gap);
 		//cplex_KS.setParam(IloCplex::Param::Barrier::Limits::ObjRange, 1.00001); // Set the tolerance as needed
			


		if(!cplex_KS.solve()){
			env.error() << "Failed to optimize the Master Problem"<<endl;
			double* obj_vec = new double[2];
			obj_vec[0] = 1.0;
			obj_vec[1] = 1.0;
	return obj_vec;
	//		throw(-1);
		}
		for(unsigned i = 0; i < numVectors_KS; i++){
                        newPattern[i] = cplex_KS.getValue(x_KS[i]);
                        if(newPattern[i] == 1)
                                std::cout<< i+1 <<" ";
                }
		// output the solution of KS
		std::cout<<std::endl;
		obj = cplex_KS.getObjValue();
		std::cout<<"Obj is :"<<" "<<obj<<std::endl;
		obj_opt = cplex_KS.getBestObjValue();
		std::cout<<"Best Obj is :"<<" "<<obj_opt<<std::endl;

	} catch (IloException& ex) {
		std::cerr << "Error: " << ex << std::endl;
	}
	double* obj_vec = new double[2];
	obj_vec[0] = obj;
	obj_vec[1] = obj_opt;
	env.end();
	return obj_vec;



	// 


}

int main() {
	IloEnv env;

	try {
		const double t = 1; // Threshold
				    // Open CSV file and determine its dimensions
				    // Variables
		int numVectors = 0;
		std::string filePath = "outputNew.csv";
		int n = getColumnCount(filePath);
		std::vector<std::vector<int>> S_data;
		loadPattern(filePath, &S_data);
		IloModel model(env);
		IloCplex cplex(model);
		IloArray<IloNumVar> x(env, S_data.size());
		numVectors = S_data.size();
		//		IloArray<IloBoolVar> x(env, numVectors);
		for (int i = 0; i < numVectors; ++i) {
			x[i] = IloNumVar(env);
			//x[i] = IloBoolVar(env);
		}
		// Objective: minimize the sum of selected vectors
		IloObjective objective = IloMinimize(env);
		for (int i = 0; i < numVectors; ++i) {
			objective.setLinearCoef(x[i], 1.0);
		}
		model.add(objective);
		// Constraints: sum of selected vectors must be greater than t in each component
		IloRangeArray constraints(env);
		for (int j = 0; j < n; ++j) {
			IloExpr constraintExpr(env);
			for (int i = 0; i < numVectors; ++i) {
				constraintExpr += x[i] * S_data[i][j];
			}
			// Create an IloConstraint object and add it to the model
			constraints.add(constraintExpr == t);
			// Clear the IloExpr object
			constraintExpr.end();
		}
		model.add(constraints);
		// Solve the ILP
		cplex.setParam(IloCplex::Param::Threads, 1);
		printf("\n Start solving LP, collect duals\n");
		cplex.solve();
		IloNumArray duals(env);
		double* Duals = new double[n];
		for(unsigned i = 0; i < n; i++){
			Duals[i] = cplex.getDual(constraints[i]);
//			std::cout<<"Dual "<<i<<" is "<<Duals[i]<<std::endl;
		}

		std::cout<<"Before remove columnns without contribution, the size of columns is "<<numVectors<<std::endl;
		//remove columns with no contribution
		if(numVectors>42){ 
			for(int i = numVectors-1; i >= 0; i--){
				double contribution = 0;
				for(int j = 0; j < n; j++){
					contribution += Duals[j] * S_data[i][j];	
				}
				if(i<10){
					printf("Contribution is %lf\n",contribution);
				}
				if(contribution < 0.997)
					S_data.erase(S_data.begin()+i);
			}
		}
	
		std::cout<<"Removed "<<numVectors-S_data.size()<<" columns"<<std::endl;


		// Solve the Knapsack problem
		double* objKS;
		double gap = 1;
		int newPattern[n];
		for(unsigned i = 0; i < n; i++)
			newPattern[i] = 0;
		std::string filePath_ = "items.csv";
		printf("\n Start solving Pricing problem, determine the existance of the negative reduced cost\n");
		//gap = continuousKnapsack(Duals, filePath_);
		objKS = knapsack(Duals, newPattern,filePath_,gap);
		float sumOfDuals = 0;
		for(unsigned i = 0; i < n; i++)
			sumOfDuals += Duals[i];
		std::cout<<std::endl;
		std::cout<< "KS problem's objective is "<<objKS[0]<<" sumOfDuals is "<<sumOfDuals<<std::endl;
		std::vector<int> tempVector;
    		for (int i = 0; i < n; ++i) {
       		 tempVector.push_back(newPattern[i]);
    		}
		S_data.push_back(tempVector);

               // std::ofstream outFile("output.csv");
		//write data back to outputNew
                std::ofstream outFile("outputNew.csv");
                        // Check if the file is open
                        if (outFile.is_open()) {
                                // Write each element of 'newPattern' followed by a comma
				//!!! Only write the new one
				for(unsigned j = 0; j < S_data.size(); j++){
                                	for (int i = 0; i < n; ++i) {
                                        	outFile << S_data[j][i];
                                        	// Add a comma after each element, except the last one
                                        	if (i < n) {
                                                	outFile << ",";
                                        	}
                                	}

                                // End the line with a newline character
                               		outFile << "\n";
				}
                                // Close the file
                                outFile.close();
                        } else {
                                // If the file couldn't be opened, print an error message
                                std::cerr << "Error: Unable to open output.csv for writing." << std::endl;
                        }



		if((1-objKS[0])>=-0.00001){
			printf("reduced cost is %lf\n",1-objKS[0]+0.00001);
			//Output to Hybrid.csv
			std::string output_file = "reducedCostResult.csv";
			std::ofstream f(output_file);
			if (!f.is_open())
			{
				std::string s("Cannot write solution to file " + output_file);
				throw std::runtime_error(s);
			}
			f << 1 << ","<<sumOfDuals<<"\n";
			f.close();
			printf("There is no negative reduced cost pattern, therefore, OPT\n");
		}
		else{
			printf("reduced cost is %lf\n",1-objKS[0]);
			std::string output_file = "reducedCostResult.csv";
			std::ofstream f(output_file);
			if (!f.is_open())
			{
				std::string s("Cannot write solution to file " + output_file);
				throw std::runtime_error(s);
			}
			f << 0 << ","<<objKS[1]<<"\n";
			f.close();

		}
	} catch (IloException& ex) {
		cerr << "Error: " << ex << endl;
	}

	env.end();
	return 0;
}












