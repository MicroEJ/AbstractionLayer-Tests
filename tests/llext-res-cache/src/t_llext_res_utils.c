/*
 * C
 *
 * Copyright 2026 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 * Build: 7E4D1F7C
 */

/**
 * @file t_llext_res_utils.c
 * @brief Test suite for LLEXT_RES_utils functions using embUnit.
 */

#include <string.h>
#include <stdint.h>

#include "t_llext_res_utils.h"
#include "embUnit.h"
#include "u_print.h"
#include "LLEXT_RES_utils.h"
#include "LLEXT_RES_errors.h"
#include "fs_configuration.h"

/* -------------------------------------------------------------------------- */
/* -                           setUp / tearDown                              - */
/* -------------------------------------------------------------------------- */

/** @brief Function called before running each test. */
static void T_LLEXT_RES_UTILS_setUp(void) {
	UTIL_print_string("\nT_LLEXT_RES_UTILS_setUp\n");
	/* Reset global state to a clean baseline */
	llext_res_g_initialized = false;
	for (int32_t i = 0; i < LLEXT_MAX_OPEN_RESOURCES; i++) {
		llext_res_g_resources[i].in_use = LLEXT_RES_SLOT_FREE;
		llext_res_g_resources[i].file_id = 0;
		llext_res_g_resources[i].file_size = 0;
		llext_res_g_resources[i].virtual_pos = 0;
	}
}

/** @brief Function called after running each test. */
static void T_LLEXT_RES_UTILS_tearDown(void) {
	UTIL_print_string("T_LLEXT_RES_UTILS_tearDown\n");
}

/* ========================================================================== */
/* =                    initialize_resources Tests                          = */
/* ========================================================================== */

/** @brief initialize_resources should mark all slots as free and set the flag. */
static void T_LLEXT_RES_UTILS_CHECK_initialize_resources(void) {
	UTIL_print_string("LLEXT_RES_UTILS initialize resources\n");

	/* Dirty the global state */
	llext_res_g_initialized = false;
	for (int32_t i = 0; i < LLEXT_MAX_OPEN_RESOURCES; i++) {
		llext_res_g_resources[i].in_use = LLEXT_RES_SLOT_USED;
		llext_res_g_resources[i].file_size = 12345;
		llext_res_g_resources[i].virtual_pos = 67890;
	}

	LLEXT_RES_utils_initialize_resources();

	TEST_ASSERT_MESSAGE(true == llext_res_g_initialized, "llext_res_g_initialized should be true after init");

	for (int32_t i = 0; i < LLEXT_MAX_OPEN_RESOURCES; i++) {
		TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_SLOT_FREE, (int)llext_res_g_resources[i].in_use);
		TEST_ASSERT_EQUAL_INT(0, (int)llext_res_g_resources[i].file_size);
		TEST_ASSERT_EQUAL_INT(0, (int)llext_res_g_resources[i].virtual_pos);
	}
}

/** @brief Calling initialize_resources twice should not crash and should reset state. */
static void T_LLEXT_RES_UTILS_CHECK_initialize_resources_idempotent(void) {
	UTIL_print_string("LLEXT_RES_UTILS initialize resources idempotent\n");

	LLEXT_RES_utils_initialize_resources();
	LLEXT_RES_utils_initialize_resources();

	TEST_ASSERT_MESSAGE(true == llext_res_g_initialized, "llext_res_g_initialized should be true");

	for (int32_t i = 0; i < LLEXT_MAX_OPEN_RESOURCES; i++) {
		TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_SLOT_FREE, (int)llext_res_g_resources[i].in_use);
	}
}

/* ========================================================================== */
/* =                        find_free_slot Tests                            = */
/* ========================================================================== */

/** @brief find_free_slot should return 0 when all slots are free. */
static void T_LLEXT_RES_UTILS_CHECK_find_free_slot_all_free(void) {
	UTIL_print_string("LLEXT_RES_UTILS find free slot all free\n");

	LLEXT_RES_utils_initialize_resources();

	int32_t slot = LLEXT_RES_utils_find_free_slot();
	TEST_ASSERT_EQUAL_INT(0, (int)slot);
}

/** @brief find_free_slot should return the first free slot when some are used. */
static void T_LLEXT_RES_UTILS_CHECK_find_free_slot_some_used(void) {
	UTIL_print_string("LLEXT_RES_UTILS find free slot some used\n");

	LLEXT_RES_utils_initialize_resources();

	/* Mark first 3 slots as used */
	llext_res_g_resources[0].in_use = LLEXT_RES_SLOT_USED;
	llext_res_g_resources[1].in_use = LLEXT_RES_SLOT_USED;
	llext_res_g_resources[2].in_use = LLEXT_RES_SLOT_USED;

	int32_t slot = LLEXT_RES_utils_find_free_slot();
	TEST_ASSERT_EQUAL_INT(3, (int)slot);
}

