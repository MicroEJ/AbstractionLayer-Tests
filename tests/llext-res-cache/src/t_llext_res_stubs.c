/*
 * C
 *
 * Copyright 2026 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 *
 * Build: 7E4D1F7C
 */

/**
 * @file t_llext_res_stubs.c
 * @brief Strong overrides of weak VEE Port error helpers for unit tests.
 *
 * The production implementations in veeport_errors.c call SNI_throwNativeException,
 * which requires a Java VM context that is not available in the embUnit test harness.
 * When SNI is not available, the production helpers assert(false), which aborts the
 * test before the tested function can return its documented error code.
 *
 * These strong overrides replace the weak production symbols so the tested functions
 * return normally, allowing embUnit assertions to validate the return value contract.
 */

#include <stdint.h>

void VEEPORT_ERRORS_throw_runtime_exception(int32_t error_code) {
	(void)error_code;
}

void VEEPORT_ERRORS_throw_runtime_exception_message(int32_t error_code, const char *format, ...) {
	(void)error_code;
	(void)format;
}

void VEEPORT_ERRORS_throw_caught_exception(int32_t error_code) {
	(void)error_code;
}

void VEEPORT_ERRORS_throw_caught_exception_message(int32_t error_code, const char *format, ...) {
	(void)error_code;
	(void)format;
}
