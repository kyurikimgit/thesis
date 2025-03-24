#ifndef MATRIX_H_
#define MATRIX_H_

//#include <boost/heap/fibonacci_heap.hpp>
//#include "Pani.h"
#include <limits.h>

/**
 * This class is a data structure for one implementation of the Hybrid Heuristic (in Pani.h). 
 * It represents the job-bin pairs as nodes in a matrix of nodes.
 * The pairs are also added to a Fibonacci heap which probably makes searches for the best pair quicker,
 * namely in O(log n) while an (adapted) matrix alone would do it in O(m), I think. (The adapted matrix would store
 * each column's best pair in the column head, and the algorithm would go through the column heads to determine
 * the best one. The column heads, however, must be kept up-to-date which could make this solution more expensive.)
 */
class Matrix {

private:

    /**
     * Struct representing a single job-bin pair called node.
     * The helper nodes of the 0th row and 0th column are for maintenance and do not represent job-bin pairs.
     */
    typedef struct node {
        // Job and bin of job-bin pair
        int jobIndex = -1;
        int binIndex = -1;
        // Previous and next job; note that the previous job of the 0th node is the last node in the row
        int previousJob = -1; 
        int nextJob = -1;
        // Previous and next bin
        int previousBin = -1;
        int nextBin = -1;
        // Value of job-bin pair (not used by helper nodes)
        float value = -1.;
        // Count of bins that can still fit job (only used by helper nodes in 0th column)
        int counter = 0;
//        int type = -1;
    } Node;

    /**
     * Struct for comparing job-bin pairs (i.e. nodes) by their values.
     */
    typedef struct compareNode {
        bool operator()(const Node& n1, const Node& n2) const {
/*            if(n1.type == 3) { // TODO: Pani::DP cannot be used here; fix that if possible
                if(n1.binIndex != n2.binIndex && n1.jobIndex == n2.jobIndex) {
                    // TODO: DP case not correct; it is tricky!
                    return n1.value < n2.value;
                }
            }*/
            return n1.value > n2.value;
        }
    } CompareNode;

    // Number of jobs
    int n = -1;
    // Number of bins
    int m = -1;
    // Dimensions of vectors
    int d = -1;
    // Type of norm
//    int t = -1;
    // Matrix of nodes
    Node** nodes;
    // Helper array of length n (for picking new assignment, fields are initialised with job counters for each job in question which are decremented whenever bin favours job)
    int* hCount;
    // Helper array of length n (for picking new assignment, fields store best value for each job in question)
    float* hValue;
    // Helper array of length n (for picking new assignment, fields store best bin for each job in question)
    int* hBest;
/*
    // Fibonacci heap (called PQ in comments)
    boost::heap::fibonacci_heap<Node, boost::heap::compare<CompareNode>> heap;
    // Handles for removal of nodes from PQ
    boost::heap::fibonacci_heap<Node, boost::heap::compare<CompareNode>>::handle_type** handles;
 */

    /**
     * Create nodes in 0th column including corner node (0, 0).
     * Note that a 0th column node does not set value because value is not used by these helper nodes.
     * It does set counter, however, because it tracks the number of bins which still can fit the job.
     */
    void createZerothColumn() {
        for(int i = 0; i <= n; i++) {
            nodes[i][0].jobIndex = i;
            nodes[i][0].binIndex = 0;
            nodes[i][0].previousJob = i-1;
            nodes[i][0].previousBin = 0;
            i < n ? nodes[i][0].nextJob = i+1 : nodes[i][0].nextJob = 0;
            nodes[i][0].nextBin = 0;
            nodes[i][0].counter = 0;
//            nodes[i][0].type = t;
        }
    }

public:

    /**
     * Constructor initialises n, m and d, the (n+1)x(m+1) nodes of the matrix and and the (n+1)x(m+1) handles for identifying nodes in the PQ.
     * Note that the handles of the first row / column will never be used.
     * @param numberJobs number of jobs n.
     * @param numberBins number of bins m.
     * @param dimensions number of dimensions d.
     */
    Matrix(const int numberJobs, const int numberBins, const int dimensions) { //, const int type) {
        // Number of jobs and bins, dimensions
        n = numberJobs;
        m = numberBins;
        d = dimensions;
//        t = type;
        // Initialise matrix and handles for nodes stored in PQ
        nodes = new Node*[numberJobs+1];
        //handles = new boost::heap::fibonacci_heap<Node, boost::heap::compare<CompareNode>>::handle_type*[numberJobs+1];
        for(int i=0; i<numberJobs+1; i++) {
            nodes[i] = new Node[numberBins+1];
            //handles[i] = new boost::heap::fibonacci_heap<Node, boost::heap::compare<CompareNode>>::handle_type[numberBins+1];
        }
        hCount = new int[n+1];
        hValue = new float[n+1];
        hBest = new int[n+1];

        createZerothColumn();
    };
    
    ~Matrix() {
        for(int i=0; i<n+1; i++) {
            delete [] nodes[i];
            //delete [] handles[i];
        }
        delete [] nodes;
        //delete [] handles;
        delete [] hCount;
        delete [] hValue;
        delete [] hBest;
    }