/** @brief find_free_slot should return -1 when all slots are used. */
static void T_LLEXT_RES_UTILS_CHECK_find_free_slot_all_used(void) {
	UTIL_print_string("LLEXT_RES_UTILS find free slot all used\n");

	LLEXT_RES_utils_initialize_resources();

	for (int32_t i = 0; i < LLEXT_MAX_OPEN_RESOURCES; i++) {
		llext_res_g_resources[i].in_use = LLEXT_RES_SLOT_USED;
	}

	int32_t slot = LLEXT_RES_utils_find_free_slot();
	TEST_ASSERT_EQUAL_INT(-1, (int)slot);
}

/** @brief find_free_slot should return the correct slot when only the last one is free. */
static void T_LLEXT_RES_UTILS_CHECK_find_free_slot_last_free(void) {
	UTIL_print_string("LLEXT_RES_UTILS find free slot last free\n");

	LLEXT_RES_utils_initialize_resources();

	for (int32_t i = 0; i < LLEXT_MAX_OPEN_RESOURCES - 1; i++) {
		llext_res_g_resources[i].in_use = LLEXT_RES_SLOT_USED;
	}

	int32_t slot = LLEXT_RES_utils_find_free_slot();
	TEST_ASSERT_EQUAL_INT(LLEXT_MAX_OPEN_RESOURCES - 1, (int)slot);
}

/* ========================================================================== */
/* =                      validate_resource Tests                           = */
/* ========================================================================== */

/** @brief validate_resource should succeed for a valid, open resource. */
static void T_LLEXT_RES_UTILS_CHECK_validate_resource_valid(void) {
	UTIL_print_string("LLEXT_RES_UTILS validate resource valid\n");

	LLEXT_RES_utils_initialize_resources();

	/* Resource IDs are 1-based (slot index + 1) */
	llext_res_g_resources[0].in_use = LLEXT_RES_SLOT_USED;

	int32_t out_slot = -1;
	int32_t ret = LLEXT_RES_utils_validate_resource((RES_ID)1, &out_slot);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_OK, (int)ret);
	TEST_ASSERT_EQUAL_INT(0, (int)out_slot);
}

/** @brief validate_resource should succeed for the last valid slot. */
static void T_LLEXT_RES_UTILS_CHECK_validate_resource_last_slot(void) {
	UTIL_print_string("LLEXT_RES_UTILS validate resource last slot\n");

	LLEXT_RES_utils_initialize_resources();

	llext_res_g_resources[LLEXT_MAX_OPEN_RESOURCES - 1].in_use = LLEXT_RES_SLOT_USED;

	int32_t out_slot = -1;
	int32_t ret = LLEXT_RES_utils_validate_resource((RES_ID)LLEXT_MAX_OPEN_RESOURCES, &out_slot);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_OK, (int)ret);
	TEST_ASSERT_EQUAL_INT(LLEXT_MAX_OPEN_RESOURCES - 1, (int)out_slot);
}

/** @brief validate_resource should fail for resource ID 0 (maps to slot -1). */
static void T_LLEXT_RES_UTILS_CHECK_validate_resource_id_zero(void) {
	UTIL_print_string("LLEXT_RES_UTILS validate resource id zero\n");

	LLEXT_RES_utils_initialize_resources();

	int32_t out_slot = -1;
	int32_t ret = LLEXT_RES_utils_validate_resource((RES_ID)0, &out_slot);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_RETURN_CODE_ERROR_UNDEFINED, (int)ret);
}

/** @brief validate_resource should fail for a negative resource ID. */
static void T_LLEXT_RES_UTILS_CHECK_validate_resource_negative_id(void) {
	UTIL_print_string("LLEXT_RES_UTILS validate resource negative id\n");

	LLEXT_RES_utils_initialize_resources();

	int32_t out_slot = -1;
	int32_t ret = LLEXT_RES_utils_validate_resource((RES_ID)(-5), &out_slot);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_RETURN_CODE_ERROR_UNDEFINED, (int)ret);
}

