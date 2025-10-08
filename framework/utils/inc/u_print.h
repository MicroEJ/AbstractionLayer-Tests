/*
 * Copyright 2019-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Prevent recursive inclusion */

#ifndef UTIL_PRINT_H
#define UTIL_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// Include VEE Port User configuration file
#if defined __has_include
	#if __has_include("veeport_configuration.h")
		#include "veeport_configuration.h"
	#endif // __has_include("veeport_configuration.h")
#else
// Ensure 'veeport_configuration.h' exists in your project for custom configurations.
	#include "veeport_configuration.h"
#endif // defined __has_include

#include <stdint.h>

// -----------------------------------------------------------------------------
// Public Macros and Defines
// -----------------------------------------------------------------------------

/**
 * @brief Print function redirection macro to allow the test developers to use
 * the printf function or equivalent.
 *
 * Redefine this macro in veeport_configuration.h to target the desired print function if necessary.
 */
#ifndef UTIL_printf
#define UTIL_printf printf
#endif

// -----------------------------------------------------------------------------
// Public constant declaration
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Public structure declaration
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Public function declaration
// -----------------------------------------------------------------------------

// weak functions to implement
/**
 * @fn void UTIL_print_initialize(void)
 * @brief This function is called before the tests which need to display some results. It can be used to initialize the
 * hardware in charge of the display (serial line, screen,...).
 *
 * @fn void UTIL_print_string(const char* string)
 * @brief  this function is called by test functions to display a string.
 * @param[in] const char* string  - the string to display.
 *
 * @fn void UTIL_print_integer(int integer)
 * @brief  this function is called by test functions to display a integer value.
 * @param[in] int integer - the value to display.
 *
 * @fn void UTIL_print_float(double value)
 * @brief this function is called by test functions to display a double value.
 * @param[in] double value - the doublye precision value to display.
 *
 * @fn void UTIL_print_longlong(long long value)
 *.@brief this function is called by test functions to display a 64-bit precision value.
 * @param[in] long long value - the 64-bit integer value to display.
 */
void UTIL_print_initialize(void);
void UTIL_print_string(const char *string);
void UTIL_print_integer(int integer);
void UTIL_print_float(double value);
void UTIL_print_longlong(long long value);

#ifdef __cplusplus
}
#endif

#endif // UTIL_PRINT_H

// -----------------------------------------------------------------------------
// End
// -----------------------------------------------------------------------------
