/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <stdio.h>
#include <stdarg.h>
#include "t_llkernel.h"
#include "embUnit.h"
#include "u_print.h"

#define LLKERNEL_VERSION "v1.2.0"

void T_LLKERNEL_main(void) {
    UTIL_print_string("\nT_LLKERNEL " LLKERNEL_VERSION "\n");
	TestRunner_start();
	TestRunner_runTest(T_LLKERNEL_tests());
	TestRunner_end();
	return;
}
