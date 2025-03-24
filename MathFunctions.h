#ifndef MATHFUNCTIONS_H_
#define MATHFUNCTIONS_H_

#include <cmath>
#include <cfloat>
#include "Job.h"

class MathFunctions {
public:

    static void copyVector(const float* _a, float* _b, const int _dims) {
        for(int i = 0; i < _dims; i++) {
            _b[i] = _a[i];
        }
    }

    static void printVectors(std::vector<Job>* jobs) {
        int dimensions = jobs->at(0).getDimensions();
        //   Add a comment to this line
        for(int j = 0; j < jobs->size(); j++) {
            printf("Job %.3d id: %d vec: (", j, jobs->at(j).getID());
            for(int k = 0; k < dimensions; k++) {
                printf("%f ", jobs->at(j).getKPIVec()[k]);
            }
            printf(")\n");
        }
    }

    static void VectorAddition(float* _a, const float* _b, const int _dims) {
        for(int i = 0; i < _dims; i++) {
            _a[i] += _b[i];
        }
    }

    static void VectorSubtraction(float* _a, const float* _b, const int _dims) {
        for(int i = 0; i < _dims; i++) {
            _a[i] -= _b[i];
        }
    }

    static void VecDiv(float* _a, const float _b, const int _dims) {
        if(_b != 0) {
            for(unsigned i = 0; i < _dims; i++) {
                _a[i] /= _b;
            }
        }
    }

    static float TightFillSum(const float* _weight, const float* _a, const float* _b, const int _dims) {
        float sum = 0.;
        for(unsigned i = 0; i < _dims; i++) {
            sum += _weight[i] * _a[i] / _b[i];
        }
        return sum;
    }

    static float TightFillMin(const float* _weight, const float* _a, const float* _b, const int _dims) {
        float min = FLT_MAX;
        float component = -1;
        for(unsigned i = 0; i < _dims; i++) {
            component = _weight[i] * _a[i] / _b[i];
            if(component < min) {
                min = component;
            }
        }
        return min;
    }

    static bool areVectorsEqual(const float* _a, const float* _b, const int _dims) {
        for(int i = 0; i < _dims; i++) {
            if(fabs(_a[i] - _b[i]) > 0.0000001) {
                return false;
            }
        }
        return true;
    }

    static void scaleVector(const float _lambda, float* _vector, const int _dims) {
        for(int i = 0; i < _dims; i++) {
            _vector[i] *= _lambda;
        }
    }

    static bool areFloatsEqual(const float _a, const float _b) {
        return fabs(_a - _b) < 0.0000001;
    }

    static float magnitude(float* _a, int _dims) {
        float magValue = 0;
        for(unsigned i = 0; i < _dims; i++) {
            if(_a[i] < 0) {
                return 1000;
            } else {
                magValue += _a[i] * _a[i];
            }
        }
        return std::sqrt(magValue);
    }
    //compute the sin value of the angle between two vectors

    static float Sin(float* _a, float* _b, int _dims) {
        float dot = DotProd(_a, _b, _dims);
        float magA = magnitude(_a, _dims);
        float magB = magnitude(_b, _dims);
        double cosTheta = dot / (magA * magB);
        double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
        //	printf("The sin value of <%f, %f> and <%f, %f> is %lf\n", _a[0], _a[1], _b[0], _b[1], sinTheta);
        // Ensure the denominators are non-zero to avoid division by zero
        return sinTheta;
    }

    static float DotProd(float* _a, float* _b, float* _c, int _dims) {
        float res = 0.0;
        for(unsigned i = 0; i < _dims; i++) {
            res += _a[i] * _b[i] * _c[i];
        }
        return res;
    }

    static float DotProd(float* _a, float* _b, int _dims) {
        float res = 0.0;
        for(unsigned i = 0; i < _dims; i++) {
            res += _a[i] * _b[i];
        }
        return res;
    }
    
    static float NormDotProd(float* _w, float* _a, float* _b, float* _aSum, float* _bSum, int _dims) {
        float res = 0.0;
        for(unsigned i = 0; i < _dims; i++) {
            res += _w[i] * (_a[i] / _aSum[i]) * (_b[i] / _bSum[i]);
        }
        return res;        
    }

    static float L2(float* _a, int _dims) {
        return sqrt(DotProd(_a, _a, _dims));
    }