/** @brief validate_resource should fail for a resource ID beyond LLEXT_MAX_OPEN_RESOURCES. */
static void T_LLEXT_RES_UTILS_CHECK_validate_resource_id_too_large(void) {
	UTIL_print_string("LLEXT_RES_UTILS validate resource id too large\n");

	LLEXT_RES_utils_initialize_resources();

	int32_t out_slot = -1;
	int32_t ret = LLEXT_RES_utils_validate_resource((RES_ID)(LLEXT_MAX_OPEN_RESOURCES + 1), &out_slot);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_RETURN_CODE_ERROR_UNDEFINED, (int)ret);
}

/** @brief validate_resource should fail for a valid ID whose slot is not in use. */
static void T_LLEXT_RES_UTILS_CHECK_validate_resource_slot_closed(void) {
	UTIL_print_string("LLEXT_RES_UTILS validate resource slot closed\n");

	LLEXT_RES_utils_initialize_resources();

	/* Slot 0 is free (default after init), resource ID 1 maps to slot 0 */
	int32_t out_slot = -1;
	int32_t ret = LLEXT_RES_utils_validate_resource((RES_ID)1, &out_slot);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_RETURN_CODE_ERROR_UNDEFINED, (int)ret);
}

/* ========================================================================== */
/* =                      build_full_path Tests                             = */
/* ========================================================================== */

/** @brief build_full_path should prepend base path to the relative path. */
static void T_LLEXT_RES_UTILS_CHECK_build_full_path_basic(void) {
	UTIL_print_string("LLEXT_RES_UTILS build full path basic\n");

	uint8_t path_param[FS_PATH_LENGTH];
	(void)memset(path_param, 0, sizeof(path_param));

	int32_t ret = LLEXT_RES_utils_build_full_path("test.res", path_param);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_OK, (int)ret);

	/* Verify the path contains the base path and the filename */
	const char *result = (const char *)path_param;
	TEST_ASSERT_MESSAGE(NULL != strstr(result, "test.res"), "path should contain filename");
}

/** @brief build_full_path should handle leading slash in relative path. */
static void T_LLEXT_RES_UTILS_CHECK_build_full_path_leading_slash(void) {
	UTIL_print_string("LLEXT_RES_UTILS build full path leading slash\n");

	uint8_t path_param[FS_PATH_LENGTH];
	(void)memset(path_param, 0, sizeof(path_param));

	int32_t ret = LLEXT_RES_utils_build_full_path("/test.res", path_param);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_OK, (int)ret);

	/* Should not produce double slashes */
	const char *result = (const char *)path_param;
	TEST_ASSERT_MESSAGE(NULL == strstr(result, "//"), "path should not contain double slashes");
}

/* ========================================================================== */
/* =                     Boundary Condition Tests                           = */
/* ========================================================================== */

/** @brief validate_resource should fail for INT32_MIN (extreme negative). */
static void T_LLEXT_RES_UTILS_CHECK_validate_resource_int32_min(void) {
	UTIL_print_string("LLEXT_RES_UTILS validate resource INT32_MIN\n");

	LLEXT_RES_utils_initialize_resources();

	int32_t out_slot = -1;
	int32_t ret = LLEXT_RES_utils_validate_resource((RES_ID)INT32_MIN, &out_slot);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_RETURN_CODE_ERROR_UNDEFINED, (int)ret);
}

/** @brief validate_resource should fail for INT32_MAX (extreme positive). */
static void T_LLEXT_RES_UTILS_CHECK_validate_resource_int32_max(void) {
	UTIL_print_string("LLEXT_RES_UTILS validate resource INT32_MAX\n");

	LLEXT_RES_utils_initialize_resources();

	int32_t out_slot = -1;
	int32_t ret = LLEXT_RES_utils_validate_resource((RES_ID)INT32_MAX, &out_slot);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_RETURN_CODE_ERROR_UNDEFINED, (int)ret);
}

/** @brief build_full_path with an empty relative path should not crash. */
static void T_LLEXT_RES_UTILS_CHECK_build_full_path_empty(void) {
	UTIL_print_string("LLEXT_RES_UTILS build full path empty\n");

	uint8_t path_param[FS_PATH_LENGTH];
	(void)memset(path_param, 0, sizeof(path_param));

	/* Empty path: result is just the base path */
	int32_t ret = LLEXT_RES_utils_build_full_path("", path_param);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_OK, (int)ret);
}

