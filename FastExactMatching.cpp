// 위 코드에 600초 이상 되면 작업 중단하는 코드 추가
#include <bits/stdc++.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include "Job.h"
#include "Machine.h"
#include "Environment.h"
#include "MathFunctions.h"
#include "GurobiBinPacking.h"

using namespace std;
using boost::multiprecision::cpp_int;

static long long GLOBAL_TARGET = 0;

struct Term {
    cpp_int exp;
    cpp_int coeff;
    Term() : exp(0), coeff(0) {}
    Term(const cpp_int &e, const cpp_int &c) : exp(e), coeff(c) {}
};

struct Polynomial {
    vector<Term> terms;
    Polynomial() { terms.clear(); }
    static Polynomial monomial(const cpp_int &exp, const cpp_int &c) {
        Polynomial p;
        if(c != 0) {
            p.terms.push_back(Term(exp, c));
        }
        return p;
    }
    static bool exceedTarget(const cpp_int &e) {
        cpp_int limit = GLOBAL_TARGET + 1;
        return (e >= limit);
    }
    void applyModCut() {
        // GLOBAL_TARGET보다 큰 지수(exp)는 버림
        auto it = remove_if(terms.begin(), terms.end(), [&](const Term &t){
            return exceedTarget(t.exp);
        });
        if(it != terms.end()) {
            terms.erase(it, terms.end());
        }
    }
    void normalizeAndCut() {
        if(terms.empty()) return;
        sort(terms.begin(), terms.end(), [&](auto &a, auto &b){
            return a.exp < b.exp;
        });
        vector<Term> tmp;
        tmp.reserve(terms.size());
        cpp_int curE = terms[0].exp;
        cpp_int curC = terms[0].coeff;
        for(size_t i = 1; i < terms.size(); i++){
            if(terms[i].exp == curE) {
                curC += terms[i].coeff;
            } else {
                if(curC != 0) tmp.push_back(Term(curE, curC));
                curE = terms[i].exp;
                curC = terms[i].coeff;
            }
        }
        if(curC != 0) tmp.push_back(Term(curE, curC));
        terms = move(tmp);
        applyModCut();
    }
    Polynomial add(const Polynomial &o) const {
        Polynomial r;
        r.terms.reserve(terms.size() + o.terms.size());
        size_t i = 0, j = 0;
        while(i < terms.size() && j < o.terms.size()){
            if(terms[i].exp < o.terms[j].exp) {
                r.terms.push_back(terms[i]);
                i++;
            } else if(terms[i].exp > o.terms[j].exp) {
                r.terms.push_back(o.terms[j]);
                j++;
            } else {
                cpp_int c = terms[i].coeff + o.terms[j].coeff;
                if(c != 0) {
                    r.terms.push_back(Term(terms[i].exp, c));
                }
                i++; j++;
            }
        }
        while(i < terms.size()){
            r.terms.push_back(terms[i]);
            i++;
        }
        while(j < o.terms.size()){
            r.terms.push_back(o.terms[j]);
            j++;
        }
        r.normalizeAndCut();
        return r;
    }
    Polynomial negate() const {
        Polynomial r;
        r.terms.reserve(terms.size());
        for(auto &t: terms){
            r.terms.push_back(Term(t.exp, -t.coeff));
        }
        return r;
    }
    cpp_int getCoeff(const cpp_int &e) const {
        // 이진 탐색으로 해당 exp의 계수 찾기
        int L = 0, R = (int)terms.size() - 1;
        while(L <= R){
            int mid = (L + R) / 2;
            if(terms[mid].exp == e) {
                return terms[mid].coeff;
            } else if(terms[mid].exp < e) {
                L = mid + 1;
            } else {
                R = mid - 1;
            }
        }
        return 0;
    }
};

Polynomial polyMultiply(const Polynomial &A, const Polynomial &B) {
    Polynomial r;
    r.terms.reserve(A.terms.size() * B.terms.size());
    for(auto &ta : A.terms){
        for(auto &tb : B.terms){
            cpp_int e = ta.exp + tb.exp;
            cpp_int c = ta.coeff * tb.coeff;
            if(c != 0){
                r.terms.push_back(Term(e, c));
            }
        }
    }
    r.normalizeAndCut();
    return r;
}

