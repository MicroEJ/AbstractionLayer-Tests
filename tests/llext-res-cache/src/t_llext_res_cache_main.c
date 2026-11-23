/*
 * C
 *
 * Copyright 2026 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 * Build: 7E4D1F7C
 */

#include "t_llext_res_cache.h"
#include "t_llext_res_utils.h"
#include "embUnit.h"
#include "u_print.h"

#define LLEXT_RES_CACHE_TEST_VERSION "v1.0.0"

void T_LLEXT_RES_CACHE_main(void) {
	UTIL_print_string("\nT_LLEXT_RES_CACHE " LLEXT_RES_CACHE_TEST_VERSION "\n");
	TestRunner_start();
	TestRunner_runTest(T_LLEXT_RES_CACHE_tests());
#if defined(LLEXT_RES_CACHE_ENABLE_STATS)
	TestRunner_runTest(T_LLEXT_RES_CACHE_stats_tests());
#endif
	TestRunner_runTest(T_LLEXT_RES_UTILS_tests());
	TestRunner_end();
}
