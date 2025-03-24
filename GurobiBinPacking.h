#ifndef GUROBI_BIN_PACKING_H
#define GUROBI_BIN_PACKING_H

#include <vector>
#include "Job.h"
#include "Machine.h"

/**
 * solveWithGurobi:
 *  - jobs: 모든 아이템 (대형+소형 구분은 내부에서 or 필요 없을 수도)
 *  - dims: 차원 수
 *  - numBins: 사용할 bin(머신) 개수
 *  - binCapacity: bin의 용량 (예: 1000.0)
 *  - outMachines: 해답을 담을 벡터 (bin별 assignedJobs)
 * 반환값:
 *   1 => feasible (성공)
 *   0 => infeasible or error
 */


double solveWithGurobi(const std::vector<Job> &jobs,
                    int dims,
                    int numBins,
                    float binCapacity,
                    std::vector<Machine> &outMachines);

#endif


// #ifndef IMPROVED_GUROBI_BIN_PACKING_H
// #define IMPROVED_GUROBI_BIN_PACKING_H

// #include <vector>
// #include "Job.h"
// #include "Machine.h"

// // /**
// //  * \brief Gurobi 기반 Bin Packing (대형 아이템은 한 bin에 최대 maxLargePerBin개, 
// //  *        소형 아이템은 용량 내에서 무제한) 모델을 풀어주는 함수.
// //  * 
// //  * \param jobs           모든 아이템 (대형/소형 여부는 내부 로직에서 판단)
// //  * \param dims           아이템(머신)의 차원 수
// //  * \param numBins        사용할 bin(머신) 개수
// //  * \param binCapacity    각 bin의 용량 (모든 차원에서 동일하다고 가정)
// //  * \param outMachines    해답(할당 결과)을 담을 벡터 (bin별 assignedJobs를 저장)
// //  * \param maxLargePerBin bin 하나에 들어갈 수 있는 대형 아이템의 최대 개수 (기본 2)
// //  * \return 1이면 feasible (성공), 0이면 infeasible 혹은 예외발생
// //  */
// int solveWithGurobi(const std::vector<Job> &jobs,
//                             int dims,
//                             int numBins,
//                             float binCapacity,
//                             std::vector<Machine> &outMachines,
//                             int maxLargePerBin = 2);

// #endif
