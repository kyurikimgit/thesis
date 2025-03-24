// #include <vector>
// #include <string>
// #include <iostream>
// #include "gurobi_c++.h"    // Gurobi C++ API header
// #include "Job.h"
// #include "Machine.h"
// #include "Environment.h"
// #include "GurobiBinPacking.h"

// // "대형아이템 최대 2개" + "소형아이템은 용량(binCapacity) 이하" Bin Packing MIP
// int solveWithGurobi(const std::vector<Job> &jobs,
//                     int dims,
//                     int numBins,       // bin 개수 (env->machines 등)
//                     float binCapacity, // 예: 1000.0
//                     std::vector<Machine> &outMachines)
// {
//   try {
//     // 1) Gurobi 초기화
//     GRBEnv env = GRBEnv(true);
//     env.set("LogFile", "gurobi.log");
//     env.start();

//     GRBModel model(env);
//     model.set(GRB_IntParam_OutputFlag, 1); // 콘솔 로그 활성화

//     int n = (int)jobs.size();

//     // 2) 대형 아이템 인덱스 수집
//     std::vector<int> largeSet; // large item indices
//     for(int i=0; i<n; i++){
//       bool isLarge = false;
//       for(int d=0; d<dims; d++){
//         if(jobs[i].getKPIVec()[d] > binCapacity / 3.0f) {
//           isLarge = true;
//           break;
//         }
//       }
//       if(isLarge) largeSet.push_back(i);
//     }

//     // 3) 이진 변수 x[i][b]: 아이템 i가 bin b에 들어가면 1
//     std::vector<std::vector<GRBVar>> x(n, std::vector<GRBVar>(numBins));
//     for(int i=0; i<n; i++){
//       for(int b=0; b<numBins; b++){
//         std::string varName = "x_"+std::to_string(i)+"_"+std::to_string(b);
//         x[i][b] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, varName);
//       }
//     }

//     // 4) 제약: 각 아이템은 정확히 1개의 bin
//     for(int i=0; i<n; i++){
//       GRBLinExpr expr = 0.0;
//       for(int b=0; b<numBins; b++){
//         expr += x[i][b];
//       }
//       model.addConstr(expr == 1.0, "assign_item_"+std::to_string(i));
//     }

//     // 5) 차원별 용량 제약: sum( item i의 x[i][b] * KPIVec[d] ) ≤ binCapacity
//     for(int b=0; b<numBins; b++){
//       for(int d=0; d<dims; d++){
//         GRBLinExpr sumDim = 0.0;
//         for(int i=0; i<n; i++){
//           sumDim += x[i][b] * jobs[i].getKPIVec()[d];
//         }
//         model.addConstr(sumDim <= binCapacity,
//                         "cap_bin"+std::to_string(b)+"_d"+std::to_string(d));
//       }
//     }

//     // 6) 대형 아이템 최대 2개 제약
//     //    sum_{i in largeSet} x[i][b] ≤ 2
//     for(int b=0; b<numBins; b++){
//       GRBLinExpr sumLarge = 0.0;
//       for(int idx : largeSet){
//         sumLarge += x[idx][b];
//       }
//       model.addConstr(sumLarge <= 2.0,
//                       "max2Large_bin"+std::to_string(b));
//     }

//     // 7) (목적함수 = 단순 feasibility)
//     //    예: model.setObjective(0.0, GRB_MINIMIZE);  // 필요 시

//     // 8) Optimize
//     model.optimize();

//     int status = model.get(GRB_IntAttr_Status);
//     // Gurobi 구버전 호환을 위해 GRB_FEASIBLE 대신 GRB_OPTIMAL만 체크
//     if(status == GRB_OPTIMAL) {
//       // feasible 해답 존재 => 100% success

//       // outMachines에 할당 정보 기록
//       // Machine(...) 은 (id, dims, capacity) 필요
//       outMachines.clear();  // 기존 내용 삭제

//       // bins 개수만큼 Machine 객체 생성
//       for(int b=0; b<numBins; b++){
//         Machine mach(b, dims, binCapacity); // (id, 차원수, 용량)
//         mach.reset(binCapacity);
//         mach.assignedJobs->clear();
//         outMachines.push_back(mach);
//       }

//       // x[i][b] == 1인 아이템 -> 해당 bin에 배정
//       for(int i=0; i<n; i++){
//         for(int b=0; b<numBins; b++){
//           double sol = x[i][b].get(GRB_DoubleAttr_X);
//           if(sol > 0.5) {
//             outMachines[b].assignedJobs->push_back(jobs[i]);
//             break;
//           }
//         }
//       }
//       return 1; // success

//     } else {
//       // infeasible or no solution found
//       return 0;
//     }
//   }
//   catch(GRBException& e) {
//     std::cerr << "Gurobi Error code = " << e.getErrorCode()
//               << " msg = " << e.getMessage() << std::endl;
//     return 0;
//   }
//   catch(...) {
//     std::cerr << "Unknown error (GurobiBinPacking)\n";
//     return 0;
//   }
// }


// // #2
// #include "GurobiBinPacking.h"
// #include "gurobi_c++.h"  // Gurobi C++ API
// #include <iostream>
// #include <stdexcept>

