/*
 * C
 *
 * Copyright 2026 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 * Build: 7E4D1F7C
 */

#ifndef T_LLEXT_RES_CACHE_H
#define T_LLEXT_RES_CACHE_H
#include "embUnit.h"
#include "LLEXT_RES_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief this function is the entry point for the LLEXT_RES_CACHE Test Suite.
 */
void T_LLEXT_RES_CACHE_main(void);

TestRef T_LLEXT_RES_CACHE_tests(void);

#if defined(LLEXT_RES_CACHE_ENABLE_STATS)
TestRef T_LLEXT_RES_CACHE_stats_tests(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