struct EdgeProp {
    long long weight;
    long long isoCost;
};

typedef boost::adjacency_list<
    boost::multisetS,
    boost::vecS,
    boost::undirectedS,
    boost::no_property,
    EdgeProp
> MyBGLGraph;

// 소형 아이템 집합 {i}에 대한 인덱스 마스크 -> (개수 << k) | 마스크
static long long encodeSubsetWeight(int mask, int k) {
    int pc = 0;
    { 
        int tmp = mask;
        while(tmp) {
            pc += (tmp & 1);
            tmp >>= 1;
        }
    }
    return ((long long)pc << k) | mask;
}

// 그래프 구성: large item 두 개 + (부분집합 small) -> feasible하면 간선
void buildEdgesBGL(const std::vector<Job> &large,
                   const std::vector<Job> &small,
                   float binSize,
                   int dims,
                   MyBGLGraph &g)
{
    int L = (int)large.size();
    int K = (int)small.size();
    for(int i = 0; i < L; i++){
        for(int j = i + 1; j < L; j++){
            for(int mask = 0; mask < (1 << K); mask++){
                double sumDim[64];
                for(int d = 0; d < dims; d++){
                    sumDim[d] = large[i].getKPIVec()[d] + large[j].getKPIVec()[d];
                }
                bool feasible = true;
                for(int s = 0; s < K && feasible; s++){
                    if(mask & (1 << s)){
                        for(int d = 0; d < dims; d++){
                            sumDim[d] += small[s].getKPIVec()[d];
                            if(sumDim[d] > binSize){
                                feasible = false;
                                break;
                            }
                        }
                    }
                }
                if(feasible){
                    long long w = encodeSubsetWeight(mask, K);
                    EdgeProp ep; 
                    ep.weight = w; 
                    ep.isoCost = 0;
                    boost::add_edge(i, j, ep, g);
                }
            }
        }
    }
}

// Isolation Lemma 무작위 비용 할당
void assignIsolationCostBGL(MyBGLGraph &g)
{
    static std::random_device rd;
    static std::mt19937_64 rng(rd());
    auto E = boost::edges(g);
    long long eCount = (long long)boost::num_edges(g);
    // 비용 범위 설정
    long long limit = 1LL << ((eCount > 50) ? 50 : eCount);
    std::uniform_int_distribution<long long> dist(1, limit);
    for(auto it = E.first; it != E.second; ++it){
        g[*it].isoCost = dist(rng);
    }
}

static const cpp_int SHIFT = (cpp_int(1) << 40);

struct PolyMatrix {
    int n;
    vector<Polynomial> mat;
    PolyMatrix(int N) : n(N), mat(N * N) {}
    inline Polynomial &at(int r, int c){
        return mat[r * n + c];
    }
};

// EdgeProp -> Pfaffian 행렬
void buildMatrixBGL(int n, const MyBGLGraph &g, PolyMatrix &A)
{
    // 초기화
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            A.at(i, j) = Polynomial();
        }
    }
    // 간선 정보를 행렬에 반영
    auto E = boost::edges(g);
    for(auto it = E.first; it != E.second; ++it){
        auto ed = *it;
        int u = boost::source(ed, g);
        int v = boost::target(ed, g);
        if(u == v) continue; // self loop 방지
        int i = (u < v) ? u : v;
        int j = (u < v) ? v : u;
        long long cVal = g[ed].isoCost;
        long long wVal = g[ed].weight;
        // cVal : Isolation Lemma 비용, wVal : 소형 아이템 인덱스 인코딩
        cpp_int e = cpp_int(cVal) * SHIFT + wVal;
        // 모노미얼(지수=e, 계수=1)
        Polynomial addTerm = Polynomial::monomial(e, 1);
        A.at(i, j) = A.at(i, j).add(addTerm);
    }
    // skew-symmetric 맞춰주기
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            A.at(j, i) = A.at(i, j).negate();
        }
    }
}