// /**
//  * \brief 헬퍼 함수: 아이템들을 대형/소형으로 분류한다.
//  * 
//  * \param jobs         모든 아이템
//  * \param dims         차원 수
//  * \param binCapacity  bin 용량
//  * \return largeSet    대형 아이템의 인덱스 목록
//  */
// static std::vector<int> identifyLargeItems(const std::vector<Job> &jobs,
//                                            int dims,
//                                            float binCapacity)
// {
//     std::vector<int> largeIndices;
//     for (int i = 0; i < (int)jobs.size(); i++) {
//         bool isLarge = false;
//         for (int d = 0; d < dims; d++) {
//             // 임의로 'binCapacity/3보다 큰 차원이 1개라도 있으면 대형' 이라는 가정
//             // (원본 코드와 동일한 기준)
//             if (jobs[i].getKPIVec()[d] > binCapacity / 3.0f) {
//                 isLarge = true;
//                 break;
//             }
//         }
//         if (isLarge) {
//             largeIndices.push_back(i);
//         }
//     }
//     return largeIndices;
// }

// /**
//  * \brief Gurobi 기반 Bin Packing (대형 아이템은 한 bin에 최대 maxLargePerBin개, 
//  *        소형 아이템은 용량 내에서 무제한) 모델을 풀어주는 함수 구현부.
//  */
// int solveWithGurobi(const std::vector<Job> &jobs,
//                             int dims,
//                             int numBins,
//                             float binCapacity,
//                             std::vector<Machine> &outMachines,
//                             int maxLargePerBin)
// {
//     try {
//         // ------------------------------------------------------
//         // 1) Gurobi 초기화
//         // ------------------------------------------------------
//         GRBEnv env = GRBEnv(true);
//         env.set("LogFile", "gurobi_improved.log");
//         env.start();

//         GRBModel model(env);
//         // 콘솔 로그 여부
//         model.set(GRB_IntParam_OutputFlag, 1);

//         int n = (int)jobs.size();
//         if (n == 0) {
//             std::cerr << "[WARN] 아이템이 하나도 없습니다. Feasible로 간주.\n";
//             outMachines.clear();
//             return 1; // 아무 아이템이 없으니 trivially feasible
//         }

//         // ------------------------------------------------------
//         // 2) 대형 아이템 인덱스 수집
//         // ------------------------------------------------------
//         std::vector<int> largeSet = identifyLargeItems(jobs, dims, binCapacity);

//         // ------------------------------------------------------
//         // 3) Gurobi 변수 생성: x[i][b] = 아이템 i가 bin b에 들어가면 1
//         // ------------------------------------------------------
//         //   - C++ API에서는 2차원 배열처럼 쓰려면 vector<vector<GRBVar>> 사용
//         //   - 여기서는 i in [0..n-1], b in [0..numBins-1].
//         std::vector<std::vector<GRBVar>> x(n, std::vector<GRBVar>(numBins));

//         for (int i = 0; i < n; i++) {
//             for (int b = 0; b < numBins; b++) {
//                 // 변수 이름
//                 std::string varName = "x_" + std::to_string(i) + "_" + std::to_string(b);
//                 // 0 or 1 (binary)
//                 x[i][b] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, varName);
//             }
//         }

//         // ------------------------------------------------------
//         // 4) 제약 1: 각 아이템은 정확히 1개의 bin에 배정
//         //    sum_{b=1..numBins} x[i][b] = 1  for all i
//         // ------------------------------------------------------
//         for (int i = 0; i < n; i++) {
//             GRBLinExpr expr = 0.0;
//             for (int b = 0; b < numBins; b++) {
//                 expr += x[i][b];
//             }
//             model.addConstr(expr == 1.0, "assign_item_" + std::to_string(i));
//         }

//         // ------------------------------------------------------
//         // 5) 차원별 용량 제약
//         //    sum_{i=0..n-1} ( x[i][b] * jobs[i].dim[d] ) <= binCapacity
//         //    for each bin b, each dimension d
//         // ------------------------------------------------------
//         for (int b = 0; b < numBins; b++) {
//             for (int d = 0; d < dims; d++) {
//                 GRBLinExpr sumDim = 0.0;
//                 for (int i = 0; i < n; i++) {
//                     sumDim += x[i][b] * jobs[i].getKPIVec()[d];
//                 }
//                 // 차원 d에 대해 bin b를 초과하면 안 됨
//                 model.addConstr(sumDim <= binCapacity,
//                     "cap_bin" + std::to_string(b) + "_d" + std::to_string(d));
//             }
//         }

//         // ------------------------------------------------------
//         // 6) 대형 아이템 개수 제한
//         //    sum_{i in largeSet} x[i][b] <= maxLargePerBin
//         // ------------------------------------------------------
//         for (int b = 0; b < numBins; b++) {
//             GRBLinExpr sumLarge = 0.0;
//             for (int idx : largeSet) {
//                 sumLarge += x[idx][b];
//             }
//             model.addConstr(sumLarge <= (double)maxLargePerBin,
//                 "maxLarge_bin" + std::to_string(b));
//         }