/** @brief build_full_path should return error when combined path exceeds FS_PATH_LENGTH. */
static void T_LLEXT_RES_UTILS_CHECK_build_full_path_too_long(void) {
	UTIL_print_string("LLEXT_RES_UTILS build full path too long\n");

	/* Create a path of length FS_PATH_LENGTH-1 chars.
	 * Combined with LLEXT_FS_BASE_PATH (at least 1 char), total >= FS_PATH_LENGTH. */
	char long_path[FS_PATH_LENGTH];
	(void)memset(long_path, 'a', (size_t)FS_PATH_LENGTH - 1u);
	long_path[FS_PATH_LENGTH - 1] = '\0';

	uint8_t path_param[FS_PATH_LENGTH];
	(void)memset(path_param, 0, sizeof(path_param));

	int32_t ret = LLEXT_RES_utils_build_full_path(long_path, path_param);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_RETURN_CODE_ERROR_UNDEFINED, (int)ret);
}

/* ========================================================================== */
/* =                          Test Suite Declaration                        = */
/* ========================================================================== */

TestRef T_LLEXT_RES_UTILS_tests(void) {
	EMB_UNIT_TESTFIXTURES(fixtures) {
		/* initialize_resources */
		new_TestFixture("T_LLEXT_RES_UTILS_initialize_resources",
		                T_LLEXT_RES_UTILS_CHECK_initialize_resources),
		new_TestFixture("T_LLEXT_RES_UTILS_initialize_resources_idempotent",
		                T_LLEXT_RES_UTILS_CHECK_initialize_resources_idempotent),
		/* find_free_slot */
		new_TestFixture("T_LLEXT_RES_UTILS_find_free_slot_all_free",
		                T_LLEXT_RES_UTILS_CHECK_find_free_slot_all_free),
		new_TestFixture("T_LLEXT_RES_UTILS_find_free_slot_some_used",
		                T_LLEXT_RES_UTILS_CHECK_find_free_slot_some_used),
		new_TestFixture("T_LLEXT_RES_UTILS_find_free_slot_all_used",
		                T_LLEXT_RES_UTILS_CHECK_find_free_slot_all_used),
		new_TestFixture("T_LLEXT_RES_UTILS_find_free_slot_last_free",
		                T_LLEXT_RES_UTILS_CHECK_find_free_slot_last_free),
		/* validate_resource */
		new_TestFixture("T_LLEXT_RES_UTILS_validate_resource_valid",
		                T_LLEXT_RES_UTILS_CHECK_validate_resource_valid),
		new_TestFixture("T_LLEXT_RES_UTILS_validate_resource_last_slot",
		                T_LLEXT_RES_UTILS_CHECK_validate_resource_last_slot),
		new_TestFixture("T_LLEXT_RES_UTILS_validate_resource_id_zero",
		                T_LLEXT_RES_UTILS_CHECK_validate_resource_id_zero),
		new_TestFixture("T_LLEXT_RES_UTILS_validate_resource_negative_id",
		                T_LLEXT_RES_UTILS_CHECK_validate_resource_negative_id),
		new_TestFixture("T_LLEXT_RES_UTILS_validate_resource_id_too_large",
		                T_LLEXT_RES_UTILS_CHECK_validate_resource_id_too_large),
		new_TestFixture("T_LLEXT_RES_UTILS_validate_resource_slot_closed",
		                T_LLEXT_RES_UTILS_CHECK_validate_resource_slot_closed),
		/* build_full_path */
		new_TestFixture("T_LLEXT_RES_UTILS_build_full_path_basic",
		                T_LLEXT_RES_UTILS_CHECK_build_full_path_basic),
		new_TestFixture("T_LLEXT_RES_UTILS_build_full_path_leading_slash",
		                T_LLEXT_RES_UTILS_CHECK_build_full_path_leading_slash),
		/* Boundary Conditions */
		new_TestFixture("T_LLEXT_RES_UTILS_validate_resource_int32_min",
		                T_LLEXT_RES_UTILS_CHECK_validate_resource_int32_min),
		new_TestFixture("T_LLEXT_RES_UTILS_validate_resource_int32_max",
		                T_LLEXT_RES_UTILS_CHECK_validate_resource_int32_max),
		new_TestFixture("T_LLEXT_RES_UTILS_build_full_path_empty",
		                T_LLEXT_RES_UTILS_CHECK_build_full_path_empty),
		new_TestFixture("T_LLEXT_RES_UTILS_build_full_path_too_long",
		                T_LLEXT_RES_UTILS_CHECK_build_full_path_too_long),
	};
	EMB_UNIT_TESTCALLER(llext_res_utils_tests, "LLEXT_RES_UTILS tests",
	                    T_LLEXT_RES_UTILS_setUp, T_LLEXT_RES_UTILS_tearDown, fixtures);

	return (TestRef) & llext_res_utils_tests;
}