// Mulmuley-Vazirani-Vazirani (MVV) Pfaffian 계산 (재귀)
static Polynomial pfaffianMVV(const PolyMatrix &A, vector<bool> &used, int startIdx = 0)
{
    int n = A.n;
    int idx = -1;
    for(int i = startIdx; i < n; i++){
        if(!used[i]){
            idx = i; 
            break;
        }
    }
    if(idx < 0){
        // 모든 노드 사용 -> 공 Pfaffian = 1
        return Polynomial::monomial(0, 1);
    }
    used[idx] = true;
    Polynomial ret;
    for(int j = idx + 1; j < n; j++){
        if(!used[j]){
            used[j] = true;
            // sign trick
            int freeCount = 0;
            for(int mid = idx + 1; mid < j; mid++){
                if(!used[mid]) freeCount++;
            }
            Polynomial edgePoly = A.mat[idx * n + j];
            if((freeCount & 1) == 1){
                edgePoly = edgePoly.negate();
            }
            // 재귀 호출
            Polynomial subPf = pfaffianMVV(A, used, idx + 1);
            Polynomial mul = polyMultiply(edgePoly, subPf);
            Polynomial tmp = ret.add(mul);
            ret = tmp;
            used[j] = false;
        }
    }
    used[idx] = false;
    return ret;
}

// 최종 Pfaffian에서 지수==T 인 항의 계수 가져오기
cpp_int computePfaffianMVV(PolyMatrix &A, long long T)
{
    GLOBAL_TARGET = T;
    vector<bool> used(A.n, false);
    Polynomial pf = pfaffianMVV(A, used, 0);
    return pf.getCoeff(T);
}

//--------------------
//  planJobs()
//--------------------
// "6개 bin만 써야 한다" 식으로 bin 개수를 고정하거나,
// 혹은 TimeLimit을 600초로 설정하여 중단 시 해를 가져온다.
// 여기서는 bin 개수를 25로 세팅하는 예시로 보여줌





static bool GLOBAL_TERMINATE = false;





extern "C" int planJobs(int &counter,
                        std::vector<Machine> **out_machines,
                        std::vector<Job> *jq,
                        std::vector<Machine> *mq,
                        float eps)
                        




{

    if (GLOBAL_TERMINATE) {
        std::cerr << "[planJobs] Execution stopped due to previous time limit.\n";
        return 0; // 더 이상 실행되지 않음
    }





    float binSize = 100.0f;
    int dims = mq->at(0).getDimensions();
    std::cout << "--- Using MIP solver with Gurobi ---\n";

    // 모든 Job, Machine 정보 준비
    std::vector<Job> allJobs = *jq;
    // 여기서는 "25개의 bin"이라고 하드코딩
    // (원래는 mq->size() 등으로 쓸 수도 있음)
    int numBins = 36; 

    // Gurobi로 풀기
    std::vector<Machine> solution;





    // solveWithGurobi()가 "double"을 반환한다고 가정 (시간초과 시 600.0 이상)
    // double runtimeSec = solveWithGurobi(allJobs, dims, numBins, binSize, solution);
    // "time limit에 걸리면 전체 실험을 중단"한다면:
    // if(runtimeSec >= 600.0) {
    //     // Gurobi가 제한 시간 만에 끝났음 -> 다음 run 안 돌리고 종료
    //     std::cout << "[planJobs] TimeLimit 600s reached -> stopping now.\n";
    //     // 원하는 처리(프로그램 종료, 혹은 특별 값 반환) etc.
    //     // 여기서는 2를 리턴해서 "타임리밋 상태"라고 가정
    //     *out_machines = new std::vector<Machine>();
    //     return 2;
    // }
    double runtimeSec = solveWithGurobi(allJobs, dims, numBins, binSize, solution);

    // 실행 시간이 600초 이상이면 프로그램 종료
    if(runtimeSec >= 3600.0) {
        std::cerr << "[planJobs] TimeLimit 3600s reached -> terminating all runs.\n";
        exit(0); // 프로그램 완전 종료
    }




    // 만약 solveWithGurobi()에서 feasible 해를 찾았다면
    // (기존 코드처럼) outMachines에 결과 저장
    // 여기서는 "runtimeSec > 0" 과 "solution이 비어있지 않음" 등을 체크
    if(!solution.empty()) {
        // 정상 해답 있다고 가정
        *out_machines = new std::vector<Machine>();
        (*out_machines)->assign(solution.begin(), solution.end());
        return 1; // success
    } else {
        // 해답 없음
        // 기존에 counter++ 하고 끝내던 로직
        counter++;
        *out_machines = new std::vector<Machine>();
        return 0; // fail
    }
}