//         // ------------------------------------------------------
//         // (선택) 7) 목적함수 - 여기서는 단순 feasibility만 확인
//         // ------------------------------------------------------
//         //model.setObjective(GRBLinExpr(0.0), GRB_MINIMIZE);
//         // => bin 개수를 최소화하고 싶다면, 
//         //    sum_{b=0..numBins-1} y[b] (여기서 y[b]=1 if bin b is used) 라는 추가 변수가 필요해짐.
//         //    이건 필요 시 확장 가능.

//         // ------------------------------------------------------
//         // 8) 최적화 수행
//         // ------------------------------------------------------
//         model.optimize();

//         int status = model.get(GRB_IntAttr_Status);
//         if (status == GRB_OPTIMAL) {
//             // --------------------------------------------------
//             // feasible 해답 존재 -> 결과 추출
//             // --------------------------------------------------
//             outMachines.clear();
//             outMachines.reserve(numBins);

//             // bin 개수만큼 Machine 객체를 미리 생성
//             for (int b = 0; b < numBins; b++) {
//                 Machine mach(b, dims, binCapacity);
//                 mach.reset(binCapacity);
//                 outMachines.push_back(mach);
//             }

//             // x[i][b] == 1이면, 아이템 i를 bin b에 배정
//             for (int i = 0; i < n; i++) {
//                 for (int b = 0; b < numBins; b++) {
//                     double solVal = x[i][b].get(GRB_DoubleAttr_X);
//                     if (solVal > 0.5) {
//                         outMachines[b].assignedJobs->push_back(jobs[i]);
//                         break;
//                     }
//                 }
//             }
//             return 1; // success
//         } else {
//             // infeasible or no solution found
//             std::cerr << "[INFO] Gurobi model is infeasible or no solution found. Status = "
//                       << status << "\n";
//             return 0;
//         }
//     }
//     catch (GRBException &e) {
//         std::cerr << "[ERROR] Gurobi Error code=" << e.getErrorCode()
//                   << " msg=" << e.getMessage() << std::endl;
//         return 0;
//     }
//     catch (std::exception &ex) {
//         std::cerr << "[ERROR] Standard exception: " << ex.what() << std::endl;
//         return 0;
//     }
//     catch (...) {
//         std::cerr << "[ERROR] Unknown exception in solveWithGurobiImproved.\n";
//         return 0;
//     }
// }

// //#3 Theorem 3 알고리즘”과 동등한 해를 주는 ILP 모델을 Gurobi로 짜는 예시입니다.
// #include "gurobi_c++.h"
// #include <vector>
// #include <cmath>
// #include <iostream>
// #include <tuple>

// // pseudo-code struct for item
// struct Item {
//     std::vector<double> coords; // d-dimensional
//     bool isDummy;               // for large dummy items
// };

// // Check if an itemset fits in binCapacity dimension-wise
// bool canFit(const Item &i1, const Item &i2,
//             const std::vector<Item> &smallSet,
//             const std::vector<int> &subset,
//             double binCapacity)
// {
//     // i1.coords, i2.coords plus sum of small coords
//     // each dimension <= binCapacity
//     // Return true if feasible
//     // ...
//     return true; // implement your dimension check
// }

// int solveTheorem3UsingGurobi(
//     const std::vector<Item> &largeItems, // size = 2*l
//     const std::vector<Item> &smallItems, // size = k
//     int l,                               // number of bins
//     double binCapacity,
//     // out: which pair-subset got chosen
//     std::vector< std::tuple<int,int,std::vector<int>> > &solution
// )
// {
//     try {
//         GRBEnv env = GRBEnv(true);
//         env.set("LogFile", "Thm3Model.log");
//         env.start();

//         GRBModel model(env);
//         model.set(GRB_IntParam_OutputFlag, 1);

//         int L = (int)largeItems.size(); // should be 2*l
//         int k = (int)smallItems.size();

//         // 1) Enumerate all feasible (i<j) + subset S of small
//         //    such that dimension-wise capacity is not exceeded
//         // We'll store these combos in a structure for indexing
//         struct Combo {
//            int i, j; 
//            std::vector<int> S; // indices of small items
//         };
//         std::vector<Combo> combos;  
//         combos.reserve((L*(L-1))/2 * (1<<k)); // worst-case

//         for(int i=0; i<L; i++){
//             for(int j=i+1; j<L; j++){
//                 // For all subsets of {0..k-1}
//                 // caution: if k is large, this is huge (2^k)
//                 for(int mask=0; mask<(1<<k); mask++){
//                     // gather indices in subset
//                     std::vector<int> S;
//                     for(int s=0; s<k; s++){
//                         if(mask & (1<<s)){
//                             S.push_back(s);
//                         }
//                     }
//                     // check dimension feasibility
//                     if(canFit(largeItems[i], largeItems[j],
//                               smallItems, S, binCapacity))
//                     {
//                         Combo c; c.i=i; c.j=j; c.S=S;
//                         combos.push_back(c);
//                     }
//                 }
//             }
//         }