    /**
     * Create node in 0th row for new bin.
     * Note that this is a helper node, not representing a job-bin pair.
     * Note that it does not set value or counter because it does not use them.
     * @param bin index of column.
     * @return next (i.e. first) active job (which is a candidate for the bin if it fits).
     */
    int createHeadNodeInColumn(const int bin) {
        return createNodeInColumn(0, -1, bin, -1.);
    }
    
    /**
     * Create node in column which is either a head node (job == 0) or a job-bin pair (job > 0).
     * @param job job (0 <= job <= n).
     * @param previousJob previous job in bin (must be -1 for head node, i.e. node with job == 0).
     * @param bin bin (1 <= bin <= m).
     * @param value value of job-bin pair (head node stores best value).
     * @return next active job (which is a candidate for the bin if it fits).
     */
    int createNodeInColumn(const int job, const int previousJob, const int bin, const float value) {
        Node* node = &nodes[job][bin];
        node->jobIndex = job;
        node->binIndex = bin;
        node->nextBin = -1;
        node->previousJob = previousJob;
        node->nextJob = -1;
//        node->type = t;
        if(job == 0) {
            node->previousBin = bin-1;
            nodes[0][bin-1].nextBin = bin;
            node->counter = -1; // saves best job for this bin
            node->value = FLT_MAX; // saves value of best job-bin pair for this bin
        } else {
            const int lastBinPrior = nodes[job][0].previousBin;
            nodes[previousJob][bin].nextJob = job;
            nodes[job][lastBinPrior].nextBin = bin;
            node->previousBin = lastBinPrior;
            nodes[job][0].previousBin = bin;
            node->value = value;
            // If value is better than best value for bin, then save this as best bin
            if(value < nodes[0][bin].value) {
                nodes[0][bin].value = value;
                nodes[0][bin].counter = job; // saves best job for this bin
            }
            nodes[job][0].counter++;
//            // Add pair to PQ and store its handle (so that it can be erased / updated later)
//            handles[job][bin] = heap.push(*node); // TODO
        }
        return nodes[job][0].nextJob;
    }
    
    /**
     * Updates value of job-bin pair in matrix and in PQ (like decreaseKey / increaseKey) and returns next job in that bin.
     * @param job job of job-bin pair (1 <= job <= n).
     * @param bin bin of job-bin pair (1 <= bin <= m).
     * @param value new value.
     * @return next job in given bin.
     */
    int updateNodeInColumn(const int job, const int bin, const float value) {
        // Decrease key (simple update after changing value)
        nodes[job][bin].value = value;
//        heap.update(handles[job][bin], nodes[job][bin]); // TODO
        // If value is better than best value for bin, then save this as best bin
        if(value < nodes[0][bin].value) {
            nodes[0][bin].value = value;
            nodes[0][bin].counter = job; // saves best job for this bin
        }
        return nodes[job][bin].nextJob;
    }    

    /**
     * Returns next active job (in any bin).
     * Note that the 0th column does not actually represent a bin, but contains all active jobs.
     * @param job job.
     * @return next active job.
     */
    int getNextJob(const int job) {
        return getNextJob(job, 0);
    }

    /**
     * Returns the (active) job following the given (active) job in the given bin.
     * Note that active jobs are not yet assigned. Here, a job being in a bin means that it would still fit and could be assigned to it.
     * @param job job.
     * @param bin bin.
     * @return next job in bin.
     */
    int getNextJob(const int job, const int bin) {
        return nodes[job][bin].nextJob;
    }
    
    /**
     * Get count of bins which still consider given job.
     * @param job job.
     * @return counter for given job.
     */
    int getCounter(const int job) {
        return nodes[job][0].counter;
    }

    /**
     * Return true if and only if counter == 0 for this job.
     * @param job job.
     * @return true if counter is 0, otherwise false.
     */
    bool isCounterZero(const int job) {
        return nodes[job][0].counter == 0;
    }
    
    /**
     * Return job at the top of the queue, i.e. the job from the best job-bin pair.
     * @return job from best job-bin pair.
     */
    int getBestJob(const int bin) {
        //return heap.empty() ? -1 : heap.top().jobIndex;
        return nodes[0][bin].counter;
    }