    static float L2(float* _weight, float* _a, int _dims) {
        return sqrt(DotProd(_weight, _a, _a, _dims));
    }

    static float* VecCoordProd(float* _res, float* _a, const float* _b, int _dims) {
        for(unsigned i = 0; i < _dims; i++) {
            _res[i] = _a[i] * _b[i];
        }
        return _res;
    };

    /**
     * Vector b is subtracted from vector a. If any component is smaller than 0
     * (or rather -0.00001 to tolerate rounding effects), this function returns
     * false, otherwise it returns true.
     * @param a first vector (usually: remaining capacity of machine).
     * @param b second vector (usually: job requirements).
     * @param _dims (dimension of vectors).
     * @return true if and only if job fits into machine.
     */
    static bool isVectorDifferencePositive(float* a, const float* b, int _dims) {
        float t;
        for(int i = 0; i < _dims; i++) {
            t = a[i] - b[i];
            if(t < -0.00001) { // not 0 because of rounding effects
                return false;
            }
        }
        return true;
    };

    static float Linf(float* vec, int _dims) {
        float max = -FLT_MAX;
        for(unsigned i = 0; i < _dims; ++i) {
            if(max < vec[i]) {
                max = vec[i];
            }
        }
        return max;
    };

    static float LinfPercentage(float* vec, float* vec2, int _dims) {
        float max = -FLT_MAX;
        float result = -1;
        for(unsigned i = 0; i < _dims; ++i) {
            if(max < vec[i]) {
                max = vec[i];
                result = vec[i] / vec2[i];
            }
        }
        return result;
    };

    static bool LInfDecSort(Job &a, Job &b) {
        int dims = a.getDimensions();
        return (Linf(a.getKPIVec(), dims) > Linf(b.getKPIVec(), dims));
    };

    static float IntervalRounding(float _left, float _num, bool _leftOpen, float _epsilon) {
        if(_leftOpen) {
            if((_left < _num) && (_num <= ((1.0 + _epsilon) * _left))) {
                return (_left);
            } else {
                return IntervalRounding(((1.0 + _epsilon) * _left), _num, true, _epsilon);
            }
        } else {
            if((_left <= _num) && (_num < ((1.0 + _epsilon) * _left))) {
                return (_left);
            } else {
                return IntervalRounding(((1.0 + _epsilon) * _left), _num, false, _epsilon);
            }
        }
    };

    /**
     * This function returns true if at least one coordinate of the given vector is negative.
     * @param _vec vector that is checked.
     * @param _dimensions dimensions of vector.
     * @return true if at least one coordinate is negative, false otherwise.
     */
    static bool isVectorNegative(const float* _vec, int _dimensions) {
        for(unsigned i = 0; i < _dimensions; ++i) {
            if(_vec[i] < 0.00001) {
                return true;
            }
        }
        return false;
    }

    /**
     * This function sums up the components of the given vector and returns the sum.
     * @param v vector of floats.
     * @param length length of vector.
     * @return sum of vector components.
     */
    static float sum(float* v, int length) {
        float res = 0.;
        for(int i = 0; i < length; i++) {
            res += v[i];
        }
        return res;
    }

    static float min(float* v, int dimensions) {
        float min = FLT_MAX;
        for(int i = 0; i < dimensions; i++) {
            if(v[i] < min)
                min = v[i];
        }
        return min;
    }

    static float max(float* v, int dimensions) {
        float max = -FLT_MAX;
        for(int i = 0; i < dimensions; i++) {
            if(v[i] > max)
                max = v[i];
        }
	//printf("v0 is %lf , v1 is %lf max is %lf\n",v[0],v[1],max);
        return max;
    }

    static float costFunction(float* v, int norm, int dimensions, float* weight) {
        float weighted[dimensions] = {0.0};
        for(unsigned i = 0; i < dimensions; i++)
            weighted[i] = v[i] * weight[i];
        switch(norm) {
            case 2:
                return sum(weighted, dimensions);
                break;
            case 3:
                return L2(weighted, dimensions);
                break;
            case 1:
                return max(weighted, dimensions);
                break;
        }
        return 0;
    }

    static float costFunction(float* v, int norm, int dimensions) {
        switch(norm) {
            case 2:
                return sum(v, dimensions);
                break;
            case 3:
                return L2(v, dimensions);
                break;
            case 1:
                return max(v, dimensions);
                break;
        }
        return 0;
    }
};
#endif