//         // 2) Create binary variables for each feasible combo
//         // x_comboIdx = 1 if we use that bin
//         std::vector<GRBVar> x;
//         x.reserve(combos.size());
//         for(size_t cIdx=0; cIdx<combos.size(); cIdx++){
//             x.push_back(
//                 model.addVar(0.0,1.0,0.0,GRB_BINARY,
//                               "x_combo"+std::to_string(cIdx))
//             );
//         }

//         // 3) Constraint: each large item used exactly once
//         //    sum_{combo includes large i} x_combo = 1
//         for(int i=0; i<L; i++){
//             GRBLinExpr expr=0;
//             for(size_t cIdx=0; cIdx<combos.size(); cIdx++){
//                 if(combos[cIdx].i==i || combos[cIdx].j==i){
//                     expr += x[cIdx];
//                 }
//             }
//             model.addConstr(expr == 1, "LargeUsed_"+std::to_string(i));
//         }

//         // 4) Constraint: each small item used exactly once
//         //    sum_{combo includes small s} x_combo = 1
//         for(int s=0; s<k; s++){
//             GRBLinExpr expr=0;
//             for(size_t cIdx=0; cIdx<combos.size(); cIdx++){
//                 // check if combos[cIdx].S contains s
//                 // (since combos[cIdx].S is small, we can do a linear search)
//                 for(int sIdx: combos[cIdx].S){
//                     if(sIdx == s){
//                         expr += x[cIdx];
//                         break;
//                     }
//                 }
//             }
//             model.addConstr(expr == 1, "SmallUsed_"+std::to_string(s));
//         }

//         // 5) Constraint: exactly l bins selected
//         //    sum_{all combos} x_combo = l
//         {
//             GRBLinExpr expr=0;
//             for(size_t cIdx=0; cIdx<combos.size(); cIdx++){
//                 expr += x[cIdx];
//             }
//             model.addConstr(expr == l, "UseExactlyLbins");
//         }

//         // 6) Optimize
//         model.optimize();
//         int status = model.get(GRB_IntAttr_Status);
//         if(status == GRB_OPTIMAL){
//             // Reconstruct solution
//             // combos[cIdx] chosen if x[cIdx] == 1
//             solution.clear();
//             for(size_t cIdx=0; cIdx<combos.size(); cIdx++){
//                 double val = x[cIdx].get(GRB_DoubleAttr_X);
//                 if(val>0.5){
//                     // store (i, j, subsetS)
//                     solution.push_back(
//                             std::make_tuple(
//                          combos[cIdx].i,
//                                          combos[cIdx].j,
//                                          combos[cIdx].S ));
//                 }
//             }
//             return 1; // feasible
//         } else {
//             // infeasible or no solution
//             return 0;
//         }
//     }
//     catch(GRBException &e){
//         std::cerr<<"[ERROR] Gurobi code="<<e.getErrorCode()
//                  <<" msg="<<e.getMessage()<<std::endl;
//         return 0;
//     }
//     catch(...){
//         std::cerr<<"[ERROR] Unknown exception\n";
//         return 0;
//     }
// }










// // #4 bin 25개 나오는거.

// #include <vector>
// #include <string>
// #include <iostream>
// #include "gurobi_c++.h"    // Gurobi C++ API header
// #include "Job.h"
// #include "Machine.h"
// #include "Environment.h"
// #include "GurobiBinPacking.h"

// // "대형아이템 최대 2개" + "소형아이템은 용량(binCapacity) 이하" Bin Packing MIP
// int solveWithGurobi(const std::vector<Job> &jobs,
//                     int dims,
//                     int numBins,       // bin 개수 (env->machines 등)
//                     float binCapacity, // 예: 1000.0
//                     std::vector<Machine> &outMachines)
// {
//   try {
//     // 1) Gurobi 초기화
//     GRBEnv env = GRBEnv(true);
//     env.set("LogFile", "gurobi.log");
//     env.start();

//     GRBModel model(env);
//     model.set(GRB_IntParam_OutputFlag, 1); // 콘솔 로그 활성화

//     int n = (int)jobs.size();

//     // 2) 대형 아이템 인덱스 수집
//     std::vector<int> largeSet; // large item indices
//     for(int i=0; i<n; i++){
//       bool isLarge = false;
//       for(int d=0; d<dims; d++){
//         if(jobs[i].getKPIVec()[d] > binCapacity / 3.0f) {
//           isLarge = true;
//           break;
//         }
//       }
//       if(isLarge) largeSet.push_back(i);
//     }

//     // 3) 이진 변수 x[i][b]: 아이템 i가 bin b에 들어가면 1
//     std::vector<std::vector<GRBVar>> x(n, std::vector<GRBVar>(numBins));
//     for(int i=0; i<n; i++){
//       for(int b=0; b<numBins; b++){
//         std::string varName = "x_"+std::to_string(i)+"_"+std::to_string(b);
//         x[i][b] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, varName);
//       }
//     }

//     // 4) 제약: 각 아이템은 정확히 1개의 bin
//     for(int i=0; i<n; i++){
//       GRBLinExpr expr = 0.0;
//       for(int b=0; b<numBins; b++){
//         expr += x[i][b];
//       }
//       model.addConstr(expr == 1.0, "assign_item_"+std::to_string(i));
//     }

