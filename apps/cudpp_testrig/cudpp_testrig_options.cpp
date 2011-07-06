// -------------------------------------------------------------
// cuDPP -- CUDA Data Parallel Primitives library
// -------------------------------------------------------------
// $Revision$
// $Date$
// ------------------------------------------------------------- 
// This source code is distributed under the terms of license.txt
// in the root directory of this source distribution.
// ------------------------------------------------------------- 
#include "cudpp_testrig_options.h"

#define CUDPP_APP_COMMON_IMPL
#include "commandline.h"

using namespace cudpp_app;

/**
 * Sets "global" options in testOptions given command line 
 *  -debug: sets bool <var>debug</var>. Usage is application-dependent.
 *  -op=OP: sets char * <var>op</var> to OP
 *  -iterations=#: sets int <var>numIterations</var> to #
 *  -dir=<path>: sets the search path for cudppRand test inputs
 */
void setOptions(int argc, const char **argv, testrigOptions &testOptions)
{
    testOptions.debug = false;
    commandLineArg(testOptions.debug, argc, argv, "debug");

    testOptions.algorithm = "scan";
    if (checkCommandLineFlag(argc, argv, "segscan"))
        testOptions.algorithm = "segscan";
    else if (checkCommandLineFlag(argc, argv, "multiscan"))
        testOptions.algorithm = "multiscan";
    else if (checkCommandLineFlag(argc, argv, "compact"))
        testOptions.algorithm = "compact";
    else if (checkCommandLineFlag(argc, argv, "sort"))
        testOptions.algorithm = "sort";
    else if (checkCommandLineFlag(argc, argv, "reduce"))
        testOptions.algorithm = "reduce";
    else if (checkCommandLineFlag(argc, argv, "spmv"))
        testOptions.algorithm = "spmv";
    else if (checkCommandLineFlag(argc, argv, "rand"))
        testOptions.algorithm = "rand";
            
    testOptions.op = "sum";
    commandLineArg(testOptions.op, argc, argv, "op");
    
    testOptions.numIterations = numTestIterations;
    commandLineArg(testOptions.numIterations, argc, argv, "iterations");
    
    testOptions.dir = "";
    commandLineArg(testOptions.dir, argc, argv, "dir");
}

// Leave this at the end of the file
// Local Variables:
// mode:c++
// c-file-style: "NVIDIA"
// End:
