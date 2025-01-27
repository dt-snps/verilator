// -*- mode: C++; c-file-style: "cc-mode" -*-
//*************************************************************************
//
// Copyright 2020 by Wilson Snyder and Marlon James. This program is free software; you can
// redistribute it and/or modify it under the terms of either the GNU
// Lesser General Public License Version 3 or the Perl Artistic License
// Version 2.0.
// SPDX-License-Identifier: LGPL-3.0-only OR Artistic-2.0
//
//*************************************************************************

#include VM_PREFIX_INCLUDE

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

bool got_error = false;

// Use cout to avoid issues with %d/%lx etc
#define CHECK_RESULT(got, exp) \
    if ((got) != (exp)) { \
        std::cout << std::dec << "%Error: " << __FILE__ << ":" << __LINE__ << ": GOT = " << (got) \
                  << "   EXP = " << (exp) << std::endl; \
        got_error = true; \
    }
#define CHECK_RESULT_CSTR(got, exp) \
    if (strcmp((got), (exp))) { \
        printf("%%Error: %s:%d: GOT = '%s'   EXP = '%s'\n", __FILE__, __LINE__, \
               (got) ? (got) : "<null>", (exp) ? (exp) : "<null>"); \
        return __LINE__; \
    }

vluint64_t main_time = 0;
#ifdef T_WRAPPER_LEGACY
#elif defined(T_WRAPPER_LEGACY_TIME64)
vluint64_t vl_time_stamp64() { return main_time; }
#elif defined(T_WRAPPER_LEGACY_TIMED)
double sc_time_stamp() { return main_time; }
#endif

int main(int argc, char** argv, char** env) {
    // Test that the old non-context Verilated:: calls all work
    // (This test should never get updated to use context)

    // Many used only by git@github.com:djg/verilated-rs.git

    Verilated::commandArgs(argc, argv);  // Commonly used
    CHECK_RESULT_CSTR(Verilated::commandArgsPlusMatch("not-matching"), "");

    const char* argadd[] = {"+testingPlusAdd+2", nullptr};
    Verilated::commandArgsAdd(1, argadd);
    CHECK_RESULT_CSTR(Verilated::commandArgsPlusMatch("testingPlusAdd"), "+testingPlusAdd+2");

    Verilated::assertOn(true);
    CHECK_RESULT(Verilated::assertOn(), true);

    Verilated::calcUnusedSigs(true);
    CHECK_RESULT(Verilated::calcUnusedSigs(), true);

    Verilated::debug(9);  // Commonly used
    CHECK_RESULT(Verilated::debug(), 9);
    Verilated::debug(0);

    Verilated::errorLimit(2);
    CHECK_RESULT(Verilated::errorLimit(), 2);

    Verilated::fatalOnError(true);
    CHECK_RESULT(Verilated::fatalOnError(), true);

    Verilated::fatalOnVpiError(true);
    CHECK_RESULT(Verilated::fatalOnVpiError(), true);

    Verilated::gotError(false);
    CHECK_RESULT(Verilated::gotError(), false);

    Verilated::gotFinish(false);
    CHECK_RESULT(Verilated::gotFinish(), false);  // Commonly used

    Verilated::mkdir(VL_STRINGIFY(TEST_OBJ_DIR) "/mkdired");

    Verilated::randReset(0);
    CHECK_RESULT(Verilated::randReset(), 0);

    Verilated::randSeed(1234);
    CHECK_RESULT(Verilated::randSeed(), 1234);

    Verilated::traceEverOn(true);  // Commonly used

    CHECK_RESULT_CSTR(Verilated::productName(), Verilated::productName());
    CHECK_RESULT_CSTR(Verilated::productVersion(), Verilated::productVersion());

    CHECK_RESULT(Verilated::timeunit(), 12);
    CHECK_RESULT(Verilated::timeprecision(), 12);

    VM_PREFIX* topp = new VM_PREFIX();

    topp->eval();
    topp->clk = 0;

    VL_PRINTF("Starting\n");

    vluint64_t sim_time = 100;
    while (
#ifdef T_WRAPPER_LEGACY
        Verilated::time()
#else
        vl_time_stamp64()
#endif
            < sim_time
        && !Verilated::gotFinish()) {
        CHECK_RESULT(VL_TIME_Q(), main_time);
        CHECK_RESULT(VL_TIME_D(), main_time);

        main_time += 1;
#ifdef T_WRAPPER_LEGACY
        Verilated::timeInc(1);
        // Check reading and writing of time
        Verilated::time(Verilated::time());
#endif

        topp->clk = !topp->clk;
        topp->eval();
    }

    topp->final();
    Verilated::flushCall();
    Verilated::runFlushCallbacks();

    Verilated::internalsDump();
    Verilated::scopesDump();

    VL_DO_DANGLING(delete topp, topp);
    Verilated::runExitCallbacks();
    return got_error ? 10 : 0;
}