//     // 5) 차원별 용량 제약: sum( item i의 x[i][b] * KPIVec[d] ) ≤ binCapacity
//     for(int b=0; b<numBins; b++){
//       for(int d=0; d<dims; d++){
//         GRBLinExpr sumDim = 0.0;
//         for(int i=0; i<n; i++){
//           sumDim += x[i][b] * jobs[i].getKPIVec()[d];
//         }
//         model.addConstr(sumDim <= binCapacity,
//                         "cap_bin"+std::to_string(b)+"_d"+std::to_string(d));
//       }
//     }

//     // 6) 대형 아이템 최대 2개 제약
//     //    sum_{i in largeSet} x[i][b] ≤ 2
//     for(int b=0; b<numBins; b++){
//       GRBLinExpr sumLarge = 0.0;
//       for(int idx : largeSet){
//         sumLarge += x[idx][b];
//       }
//       model.addConstr(sumLarge <= 2.0,
//                       "max2Large_bin"+std::to_string(b));
//     }

//     // 7) (목적함수 = 단순 feasibility)
//     //    예: model.setObjective(0.0, GRB_MINIMIZE);  // 필요 시

//     // 8) Optimize
//     model.optimize();

//     int status = model.get(GRB_IntAttr_Status);
//     // Gurobi 구버전 호환을 위해 GRB_FEASIBLE 대신 GRB_OPTIMAL만 체크
//     if(status == GRB_OPTIMAL) {
//       // feasible 해답 존재 => 100% success

//       // outMachines에 할당 정보 기록
//       // Machine(...) 은 (id, dims, capacity) 필요
//       outMachines.clear();  // 기존 내용 삭제

//       // bins 개수만큼 Machine 객체 생성
//       for(int b=0; b<numBins; b++){
//         Machine mach(b, dims, binCapacity); // (id, 차원수, 용량)
//         mach.reset(binCapacity);
//         mach.assignedJobs->clear();
//         outMachines.push_back(mach);
//       }

//       // x[i][b] == 1인 아이템 -> 해당 bin에 배정
//       for(int i=0; i<n; i++){
//         for(int b=0; b<numBins; b++){
//           double sol = x[i][b].get(GRB_DoubleAttr_X);
//           if(sol > 0.5) {
//             outMachines[b].assignedJobs->push_back(jobs[i]);
//             break;
//           }
//         }
//       }
//       return 1; // success

//     } else {
//       // infeasible or no solution found
//       return 0;
//     }
//   }
//   catch(GRBException& e) {
//     std::cerr << "Gurobi Error code = " << e.getErrorCode()
//               << " msg = " << e.getMessage() << std::endl;
//     return 0;
//   }
//   catch(...) {
//     std::cerr << "Unknown error (GurobiBinPacking)\n";
//     return 0;
//   }
// }










// #4-1 1. average computation time (s) 을 결과값으로 받고싶어. 
//실행시간이 600초에 도달했을 경우 계산을 멈추고 그 상태에서 결과값을 보고싶어.
// 문제점:실험 루프(예: 여러 run을 반복하는 상위 코드)에서 각 run마다 solveWithGurobi()를 호출하고 있으며, 한 run에서 600초 제한에 도달하여 시간 제한 상태가 되면 해당 run은 완료된 것으로 처리되어 다음 run이 시작됩니다.
// #include <vector>
// #include <string>
// #include <iostream>
// #include "gurobi_c++.h"    // Gurobi C++ API header
// #include "Job.h"
// #include "Machine.h"
// #include "Environment.h"
// #include "GurobiBinPacking.h"

// // "대형아이템 최대 2개" + "소형아이템은 용량(binCapacity) 이하" Bin Packing MIP
// // 변경사항: 1) 함수 반환형을 double로 바꿔 Gurobi 실행시간(초)을 반환
// //          2) 600초 초과 시 중단(부분해라도 있으면 그 해를 가져옴)
// double solveWithGurobi(const std::vector<Job> &jobs,
//                        int dims,
//                        int numBins,       // bin 개수 (env->machines 등)
//                        float binCapacity, // 예: 1000.0
//                        std::vector<Machine> &outMachines)
// {
//   try {
//     // [A] Gurobi 초기화
//     GRBEnv env = GRBEnv(true);
//     env.set("LogFile", "gurobi.log");
//     env.start();

//     GRBModel model(env);
//     model.set(GRB_IntParam_OutputFlag, 1); // 콘솔 로그 활성화

//     // (1) 제한시간 600초 설정
//     model.set(GRB_DoubleParam_TimeLimit, 600.0);

//     int n = static_cast<int>(jobs.size());

//     // [B] 대형 아이템 인덱스 수집
//     std::vector<int> largeSet; // large item indices
//     for(int i = 0; i < n; i++){
//       bool isLarge = false;
//       for(int d = 0; d < dims; d++){
//         if(jobs[i].getKPIVec()[d] > binCapacity / 3.0f) {
//           isLarge = true;
//           break;
//         }
//       }
//       if(isLarge) largeSet.push_back(i);
//     }

