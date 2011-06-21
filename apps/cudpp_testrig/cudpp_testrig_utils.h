// -------------------------------------------------------------
// cuDPP -- CUDA Data Parallel Primitives library
// -------------------------------------------------------------
// $Revision: $
// $Date: $
// ------------------------------------------------------------- 
// This source code is distributed under the terms of license.txt in
// the root directory of this source distribution.
// ------------------------------------------------------------- 

/**
 * @file
 * cudpp_testrig_utils.h
 *
 */

#ifndef __CUDPP_TESTRIG_UTILS_H__
#define __CUDPP_TESTRIG_UTILS_H__

#define NO_MINMAX
#include <math.h>
#include <cstdio>
#include <climits>
#include <float.h>
#include <algorithm>


// template specializations defined below after class definitions

template <typename T>
class VectorSupport
{
public:
    static void fillVector(T *a, size_t numElements, T info);
    //! @todo should this really be T info? instead unsigned int or float? 
    static int  verifySort(T *keysSorted, unsigned int *valuesSorted, T *keysUnsorted, size_t len);
};

template <typename T>
class OperatorAdd
{
public:
    T operator()(const T& a, const T& b) { return a + b; }
    T identity() { return (T)0; }
};

template <typename T>
class OperatorMultiply
{
public:
    T operator()(const T& a, const T& b) { return a * b; }
    T identity() { return (T)1; }
};

template <typename T>
class OperatorMax
{
public:
    T operator() (const T& a, const T& b) const { return std::max<T>(a, b); }
    T identity() const { return (T)0; }
};

template <typename T>
class OperatorMin
{
public:
    T operator() (const T& a, const T& b) const { return std::min<T>(a, b); }
    T identity() const { return (T)0; }
};

// specializations
template <> inline
int OperatorMax<int>::identity() const { return INT_MIN; }
template <> inline
unsigned int OperatorMax<unsigned int>::identity() const { return 0; }
template <> inline
float OperatorMax<float>::identity() const { return -FLT_MAX; }
template <> inline
double OperatorMax<double>::identity() const { return -DBL_MAX; }
template <> inline
long long OperatorMax<long long>::identity() const { return LLONG_MIN; }
template <> inline
unsigned long long OperatorMax<unsigned long long>::identity() const { return 0; }

template <> inline
int OperatorMin<int>::identity() const { return INT_MAX; }
template <> inline
unsigned int OperatorMin<unsigned int>::identity() const { return UINT_MAX; }
template <> inline
float OperatorMin<float>::identity() const { return FLT_MAX; }
template <> inline
double OperatorMin<double>::identity() const { return DBL_MAX; }
template <> inline
long long OperatorMin<long long>::identity() const { return LLONG_MAX; }
template <> inline
unsigned long long OperatorMin<unsigned long long>::identity() const { return ULLONG_MAX; }
    

// "info" is the number of key bits
template<> inline
void VectorSupport<unsigned int>::fillVector(unsigned int *a, size_t numElements, unsigned int keybits)
{
    // Fill up with some random data
    int keyshiftmask = 0;
    if (keybits > 16) keyshiftmask = (1 << (keybits - 16)) - 1;
    int keymask = 0xffff;
    if (keybits < 16) keymask = (1 << keybits) - 1;

    srand(95123);
    for(unsigned int i=0; i < numElements; ++i)   
    { 
        a[i] = ((rand() & keyshiftmask)<<16) | (rand() & keymask); 
    }
}

// "info" is the number of key bits
template<> inline
void VectorSupport<unsigned long long>::fillVector(unsigned long long *a, size_t numElements, unsigned long long keybits)
{
    // Fill up with some random data
    unsigned long long keyshiftmask16 = 0;
    if (keybits > 16) keyshiftmask16 = ((1 << (keybits - 16)) - 1) & 0xffff;
    unsigned long long keyshiftmask32 = 0;
    if (keybits > 32) keyshiftmask32 = ((1 << (keybits - 32)) - 1) & 0xffff;
    unsigned long long keyshiftmask48 = 0;
    if (keybits > 48) keyshiftmask48 = ((1 << (keybits - 48)) - 1) & 0xffff;
    unsigned long long keymask = 0xffff;
    if (keybits < 16) keymask = (1 << keybits) - 1;

    srand(95123);
    for(unsigned int i=0; i < numElements; ++i)   
    { 
        a[i] =
          ((rand() & keyshiftmask48)<<48) |
          ((rand() & keyshiftmask32)<<32) |
          ((rand() & keyshiftmask16)<<16) |
          (rand() & keymask); 
    }
}

// "info" is the number of key bits
template<> inline
void VectorSupport<int>::fillVector(int *a, size_t numElements,int keybits)
{
    VectorSupport<unsigned int>::fillVector((unsigned int *)a, 
                                            numElements, 
                                            keybits);
}

