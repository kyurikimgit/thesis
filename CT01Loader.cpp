#include <stdlib.h>
//
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <limits>
#include <string>
#include "Job.h"
#include "Environment.h"
#define LARGE 1
#define CT01
class TestVectorCreator {
	public:

		TestVectorCreator() {
		}
		std::vector<std::vector<float>*>* createVectors(int vbpClass, int vbpSize, int vbpDimension, int vbpInstance) {
			std::vector<std::vector<float>*>* queue = new std::vector<std::vector<float>*>();
			printf("[DEBUG] createVectors() called with vbpDimension: %d\n", vbpDimension);
			printf("vbpClass %d,Size %d,Dimension %d,Instance %d",vbpClass,vbpSize,vbpDimension,vbpInstance);
			int amount = 0;
			int id = 0;
			int line = 0;
			int index = 0;
			// store values read from line
			float instances[vbpSize][vbpDimension+1];
			for(unsigned i = 0; i < vbpSize; i++){
				for(unsigned j = 0; j < vbpDimension+1; j++){
					instances[i][j] = 0.;
				}
			}
			double capacity[vbpDimension];
			char filename_[1024];
			FILE * fp;
			char buffer_[100];
                        char tmp[100];

//Locate path of file
#ifdef CT01
			if(vbpClass == 10){
				if(vbpInstance == 10){
                                        sprintf(filename_,"/home/kyurikim/vectorbinpacking/Benchmarks/CT01/CL_10_%d_10.vbp",vbpSize);
                                }
                                else
                                        sprintf(filename_,"/home/kyurikim/vectorbinpacking/Benchmarks/CT01/CL_10_%d_%d.vbp",vbpSize,vbpInstance);
			}
			else{
				if(vbpInstance == 10){
					sprintf(filename_,"/home/kyurikim/vectorbinpacking/Benchmarks/CT01/CL_%d_%d_10.vbp",vbpClass,vbpSize);
				}
				else
					sprintf(filename_,"/home/kyurikim/vectorbinpacking/Benchmarks/CT01/CL_%d_%d_%d.vbp",vbpClass,vbpSize,vbpInstance);
			}
#endif

			fp = fopen(filename_, "r");	
			if (fp == NULL){
				printf("no file\n");
				exit(EXIT_FAILURE);
			}
//Interpret VBP file into jobs
//Will the buffer be cleared by default or not?
			while ((fgets(buffer_, 100, fp)) != NULL) {
				int j = 0;
				line++;
				memset(tmp,0,strlen(tmp));
				if(line == 1){
					continue;
				}
				else if(line == 2){
					for(unsigned i = 0; i < strlen(buffer_); i++){
						if(buffer_[i] != ' ' && buffer_[i] != '\n'){
							strncat(tmp, buffer_+i,1);
						}
                                        	else{
                                                	capacity[j] = float(atoi(tmp));
                                                	memset(tmp,0,strlen(tmp));
                                                	j++;
                                        	}
                                	}
					printf("capacity is %lf, %lf\n ",capacity[0],capacity[1]);	
					continue;
				}
				else if(line == 3){
					continue;
				}
				else {
					j = 0;  // 각 줄 시작 시 j를 0으로 초기화
					for(unsigned i = 0; i < strlen(buffer_); i++){
						if(buffer_[i] != ' ' && buffer_[i] != '\n'){
							strncat(tmp, buffer_+i, 1);
						}
						else{
							if(j <= vbpDimension){  
								// j==0 ~ vbpDimension-1: weight, j==vbpDimension: demand
								instances[index][j] = float(atoi(tmp));
							}
							memset(tmp, 0, sizeof(tmp));
							j++;
						}
					}
					index++;  // 항상 다음 아이템으로 이동
					memset(buffer_, 0, sizeof(buffer_));
				}

				// else{
				// 	for(unsigned i = 0; i < strlen(buffer_); i++){
				// 		if(buffer_[i] != ' ' && buffer_[i] != '\n'){
				// 			strncat(tmp, buffer_+i, 1);
				// 		}
				// 		else{
				// 			if(j <= vbpDimension){  
				// 				// vbpDimension 값까지는 각 차원의 weight, 
				// 				// j==vbpDimension는 demand 값이므로 모두 읽어들임
				// 				instances[index][j] = float(atoi(tmp));
				// 			}
				// 			memset(tmp, 0, sizeof(tmp));
				// 			j++;
				// 		}
				// 	}
				// 	// index++;
				// 	if (j > vbpDimension) {  
    			// 		index++;  // demand까지 읽고 나면 다음 아이템으로 이동
				// 	}
				// 	memset(buffer_, 0, sizeof(buffer_));
				// }

				// else{
				// 	for(unsigned i = 0; i < strlen(buffer_); i++){
				// 		if(buffer_[i] != ' ' && buffer_[i] != '\n'){
				// 			strncat(tmp, buffer_+i,1);
				// 		}
				// 		else{
				// 			instances[index][j] += float(atoi(tmp));
				// 			memset(tmp,0,strlen(tmp));
				// 			j++;
				// 		}
				// 	}
				// 	index++;
				// 	memset(buffer_,0,strlen(buffer_));
				// }
			}
			fclose(fp);
			for(unsigned i = 0; i < vbpSize; i++){
				amount += instances[i][vbpDimension];
			}
			double arr[amount][vbpDimension];
                        for(unsigned i = 0; i < amount; i++){
				for(unsigned j = 0; j < vbpDimension; j++){
                           	     arr[i][j] = 0;
				}
                        }
			printf("amount of job is %d\n",amount);
			//condition for large set of instances
			if(LARGE == 1){


				for(unsigned i = 0; i < vbpSize; i++){
   					 for(unsigned j = 0; j < instances[i][vbpDimension]; j++){
        				for(unsigned k = 0; k < vbpDimension; k++){
            				arr[id][k] = instances[i][k];  // (Changed) 원본 값 그대로 사용
       					}
        						id++;
    				}
							}


				// for(unsigned i = 0; i < vbpSize; i++){
				// 	for(unsigned j = 0; j < instances[i][vbpDimension]; j++){
				// 		for(unsigned k = 0; k < vbpDimension; k++){
                //                 		arr[id][k] = float((instances[i][k]-0.01)/capacity[k]);
				// 		}
                //                        		id++;
				// 	}
                //         	}
			}
			//condition for normal set of instances
			else{
				
				 for(unsigned i = 0; i < vbpSize; i++){
					for(unsigned k = 0; k < vbpDimension; k++){
                                		arr[id][k] = instances[i][k];
					}
					id++;
                                }
                        }
			float total[vbpDimension];
			for(unsigned i = 0; i < vbpDimension; i++){
				total[i] = 0;
			}
			 for(int i = 0; i < id; i++) {
//				printf("Item %d is %f, %f\n",id,arr[i][0],arr[i][1]);
				for(unsigned j = 0; j < vbpDimension; j++){
					total[j] += arr[i][j];
				}
			// Create job vector and push it into queue
				std::vector<float>* vec = new std::vector<float>();
				 for(int j = 0; j < vbpDimension; j++) {
					 vec->push_back(arr[i][j]);
				 }
				 queue->push_back(vec);
			 }
			
			printf("%d jobs have capacity %lf, %lf job 19 is %lf %lf \n",id,total[0],total[1],arr[18][0],arr[18][1]);
				 return queue;

	
			}
};

extern "C"
std::vector <Job>* loadJobs(const char* _path, JobMetaData* _jobdata, Environment* _env, int _runs) {
	std::vector <Job>* jobVec = new std::vector <Job>();
	std::random_device device;
	std::mt19937 generator(device());

	std::uniform_int_distribution<int> distBurst(_jobdata->lowerBurst, _jobdata->upperBurst);
	std::uniform_int_distribution<int> distArrive(0, _jobdata->duration);

	TestVectorCreator creator = TestVectorCreator();
//The function to create vectors that takes environmenal variables as inputs
	std::vector<std::vector<float>*>* vectors = creator.createVectors(_env->vbpClass, _env->vbpSize, _env->vbpDimension, _env->vbpInstance);
	// use vectors to generate jobs
	for(unsigned i = 0; i < vectors->size(); i++) {
		jobVec->push_back(Job(i + 1, vectors->at(i), distBurst(generator), distArrive(generator)));
	}
	for(unsigned i = 0; i < vectors->size(); i++) {
		delete vectors->at(i);
	}
	delete vectors;

	return jobVec;
}