//     // [C] 이진 변수 x[i][b]: 아이템 i가 bin b에 들어가면 1
//     std::vector<std::vector<GRBVar>> x(n, std::vector<GRBVar>(numBins));
//     for(int i = 0; i < n; i++){
//       for(int b = 0; b < numBins; b++){
//         std::string varName = "x_" + std::to_string(i) + "_" + std::to_string(b);
//         x[i][b] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, varName);
//       }
//     }

//     // [D] 제약 1: 각 아이템은 정확히 1개의 bin
//     for(int i=0; i<n; i++){
//       GRBLinExpr expr = 0.0;
//       for(int b=0; b<numBins; b++){
//         expr += x[i][b];
//       }
//       model.addConstr(expr == 1.0, "assign_item_" + std::to_string(i));
//     }

//     // [E] 제약 2: 차원별 용량
//     // sum( x[i][b] * item_i_dimension_d ) <= binCapacity
//     for(int b=0; b<numBins; b++){
//       for(int d=0; d<dims; d++){
//         GRBLinExpr sumDim = 0.0;
//         for(int i=0; i<n; i++){
//           sumDim += x[i][b] * jobs[i].getKPIVec()[d];
//         }
//         model.addConstr(sumDim <= binCapacity,
//                         "cap_bin"+std::to_string(b)+"_d"+std::to_string(d));
//       }
//     }

//     // [F] 제약 3: 대형 아이템 최대 2개
//     // sum_{i in largeSet} x[i][b] <= 2
//     for(int b=0; b<numBins; b++){
//       GRBLinExpr sumLarge = 0.0;
//       for(int idx : largeSet){
//         sumLarge += x[idx][b];
//       }
//       model.addConstr(sumLarge <= 2.0, "max2Large_bin" + std::to_string(b));
//     }

//     // [G] 목적함수 설정 (단순 Feasibility)
//     // model.setObjective(0.0, GRB_MINIMIZE);

//     // [H] Optimize
//     model.optimize();

//     // (2) Gurobi가 실제로 사용한 런타임(초) 가져오기
//     double runtimeSec = model.get(GRB_DoubleAttr_Runtime);

//     int status = model.get(GRB_IntAttr_Status);
//     // GRB_TIME_LIMIT이나 GRB_OPTIMAL일 때 둘 다 해를 읽을 수 있음
//     if(status == GRB_OPTIMAL || status == GRB_TIME_LIMIT) {
//       // feasible 해답(또는 부분해) 존재

//       outMachines.clear();  // 기존 내용 삭제
//       outMachines.reserve(numBins);

//       // bins 개수만큼 Machine 객체 생성
//       for(int b = 0; b < numBins; b++){
//         Machine mach(b, dims, binCapacity);
//         mach.reset(binCapacity);
//         mach.assignedJobs->clear();
//         outMachines.push_back(mach);
//       }

//       // x[i][b] == 1이면 해당 bin에 배정
//       for(int i=0; i<n; i++){
//         for(int b=0; b<numBins; b++){
//           double sol = x[i][b].get(GRB_DoubleAttr_X);
//           if(sol > 0.5) {
//             outMachines[b].assignedJobs->push_back(jobs[i]);
//             break;
//           }
//         }
//       }
//       // 1: success 대신, runtime(초)를 리턴하도록 변경
//       return runtimeSec;

//     } else if(status == GRB_INFEASIBLE) {
//       std::cerr << "[Gurobi] Model is infeasible.\n";
//       // -1 등으로 표기 가능. 여기서는 0.0 리턴
//       return 0.0;

//     } else {
//       std::cerr << "[Gurobi] Unknown status or no solution found. status = "
//                 << status << "\n";
//       // 부분해도 없으면 0.0
//       return 0.0;
//     }
//   }
//   catch(GRBException& e) {
//     std::cerr << "Gurobi Error code = " << e.getErrorCode()
//               << " msg = " << e.getMessage() << std::endl;
//     return 0.0;
//   }
//   catch(...) {
//     std::cerr << "Unknown error (GurobiBinPacking)\n";
//     return 0.0;
//   }
// }










// #4-2 만약 전체 실험을 600초 내에 모두 마치고 싶다면, 상위 레벨에서 타이머를 두어 600초가 넘어가면 더 이상 새로운 run을 시작하지 않고 결과를 집계하도록 제어해야 합니다.
// 예를 들어, 각 run의 실행시간을 누적하여 전체 경과시간을 확인하고, 전체 경과시간이 600초를 넘으면 반복문을 조기 종료하도록 하면 됩니다.
#include <vector>
#include <string>
#include <iostream>
#include "gurobi_c++.h"    // Gurobi C++ API header
#include "Job.h"
#include "Machine.h"
#include "Environment.h"
#include "GurobiBinPacking.h"