// "info" is the number of key bits
template<> inline
void VectorSupport<long long>::fillVector(long long *a, size_t numElements,long long keybits)
{
    VectorSupport<unsigned long long>::fillVector((unsigned long long *)a, 
                                                  numElements, 
                                                  keybits);
}

// "info" is the range
template<> inline
void VectorSupport<float>::fillVector(float *a, size_t numElements, float range)
{
    srand(95123);
    for(unsigned int j = 0; j < numElements; j++)
    {
        a[j] = pow(-1, (float)j) * (range * (rand() / (float)RAND_MAX));
    }
}

// "info" is the range
template<> inline
void VectorSupport<double>::fillVector(double *a, size_t numElements, double range)
{
    srand(95123);
    for(unsigned int j = 0; j < numElements; j++)
    {
        a[j] = pow(-1, (double)j) * (range * (rand() / (double)RAND_MAX));
    }
}

// assumes the values were initially indices into the array, for simplicity of 
// checking correct order of values
template<> inline
int VectorSupport<unsigned int>::verifySort(unsigned int *keysSorted, unsigned int *valuesSorted, 
                                            unsigned int *keysUnsorted, size_t len)
{
    int retval = 0;

    for(unsigned int i=0; i<len-1; ++i)
    {      
        if( (keysSorted[i])>(keysSorted[i+1]) )
        {
            printf("Unordered key[%u]:%u > key[%u]:%u\n", i, keysSorted[i], i+1, keysSorted[i+1]);
            retval = 1;
            break;
        }               
    }

    if (valuesSorted)
    {
        for(unsigned int i=0; i<len; ++i)
        {
            if( keysUnsorted[(valuesSorted[i])] != keysSorted[i] )
            {
                printf("Incorrectly sorted value[%u] (%u) %u != %u\n", 
                    i, valuesSorted[i], keysUnsorted[valuesSorted[i]], keysSorted[i]);
                retval = 1;
                break;
            }
        }
    }

    return retval;
}

template<> inline
int VectorSupport<float>::verifySort(float *keysSorted, unsigned int *valuesSorted, 
                                     float *keysUnsorted, size_t len)
{
    int retval = 0;

    for(unsigned int i=0; i<len-1; ++i)
    {      
        if( (keysSorted[i])>(keysSorted[i+1]) )
        {
            printf("Unordered key[%u]:%f > key[%u]:%f\n", i, keysSorted[i], i+1, keysSorted[i+1]);
            retval = 1;
            break;
        }               
    }

    if (valuesSorted)
    {
        for(unsigned int i=0; i<len; ++i)
        {
            if( keysUnsorted[(valuesSorted[i])] != keysSorted[i] )
            {
                printf("Incorrectly sorted value[%u] (%u) %f != %f\n", 
                    i, valuesSorted[i], keysUnsorted[valuesSorted[i]], keysSorted[i]);
                retval = 1;
                break;
            }
        }
    }

    return retval;
}

// assumes the values were initially indices into the array, for simplicity of 
// checking correct order of values
template<> inline
int VectorSupport<int>::verifySort(int *keysSorted, unsigned int *valuesSorted, 
                                   int *keysUnsorted, size_t len)
{
    int retval = 0;

    for(unsigned int i=0; i<len-1; ++i)
    {      
        if( (keysSorted[i])>(keysSorted[i+1]) )
        {
            printf("Unordered key[%u]:%d > key[%u]:%d\n", i, keysSorted[i], i+1, keysSorted[i+1]);
            retval = 1;
            break;
        }               
    }

    if (valuesSorted)
    {
        for(unsigned int i=0; i<len; ++i)
        {
            if( keysUnsorted[(valuesSorted[i])] != keysSorted[i] )
            {
                printf("Incorrectly sorted value[%u] (%u) %d != %d\n", 
                    i, valuesSorted[i], keysUnsorted[valuesSorted[i]], keysSorted[i]);
                retval = 1;
                break;
            }
        }
    }

    return retval;
}

template<> inline
int VectorSupport<double>::verifySort(double *keysSorted, unsigned int *valuesSorted, 
                                      double *keysUnsorted, size_t len)
{
    int retval = 0;

    for(unsigned int i=0; i<len-1; ++i)
    {      
        if( (keysSorted[i])>(keysSorted[i+1]) )
        {
            printf("Unordered key[%u]:%f > key[%u]:%f\n", i, keysSorted[i], i+1, keysSorted[i+1]);
            retval = 1;
            break;
        }               
    }

    if (valuesSorted)
    {
        for(unsigned int i=0; i<len; ++i)
        {
            if( keysUnsorted[(valuesSorted[i])] != keysSorted[i] )
            {
                printf("Incorrectly sorted value[%u] (%u) %f != %f\n", 
                    i, valuesSorted[i], keysUnsorted[valuesSorted[i]], keysSorted[i]);
                retval = 1;
                break;
            }
        }
    }

    return retval;
}



#endif // __CUDPP_TESTRIG_UTILS_H__