    /**
     * Return bin with best job-bin pair. However, if any job is favoured by all bins that can still fit this job,
     * then return the best bin for that job. (If there are multiple such jobs, then the first one is used, which may
     * not be the best choice.)
     * @return bin from best job-bin pair.
     */
    int getBestBin() {
        //return heap.empty() ? -1 : heap.top().binIndex;
        // Initialise helper array 
        std::fill(hCount, hCount+(n+1), 0);
//        for(int i=0; i<n; i++) {
//            printf("%d \n", hCount[i]);
//        } 
//        std::fill(hValue, hValue+n, FLT_MAX);
//        std::fill(hBest, hBest+n, 0);
        // Initialise variables for iterating through bins and storing global optimum
        int bin = 0;
        int bestBin = -1;
        float value = FLT_MAX;
        while(nodes[0][bin].nextBin > bin) {
            bin = nodes[0][bin].nextBin;
            // If value of bin is smaller than global best so far, set value to new global best and memorise bin
            if(nodes[0][bin].value < value) {
                value = nodes[0][bin].value;
                bestBin = bin;
            }
            // Get favourite job of this bin
            int job = nodes[0][bin].counter;
            if(job == -1) {
                continue;
            }
            // If job appears for first time, initialise count, value and best bin
            if(hCount[job] == 0) {
                hCount[job] = nodes[job][0].counter;
                hValue[job] = nodes[0][bin].value;
                hBest[job] = bin;
            } else if(nodes[0][bin].value < hValue[job]) {
                // If value is better than previous value for this job, update value and memorise bin
                hValue[job] = nodes[0][bin].value;
                hBest[job] = bin;
            }
            // Reduce count for that job
            hCount[job]--;
            // If count is 0 (i.e. all bins accepting job favour job), use best job-bin pair for this job immediately (TODO: this might not be the best decision)
            if(hCount[job] == 0) {
                return hBest[job];
            }
        }
        return bestBin; 
    }
    
    /**
     * Remove job-bin pair with given job and given bin and return next job in that bin.
     * This function is called when updating a bin. Some jobs will not fit anymore and must be deleted (and the others are just updated by another function).
     * @param job job.
     * @param bin bin.
     * @return next job in given bin.
     */
    int removeNodeInColumn(const int job, const int bin) {
        //printf("MAT 440 -- Remove node: %d %d\n", job, bin);
        const int previousJob = nodes[job][bin].previousJob;
        const int nextJob = nodes[job][bin].nextJob;
        const int previousBin = nodes[job][bin].previousBin;
        const int nextBin = nodes[job][bin].nextBin;
        nodes[previousJob][bin].nextJob = nextJob;
        if(nextJob != -1) {
            nodes[nextJob][bin].previousJob = previousJob;  
        }
        nodes[job][previousBin].nextBin = nextBin;
        if(nextBin != -1) {
            nodes[job][nextBin].previousBin = previousBin;  
        } else {
            nodes[job][0].previousBin = previousBin;  
        }
        nodes[job][0].counter--;
        // Remove from PQ
//        if(job != 0 && bin != 0) {
//            heap.erase(handles[job][bin]);
//        }
        return nextJob;
    }
    
    /**
     * Remove row of given job in matrix and erase all job-bin pairs of that row in PQ. 
     * Note that the nodes are not actually deleted in the matrix, but next or previous of the neighbouring nodes are set instead.
     * @param job job whose row is to be removed from the matrix (1 <= job <= n).
     */
    void removeRow(const int job, const int bin) {
        //printf("MAT 440 -- Remove row: %d\n", job);
        int currentBin = 0;
        int previousJob = -1;
        int nextJob = -1;
        while(true) {
            previousJob = nodes[job][currentBin].previousJob;
            nextJob = nodes[job][currentBin].nextJob;
            nodes[previousJob][currentBin].nextJob = nextJob;
            if(nextJob != -1) {
                nodes[nextJob][currentBin].previousJob = previousJob;
            }
            // Remove from PQ
//            if(currentBin != 0) {
//                heap.erase(handles[job][currentBin]);
//            }            
            if(currentBin == bin) {
                // Best job was deleted; hence, counter and value of column head must be reset (they are potentially updated by another function)
                nodes[0][currentBin].counter = -1;
                nodes[0][currentBin].value = FLT_MAX;
            } else if(nodes[0][currentBin].counter == job) {
                // Job was best job of this bin, so column head must be updated with new best job
                int currentJob = 0;
                nodes[0][currentBin].counter = -1;
                nodes[0][currentBin].value = FLT_MAX;
                while(nodes[currentJob][currentBin].nextJob > currentJob) {
                    currentJob = nodes[currentJob][currentBin].nextJob;
                    if(nodes[currentJob][currentBin].value < nodes[0][currentBin].value) {
                        nodes[0][currentBin].value = nodes[currentJob][currentBin].value;
                        nodes[0][currentBin].counter = currentJob;
                    }
                }
            }
            currentBin = nodes[job][currentBin].nextBin;
            if(currentBin < 1) {
                break;
            }
        }
        // TODO: Statt alles auf -1 und FLT_MAX zu setzen, müssen alle Spalten, deren bester Job der zugewiesene ist, aktualisiert werden!!!
    }
    
//    void printX() {
//        printf("Best job in Bin 3: %d\n", nodes[0][3].counter);
//    }
    
    
    /**
     * Print all job-bin pairs in matrix row by row. For debugging.
     */
    void print() {
        
        printf("%d\n", nodes[0][1].counter);
        printf("%d\n", nodes[0][2].counter);
        printf("%d\n", nodes[0][3].counter);
        
        int currentJob = nodes[0][0].nextJob;
        while(currentJob > 0) {
            int currentBin = nodes[currentJob][0].nextBin;
            while(currentBin > 0) {
                printf("(%d %d) ", currentJob, currentBin);
                currentBin = nodes[currentJob][currentBin].nextBin;
            }
            printf("\n");
            currentJob = nodes[currentJob][0].nextJob;
        }
        printf("\n-----------\n");
    }
};



#endif