// "대형아이템 최대 2개" + "소형아이템은 용량(binCapacity) 이하" Bin Packing MIP
//  - (1) 600초 TimeLimit 설정
//  - (2) Gurobi 실행시간(초)을 반환
double solveWithGurobi(const std::vector<Job> &jobs,
                       int dims,
                       int numBins,       // bin 개수
                       float binCapacity, // 예: 1000.0
                       std::vector<Machine> &outMachines)
{
  try {
    // [A] Gurobi 초기화
    GRBEnv env = GRBEnv(true);
    env.set("LogFile", "gurobi.log");
    env.start();

    GRBModel model(env);
    model.set(GRB_IntParam_OutputFlag, 1); // 콘솔 로그 활성화

    // (1) 제한시간 600초 설정
    model.set(GRB_DoubleParam_TimeLimit, 3600.0);

    int n = static_cast<int>(jobs.size());

    // [B] 대형 아이템 인덱스 수집
    std::vector<int> largeSet; // large item indices
    largeSet.reserve(n);

    for(int i = 0; i < n; i++){
      bool isLarge = false;
      for(int d = 0; d < dims; d++){
        if(jobs[i].getKPIVec()[d] > binCapacity / 3.0f) {
          isLarge = true;
          break;
        }
      }
      if(isLarge) largeSet.push_back(i);
    }

    std::cout << "[INFO] Number of large items: " << largeSet.size() << std::endl;

    // [C] 이진 변수 x[i][b]: 아이템 i가 bin b에 들어가면 1
    std::vector<std::vector<GRBVar>> x(n, std::vector<GRBVar>(numBins));
    for(int i = 0; i < n; i++){
      for(int b = 0; b < numBins; b++){
        std::string varName = "x_" + std::to_string(i) + "_" + std::to_string(b);
        x[i][b] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, varName);
      }
    }

    // [D] 제약 1: 각 아이템은 정확히 1개의 bin
    for(int i=0; i<n; i++){
      GRBLinExpr expr = 0.0;
      for(int b=0; b<numBins; b++){
        expr += x[i][b];
      }
      model.addConstr(expr == 1.0, "assign_item_" + std::to_string(i));
    }

    // [E] 제약 2: 차원별 용량
    // sum( x[i][b] * jobs[i].getKPIVec()[d] ) <= binCapacity
    for(int b=0; b<numBins; b++){
      for(int d=0; d<dims; d++){
        GRBLinExpr sumDim = 0.0;
        for(int i=0; i<n; i++){
          sumDim += x[i][b] * jobs[i].getKPIVec()[d];
        }
        model.addConstr(sumDim <= binCapacity,
                        "cap_bin"+std::to_string(b)+"_d"+std::to_string(d));
      }
    }

    // [F] 제약 3: 대형 아이템 최대 2개
    // sum_{i in largeSet} x[i][b] <= 2
    for(int b=0; b<numBins; b++){
      GRBLinExpr sumLarge = 0.0;
      for(int idx : largeSet){
        sumLarge += x[idx][b];
      }
      model.addConstr(sumLarge <= 2.0, "max2Large_bin" + std::to_string(b));
    }

    // [G] 목적함수 설정 (단순 Feasibility)
    // model.setObjective(0.0, GRB_MINIMIZE);

    // [H] Optimize
    model.optimize();

    // (2) 실제 Gurobi 실행시간(초) 가져오기
    double runtimeSec = model.get(GRB_DoubleAttr_Runtime);

    int status = model.get(GRB_IntAttr_Status);

    // GRB_TIME_LIMIT이면 부분해라도 있을 수 있음
    // GRB_OPTIMAL이면 최적해 있음
    if(status == GRB_OPTIMAL || status == GRB_TIME_LIMIT) {
      // feasible 해답(또는 부분해) 존재하는지 먼저 확인
      int solCount = model.get(GRB_IntAttr_SolCount);
      if(solCount > 0) {
        // 해가 하나 이상 존재 -> 변수값 추출
        outMachines.clear();
        outMachines.reserve(numBins);

        // bins 개수만큼 Machine 객체 생성
        for(int b = 0; b < numBins; b++){
          Machine mach(b, dims, binCapacity);
          mach.reset(binCapacity);
          mach.assignedJobs->clear();
          outMachines.push_back(mach);
        }

        // x[i][b] == 1이면 해당 bin에 배정
        for(int i=0; i<n; i++){
          for(int b=0; b<numBins; b++){
            double sol = x[i][b].get(GRB_DoubleAttr_X);
            if(sol > 0.5) {
              outMachines[b].assignedJobs->push_back(jobs[i]);
              break;
            }
          }
        }
      } else {
        // feasible 해를 못 찾음 (solution count=0)
        std::cerr << "[Gurobi] Time limit or optimal, but no feasible solution found.\n";
      }
      return runtimeSec;

    } else if(status == GRB_INFEASIBLE) {
      std::cerr << "[Gurobi] Model is infeasible.\n";
      return 0.0;

    } else {
      // 부분해도 없으면 0.0
      std::cerr << "[Gurobi] Unknown status or no solution found. status = "
                << status << "\n";
      return 0.0;
    }
  }
  catch(GRBException& e) {
    std::cerr << "Gurobi Error code = " << e.getErrorCode()
              << " msg = " << e.getMessage() << std::endl;
    return 0.0;
  }
  catch(...) {
    std::cerr << "Unknown error (GurobiBinPacking)\n";
    return 0.0;
  }
}











// //#5 초기 구로비
// #include <vector>
// #include <string>
// #include <iostream>
// #include "gurobi_c++.h"        // Gurobi C++ API header
// #include "Job.h"
// #include "Machine.h"
// #include "Environment.h"

// // 이 함수가 "대형=2개 이하, 소형=제약"인 Bin Packing을 MIP로 풀고
// // 성공 시 outMachines에 배정 결과를 써넣는다.
// int solveWithGurobi(const std::vector<Job> &jobs,
//                     int dims,
//                     int numBins,            // bin 개수 (env->machines 등)
//                     float binCapacity,      // 1.0 or 1000.0, etc
//                     std::vector<Machine> &outMachines)
// {
//   try {
//     // 1) Gurobi 환경 + 모델
//     GRBEnv env = GRBEnv(true);
//     env.set("LogFile", "gurobi.log");
//     env.start();

//     GRBModel model(env);
//     model.set(GRB_IntParam_OutputFlag, 1); // 로그 출력

//     int n = (int)jobs.size();
//     // 2) 아이템별로 대형인지 판정
//     std::vector<int> largeSet;  // large item indices
//     for(int i=0; i<n; i++){
//       bool isLarge = false;
//       for(int d=0; d<dims; d++){
//         if(jobs[i].getKPIVec()[d] > binCapacity/3.0f) {
//           isLarge = true; break;
//         }
//       }
//       if(isLarge) largeSet.push_back(i);
//     }

//     // 3) MIP 변수: x_{i,b} (item i가 bin b에 들어가면 1)
//     //    크기: n * numBins
//     //    Gurobi에서는 GRBVar 2차원 배열처럼 다룰 수 있게 vector<vector<GRBVar>> 사용
//     std::vector<std::vector<GRBVar>> x(n, std::vector<GRBVar>(numBins));
//     for(int i=0; i<n; i++){
//       for(int b=0; b<numBins; b++){
//         // 이름 붙이기(디버그용)
//         std::string varName = "x_"+std::to_string(i)+"_"+std::to_string(b);
//         x[i][b] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, varName);
//       }
//     }

//     // 4) 제약: 각 아이템은 정확히 1개의 bin
//     for(int i=0; i<n; i++){
//       GRBLinExpr expr = 0.0;
//       for(int b=0; b<numBins; b++){
//         expr += x[i][b];
//       }
//       model.addConstr(expr == 1.0, "assign_item_"+std::to_string(i));
//     }

//     // 5) 제약: 각 bin에 대해, 차원별로 합 ≤ binCapacity
//     //    소형이든 대형이든 다 합쳐서 coords[d]의 합 <= binCapacity
//     //    (문제에 따라 binCapacity=1 이거나 1000 등)
//     for(int b=0; b<numBins; b++){
//       for(int d=0; d<dims; d++){
//         GRBLinExpr sumDim = 0.0;
//         for(int i=0; i<n; i++){
//           sumDim += x[i][b] * jobs[i].getKPIVec()[d];
//         }
//         model.addConstr(sumDim <= binCapacity, "capacity_b"+std::to_string(b)+"_d"+std::to_string(d));
//       }
//     }

//     // 6) 제약: 대형아이템은 한 bin에 최대 2개
//     //    => sum_{i in largeSet} x_{i,b} <= 2
//     for(int b=0; b<numBins; b++){
//       GRBLinExpr sumLarge = 0.0;
//       for(int idx : largeSet){
//         sumLarge += x[idx][b];
//       }
//       model.addConstr(sumLarge <= 2.0, "max2Large_bin"+std::to_string(b));
//     }

//     // 7) 목적함수 (없다면 Feasibility)
//     //    여기서는 단순히 feasible 확인만 (정해진 bin수 numBins 안에서 가능하면 success)
//     //    굳이 obj를 넣지 않아도 OK. => model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
//     // model.setObjective( 0.0 , GRB_MINIMIZE );

//     // 8) Optimize
//     model.optimize();

//     int status = model.get(GRB_IntAttr_Status);
//     if(status == GRB_OPTIMAL){
//       // 해답 존재 => 100% success
//       // 9) 해를 해석해서 outMachines에 기록
//       outMachines.resize(numBins); // bin 수만큼 Machine 준비
//       for(int b=0; b<numBins; b++){
//         outMachines[b].setCapacity(binCapacity); 
//         outMachines[b].reset(binCapacity); 
//         outMachines[b].assignedJobs->clear();
//       }
//       // x[i][b]=1이면, 아이템 i가 bin b에 들어감
//       for(int i=0; i<n; i++){
//         for(int b=0; b<numBins; b++){
//           double sol = x[i][b].get(GRB_DoubleAttr_X);
//           if(sol > 0.5) {
//             // 아이템 i를 bin b에 배정
//             outMachines[b].assignedJobs->push_back(jobs[i]);
//             break;
//           }
//         }
//       }
//       return 1; // success
//     } else {
//       // infeasible or no solution found
//       return 0;
//     }
//   }
//   catch(GRBException e) {
//     std::cerr<<"Gurobi Error code="<< e.getErrorCode()
//              <<" msg="<< e.getMessage()<<"\n";
//     return 0;
//   }
//   catch(...) {
//     std::cerr<<"Unknown error\n";
//     return 0;
//   }
// }
