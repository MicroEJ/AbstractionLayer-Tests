/*
 * C
 *
 * Copyright 2026 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 * Build: 7E4D1F7C
 */

/**
 * @file t_llext_res_cache.c
 * @brief Test suite for llext_res_cache LRU implementation using embUnit.
 */

#include <string.h>
#include <stdint.h>

#include "t_llext_res_cache.h"
#include "embUnit.h"
#include "u_print.h"
#include "LLEXT_RES_cache.h"

/* -------------------------------------------------------------------------- */
/* -                                Constants                                - */
/* -------------------------------------------------------------------------- */

/** @brief Helper: Generate test keys from integers (add 1 to avoid EMPTY_CACHE_KEY which is 0). */
#define TEST_KEY(n)    ((llext_res_cache_entry_key)(uintptr_t)((n) + 1))

/* -------------------------------------------------------------------------- */
/* -                            Static variables                             - */
/* -------------------------------------------------------------------------- */

/** @brief Cache instance shared across tests, reset in setUp. */
static struct llext_res_cache cache;

/* -------------------------------------------------------------------------- */
/* -                           setUp / tearDown                              - */
/* -------------------------------------------------------------------------- */

/** @brief Function called before running each test. */
static void T_LLEXT_RES_CACHE_setUp(void) {
	UTIL_print_string("\nT_LLEXT_RES_CACHE_setUp\n");
	memset(&cache, 0, sizeof(cache));
}

/** @brief Function called after running each test. */
static void T_LLEXT_RES_CACHE_tearDown(void) {
	UTIL_print_string("T_LLEXT_RES_CACHE_tearDown\n");
}

/* -------------------------------------------------------------------------- */
/* -                           Helper functions                              - */
/* -------------------------------------------------------------------------- */

/** @brief Empty cache key value (must match EMPTY_CACHE_KEY in LLEXT_RES_cache_generic.c). */
#define EMPTY_CACHE_KEY ((uintptr_t)0U)

/**
 * @brief Count the number of non-empty entries in the cache.
 */
static uint32_t get_entries_used(const struct llext_res_cache *c) {
	uint32_t count = 0;
	for (uint32_t i = 0u; i < (uint32_t)LLEXT_RES_CACHE_MAX_ENTRIES; i++) {
		if (EMPTY_CACHE_KEY != c->entries[i].key) {
			count++;
		}
	}
	return count;
}

/**
 * @brief Fill cache to capacity with sequential keys.
 */
static void fill_cache(int start_key) {
	unsigned char data[16];
	for (uint32_t i = 0u; i < LLEXT_RES_CACHE_MAX_ENTRIES; i++) {
		memset(data, (unsigned char)(start_key + (int)i), sizeof(data));
		struct llext_res_cache_entry *entry =
			LLEXT_RES_CACHE_put(&cache, TEST_KEY(start_key + (int)i), data, sizeof(data));
		TEST_ASSERT_NOT_NULL(entry);
	}
}

/* ========================================================================== */
/* =                          Basic Operations Tests                        = */
/* ========================================================================== */

/** @brief Put an entry and verify it can be retrieved with matching data. */
static void T_LLEXT_RES_CACHE_CHECK_put_and_get(void) {
	UTIL_print_string("LLEXT_RES_CACHE put and get\n");

	unsigned char data[] = { 0x11, 0x22, 0x33, 0x44, 0x55 };
	struct llext_res_cache_entry *put_entry = LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));

	TEST_ASSERT_NOT_NULL(put_entry);
	TEST_ASSERT_MESSAGE(put_entry->key == TEST_KEY(1), "put entry key mismatch");
	TEST_ASSERT_EQUAL_INT((int)sizeof(data), (int)put_entry->size);
	TEST_ASSERT_MESSAGE(0 == memcmp(put_entry->data, data, sizeof(data)), "put entry data mismatch");

	struct llext_res_cache_entry *get_entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), sizeof(data));

	TEST_ASSERT_NOT_NULL(get_entry);
	TEST_ASSERT_MESSAGE(get_entry == put_entry, "get entry should match put entry");
	TEST_ASSERT_MESSAGE(0 == memcmp(get_entry->data, data, sizeof(data)), "get entry data mismatch");
}

/** @brief Get with a key not in the cache should return NULL. */
static void T_LLEXT_RES_CACHE_CHECK_get_nonexistent_key(void) {
	UTIL_print_string("LLEXT_RES_CACHE get nonexistent key\n");

	struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(999), 10);
	TEST_ASSERT_NULL(entry);
}

/** @brief Putting with an existing key should update the entry. */
static void T_LLEXT_RES_CACHE_CHECK_put_updates_existing(void) {
	UTIL_print_string("LLEXT_RES_CACHE put updates existing\n");

	unsigned char data1[] = { 0xAA, 0xBB, 0xCC };
	unsigned char data2[] = { 0x11, 0x22, 0x33, 0x44 };

	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data1, sizeof(data1));
	struct llext_res_cache_entry *entry2 = LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data2, sizeof(data2));

	TEST_ASSERT_NOT_NULL(entry2);
	TEST_ASSERT_EQUAL_INT((int)sizeof(data2), (int)entry2->size);
	TEST_ASSERT_MESSAGE(0 == memcmp(entry2->data, data2, sizeof(data2)), "updated entry data mismatch");

	/* Verify only one entry exists for this key */
	struct llext_res_cache_entry *get_entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), sizeof(data2));
	TEST_ASSERT_MESSAGE(get_entry == entry2, "get should return the updated entry");
}

/* ========================================================================== */
/* =                          Size Handling Tests                           = */
/* ========================================================================== */

/** @brief Get should succeed when requesting less or equal size, fail when requesting more. */
static void T_LLEXT_RES_CACHE_CHECK_get_size_check(void) {
	UTIL_print_string("LLEXT_RES_CACHE get size check\n");

	unsigned char data[100];
	memset(data, 0x42, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));

	/* Request less than stored - should succeed */
	struct llext_res_cache_entry *entry1 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 50);
	TEST_ASSERT_NOT_NULL(entry1);

	/* Request exactly stored size - should succeed */
	struct llext_res_cache_entry *entry2 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 100);
	TEST_ASSERT_NOT_NULL(entry2);

	/* Request more than stored - should fail */
	struct llext_res_cache_entry *entry3 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 101);
	TEST_ASSERT_NULL(entry3);
}

/** @brief Putting data larger than max entry size should fail. */
static void T_LLEXT_RES_CACHE_CHECK_put_size_too_large(void) {
	UTIL_print_string("LLEXT_RES_CACHE put size too large\n");

	unsigned char data[LLEXT_RES_CACHE_ENTRY_DATA_SIZE + 1u];
	memset(data, 0x99, sizeof(data));

	struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	TEST_ASSERT_NULL(entry);
}

/** @brief Putting data at exact max size should succeed. */
static void T_LLEXT_RES_CACHE_CHECK_put_exact_max_size(void) {
	UTIL_print_string("LLEXT_RES_CACHE put exact max size\n");

	unsigned char data[LLEXT_RES_CACHE_ENTRY_DATA_SIZE];
	memset(data, 0x77, sizeof(data));

	struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	TEST_ASSERT_NOT_NULL(entry);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_CACHE_ENTRY_DATA_SIZE, (int)entry->size);
}

/** @brief Putting data with zero size should succeed. */
static void T_LLEXT_RES_CACHE_CHECK_put_zero_size(void) {
	UTIL_print_string("LLEXT_RES_CACHE put zero size\n");

	unsigned char data[] = { 0x00 };

	struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, 0);
	TEST_ASSERT_NOT_NULL(entry);
	TEST_ASSERT_EQUAL_INT(0, (int)entry->size);

	/* Should be able to get with size 0 */
	struct llext_res_cache_entry *get_entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 0);
	TEST_ASSERT_NOT_NULL(get_entry);
}

/* ========================================================================== */
/* =                          LRU Eviction Tests                            = */
/* ========================================================================== */

/** @brief Oldest entry should be evicted when cache is full. */
static void T_LLEXT_RES_CACHE_CHECK_lru_eviction_order(void) {
	UTIL_print_string("LLEXT_RES_CACHE lru eviction order\n");

	/* Fill cache completely */
	fill_cache(1);

	/* Key 1 was added first, so it should have the oldest timestamp.
	 * Adding a new entry should evict key 1. */
	unsigned char new_data[] = { 0xFF, 0xEE, 0xDD };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(100), new_data, sizeof(new_data));

	/* Key 1 should be gone */
	struct llext_res_cache_entry *entry1 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1);
	TEST_ASSERT_NULL(entry1);

	/* Key 100 should exist */
	struct llext_res_cache_entry *entry100 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(100), sizeof(new_data));
	TEST_ASSERT_NOT_NULL(entry100);

	/* Keys 2..MAX_ENTRIES should still exist */
	for (int i = 2; i <= (int)LLEXT_RES_CACHE_MAX_ENTRIES; i++) {
		struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(i), 1);
		TEST_ASSERT_NOT_NULL(entry);
	}
}

/** @brief Accessing an entry should update its timestamp and prevent eviction. */
static void T_LLEXT_RES_CACHE_CHECK_lru_access_updates_timestamp(void) {
	UTIL_print_string("LLEXT_RES_CACHE lru access updates timestamp\n");

	/* Fill cache completely */
	fill_cache(1);

	/* Access key 1 to make it recently used */
	struct llext_res_cache_entry *entry1 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1);
	TEST_ASSERT_NOT_NULL(entry1);

	/* Now key 2 should be the oldest (least recently used).
	 * Adding a new entry should evict key 2, not key 1. */
	unsigned char new_data[] = { 0xAB, 0xCD };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(200), new_data, sizeof(new_data));

	/* Key 1 should still exist (was accessed) */
	entry1 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1);
	TEST_ASSERT_NOT_NULL(entry1);

	/* Key 2 should be evicted */
	struct llext_res_cache_entry *entry2 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(2), 1);
	TEST_ASSERT_NULL(entry2);
}

/** @brief Multiple evictions should evict entries in LRU order. */
static void T_LLEXT_RES_CACHE_CHECK_lru_multiple_evictions(void) {
	UTIL_print_string("LLEXT_RES_CACHE lru multiple evictions\n");

	/* Fill cache completely */
	fill_cache(1);

	/* Add 3 more entries, which should evict the 3 oldest keys (1, 2, 3) */
	int evict_count = 3;
	for (int i = 0; i < evict_count; i++) {
		unsigned char data = (unsigned char)(0x50 + i);
		LLEXT_RES_CACHE_put(&cache, TEST_KEY(500 + i), &data, 1);
	}

	/* Keys 1..evict_count should be gone */
	for (int i = 1; i <= evict_count; i++) {
		struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(i), 1);
		TEST_ASSERT_NULL(entry);
	}

	/* Keys (evict_count+1)..MAX_ENTRIES should still exist */
	for (int i = evict_count + 1; i <= (int)LLEXT_RES_CACHE_MAX_ENTRIES; i++) {
		struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(i), 1);
		TEST_ASSERT_NOT_NULL(entry);
	}

	/* Keys 500..(500+evict_count-1) should exist */
	for (int i = 0; i < evict_count; i++) {
		struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(500 + i), 1);
		TEST_ASSERT_NOT_NULL(entry);
	}
}

/* ========================================================================== */
/* =                       Cache Invalidation Tests                         = */
/* ========================================================================== */

/** @brief Invalidate should remove only entries matching the key/mask pattern. */
static void T_LLEXT_RES_CACHE_CHECK_invalidate_single_entry(void) {
	UTIL_print_string("LLEXT_RES_CACHE invalidate single entry\n");

	/* Add entries with keys that have specific patterns.
	 * Key format: upper bits = resource ID, lower bits = offset. */
	llext_res_cache_entry_key key1 = (llext_res_cache_entry_key)0x01000010;
	llext_res_cache_entry_key key2 = (llext_res_cache_entry_key)0x02000020;

	unsigned char data1[] = { 0x11 };
	unsigned char data2[] = { 0x22 };

	LLEXT_RES_CACHE_put(&cache, key1, data1, sizeof(data1));
	LLEXT_RES_CACHE_put(&cache, key2, data2, sizeof(data2));

	/* Invalidate only resource 1 (mask upper byte) */
	LLEXT_RES_CACHE_invalidate(&cache, (llext_res_cache_entry_key)0x01000000, 0xFF000000);

	/* Key1 should be gone */
	struct llext_res_cache_entry *entry1 = LLEXT_RES_CACHE_get(&cache, key1, 1);
	TEST_ASSERT_NULL(entry1);

	/* Key2 should still exist */
	struct llext_res_cache_entry *entry2 = LLEXT_RES_CACHE_get(&cache, key2, 1);
	TEST_ASSERT_NOT_NULL(entry2);
}

/** @brief Invalidate should remove all entries matching the same resource prefix. */
static void T_LLEXT_RES_CACHE_CHECK_invalidate_multiple_entries(void) {
	UTIL_print_string("LLEXT_RES_CACHE invalidate multiple entries\n");

	/* Add multiple entries for same resource (resource 5) */
	llext_res_cache_entry_key key1 = (llext_res_cache_entry_key)0x05000000;
	llext_res_cache_entry_key key2 = (llext_res_cache_entry_key)0x05000100;
	llext_res_cache_entry_key key3 = (llext_res_cache_entry_key)0x05000200;
	llext_res_cache_entry_key key4 = (llext_res_cache_entry_key)0x06000000; /* Different resource */

	unsigned char data[] = { 0xAA };
	LLEXT_RES_CACHE_put(&cache, key1, data, 1);
	LLEXT_RES_CACHE_put(&cache, key2, data, 1);
	LLEXT_RES_CACHE_put(&cache, key3, data, 1);
	LLEXT_RES_CACHE_put(&cache, key4, data, 1);

	/* Invalidate all entries for resource 5 */
	LLEXT_RES_CACHE_invalidate(&cache, (llext_res_cache_entry_key)0x05000000, 0xFF000000);

	/* Keys for resource 5 should be gone */
	TEST_ASSERT_NULL(LLEXT_RES_CACHE_get(&cache, key1, 1));
	TEST_ASSERT_NULL(LLEXT_RES_CACHE_get(&cache, key2, 1));
	TEST_ASSERT_NULL(LLEXT_RES_CACHE_get(&cache, key3, 1));

	/* Key for resource 6 should still exist */
	TEST_ASSERT_NOT_NULL(LLEXT_RES_CACHE_get(&cache, key4, 1));
}

/** @brief Invalidate with a non-matching pattern should not remove any entries. */
static void T_LLEXT_RES_CACHE_CHECK_invalidate_no_match(void) {
	UTIL_print_string("LLEXT_RES_CACHE invalidate no match\n");

	unsigned char data[] = { 0x12, 0x34 };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(2), data, sizeof(data));

	/* Invalidate with a prefix that doesn't match any entry */
	LLEXT_RES_CACHE_invalidate(&cache, (llext_res_cache_entry_key)0xFFFF0000, 0xFFFF0000);

	/* Both entries should still exist */
	TEST_ASSERT_NOT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1));
	TEST_ASSERT_NOT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(2), 1));
}

/** @brief Invalidating an empty cache should not crash. */
static void T_LLEXT_RES_CACHE_CHECK_invalidate_empty_cache(void) {
	UTIL_print_string("LLEXT_RES_CACHE invalidate empty cache\n");

	/* Invalidating an empty cache should not crash */
	LLEXT_RES_CACHE_invalidate(&cache, (llext_res_cache_entry_key)0x01000000, 0xFF000000);

	TEST_ASSERT_EQUAL_INT(0, (int)get_entries_used(&cache));
}

/** @brief Invalidating all entries should empty the cache and leave it functional. */
static void T_LLEXT_RES_CACHE_CHECK_invalidate_all_entries(void) {
	UTIL_print_string("LLEXT_RES_CACHE invalidate all entries\n");

	/* Fill cache with entries all belonging to resource 0x01 */
	unsigned char data[] = { 0xBB };
	for (uint32_t i = 0u; i < LLEXT_RES_CACHE_MAX_ENTRIES; i++) {
		llext_res_cache_entry_key key = (llext_res_cache_entry_key)(0x01000000u | (i * 0x100u));
		LLEXT_RES_CACHE_put(&cache, key, data, sizeof(data));
	}
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_CACHE_MAX_ENTRIES, (int)get_entries_used(&cache));

	/* Invalidate all entries for resource 0x01 */
	LLEXT_RES_CACHE_invalidate(&cache, (llext_res_cache_entry_key)0x01000000, 0xFF000000);

	/* Cache should be empty */
	TEST_ASSERT_EQUAL_INT(0, (int)get_entries_used(&cache));

	/* Cache should still be functional after full invalidation */
	struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_put(&cache, TEST_KEY(42), data, sizeof(data));
	TEST_ASSERT_NOT_NULL(entry);
	TEST_ASSERT_NOT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(42), 1));
}

/* ========================================================================== */
/* =                            Edge Cases Tests                            = */
/* ========================================================================== */

/** @brief Get on an empty cache should return NULL. */
static void T_LLEXT_RES_CACHE_CHECK_empty_cache_get(void) {
	UTIL_print_string("LLEXT_RES_CACHE empty cache get\n");

	struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 10);
	TEST_ASSERT_NULL(entry);
}

/** @brief Filling cache to exact capacity should allow retrieving all entries. */
static void T_LLEXT_RES_CACHE_CHECK_fill_exact_capacity(void) {
	UTIL_print_string("LLEXT_RES_CACHE fill exact capacity\n");

	/* Fill exactly to capacity */
	fill_cache(1);

	/* All entries should be retrievable */
	for (int i = 1; i <= (int)LLEXT_RES_CACHE_MAX_ENTRIES; i++) {
		struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(i), 1);
		TEST_ASSERT_NOT_NULL(entry);
	}
}

/** @brief Replacing a key with different sized data should update correctly. */
static void T_LLEXT_RES_CACHE_CHECK_replace_same_key_different_size(void) {
	UTIL_print_string("LLEXT_RES_CACHE replace same key different size\n");

	unsigned char data1[50];
	unsigned char data2[100];
	memset(data1, 0xAA, sizeof(data1));
	memset(data2, 0xBB, sizeof(data2));

	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data1, sizeof(data1));

	/* Verify initial size */
	struct llext_res_cache_entry *entry1 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 50);
	TEST_ASSERT_NOT_NULL(entry1);
	TEST_ASSERT_EQUAL_INT(50, (int)entry1->size);

	/* Replace with larger data */
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data2, sizeof(data2));

	/* Verify new size and data */
	struct llext_res_cache_entry *entry2 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 100);
	TEST_ASSERT_NOT_NULL(entry2);
	TEST_ASSERT_EQUAL_INT(100, (int)entry2->size);
	TEST_ASSERT_MESSAGE(0xBB == entry2->data[0], "replaced entry data mismatch");
}

/** @brief Multiple keys can store the same data independently. */
static void T_LLEXT_RES_CACHE_CHECK_multiple_keys_same_data(void) {
	UTIL_print_string("LLEXT_RES_CACHE multiple keys same data\n");

	unsigned char data[] = { 0x12, 0x34, 0x56 };

	/* Store same data under different keys */
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(2), data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(3), data, sizeof(data));

	/* All should be retrievable independently */
	struct llext_res_cache_entry *e1 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), sizeof(data));
	struct llext_res_cache_entry *e2 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(2), sizeof(data));
	struct llext_res_cache_entry *e3 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(3), sizeof(data));

	TEST_ASSERT_NOT_NULL(e1);
	TEST_ASSERT_NOT_NULL(e2);
	TEST_ASSERT_NOT_NULL(e3);
	TEST_ASSERT_MESSAGE((e1 != e2) && (e2 != e3) && (e1 != e3), "entries should be different");
}

/** @brief Put with an existing key should not duplicate when an empty slot precedes the matching entry. */
static void T_LLEXT_RES_CACHE_CHECK_put_no_duplicate_with_empty_slot_before(void) {
	UTIL_print_string("LLEXT_RES_CACHE put no duplicate with empty slot before\n");

	/* Fill the first 3 slots with keys A, B, C */
	unsigned char data1[] = { 0xAA };
	unsigned char data2[] = { 0xBB };
	unsigned char data3[] = { 0xCC };
	llext_res_cache_entry_key key_a = TEST_KEY(10);
	llext_res_cache_entry_key key_b = TEST_KEY(20);
	llext_res_cache_entry_key key_c = TEST_KEY(30);

	LLEXT_RES_CACHE_put(&cache, key_a, data1, sizeof(data1));
	LLEXT_RES_CACHE_put(&cache, key_b, data2, sizeof(data2));
	LLEXT_RES_CACHE_put(&cache, key_c, data3, sizeof(data3));
	TEST_ASSERT_EQUAL_INT(3, (int)get_entries_used(&cache));

	/* Invalidate key_a to create an empty slot before key_b and key_c */
	LLEXT_RES_CACHE_invalidate(&cache, key_a, UINTPTR_MAX);
	TEST_ASSERT_EQUAL_INT(2, (int)get_entries_used(&cache));

	/* Update key_c with new data. The empty slot (previously key_a) should NOT
	 * be used; instead key_c's existing entry should be updated in place. */
	unsigned char data3_updated[] = { 0xDD };
	LLEXT_RES_CACHE_put(&cache, key_c, data3_updated, sizeof(data3_updated));

	/* Entry count should still be 2, not 3 (no duplication) */
	TEST_ASSERT_EQUAL_INT(2, (int)get_entries_used(&cache));

	/* Verify the updated data is correct */
	struct llext_res_cache_entry *entry_c = LLEXT_RES_CACHE_get(&cache, key_c, sizeof(data3_updated));
	TEST_ASSERT_NOT_NULL(entry_c);
	TEST_ASSERT_MESSAGE(0xDD == entry_c->data[0], "updated entry data mismatch");
}

/* ========================================================================== */
/* =                          Entries Used Tests                            = */
/* ========================================================================== */

/** @brief Entries used count should track additions and updates correctly. */
static void T_LLEXT_RES_CACHE_CHECK_entries_used(void) {
	UTIL_print_string("LLEXT_RES_CACHE entries used\n");

	TEST_ASSERT_EQUAL_INT(0, (int)get_entries_used(&cache));

	unsigned char data[] = { 0x11 };

	/* Add entries and check count */
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	TEST_ASSERT_EQUAL_INT(1, (int)get_entries_used(&cache));

	LLEXT_RES_CACHE_put(&cache, TEST_KEY(2), data, sizeof(data));
	TEST_ASSERT_EQUAL_INT(2, (int)get_entries_used(&cache));

	LLEXT_RES_CACHE_put(&cache, TEST_KEY(3), data, sizeof(data));
	TEST_ASSERT_EQUAL_INT(3, (int)get_entries_used(&cache));

	/* Updating existing key shouldn't increase count */
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	TEST_ASSERT_EQUAL_INT(3, (int)get_entries_used(&cache));
}

/** @brief Entries used should decrease after invalidation. */
static void T_LLEXT_RES_CACHE_CHECK_entries_used_after_invalidation(void) {
	UTIL_print_string("LLEXT_RES_CACHE entries used after invalidation\n");

	/* Add entries for two different resources */
	unsigned char data[] = { 0xCC };
	llext_res_cache_entry_key key_r1_a = (llext_res_cache_entry_key)0x01000000;
	llext_res_cache_entry_key key_r1_b = (llext_res_cache_entry_key)0x01000100;
	llext_res_cache_entry_key key_r2_a = (llext_res_cache_entry_key)0x02000000;

	LLEXT_RES_CACHE_put(&cache, key_r1_a, data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, key_r1_b, data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, key_r2_a, data, sizeof(data));
	TEST_ASSERT_EQUAL_INT(3, (int)get_entries_used(&cache));

	/* Invalidate resource 1 (2 entries) */
	LLEXT_RES_CACHE_invalidate(&cache, (llext_res_cache_entry_key)0x01000000, 0xFF000000);
	TEST_ASSERT_EQUAL_INT(1, (int)get_entries_used(&cache));
}

/** @brief Entries used should remain at max when evictions occur. */
static void T_LLEXT_RES_CACHE_CHECK_entries_used_at_capacity_with_eviction(void) {
	UTIL_print_string("LLEXT_RES_CACHE entries used at capacity with eviction\n");

	/* Fill cache completely */
	fill_cache(1);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_CACHE_MAX_ENTRIES, (int)get_entries_used(&cache));

	/* Add more entries causing evictions - count should stay at MAX */
	unsigned char data[] = { 0xDD };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(100), data, sizeof(data));
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_CACHE_MAX_ENTRIES, (int)get_entries_used(&cache));

	LLEXT_RES_CACHE_put(&cache, TEST_KEY(101), data, sizeof(data));
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_CACHE_MAX_ENTRIES, (int)get_entries_used(&cache));
}

/* ========================================================================== */
/* =                          Cache Clear Tests                             = */
/* ========================================================================== */

/** @brief Clear should empty a populated cache. */
static void T_LLEXT_RES_CACHE_CHECK_clear_empties_cache(void) {
	UTIL_print_string("LLEXT_RES_CACHE clear empties cache\n");

	fill_cache(1);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_CACHE_MAX_ENTRIES, (int)get_entries_used(&cache));

	LLEXT_RES_CACHE_clear(&cache);

	TEST_ASSERT_EQUAL_INT(0, (int)get_entries_used(&cache));

	/* All previously stored keys should be gone */
	for (int i = 1; i <= (int)LLEXT_RES_CACHE_MAX_ENTRIES; i++) {
		TEST_ASSERT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(i), 1));
	}
}

/** @brief Clear should reset the access counter to zero. */
static void T_LLEXT_RES_CACHE_CHECK_clear_resets_access_counter(void) {
	UTIL_print_string("LLEXT_RES_CACHE clear resets access counter\n");

	/* Perform many operations to increment the access counter */
	unsigned char data[] = { 0x11 };
	for (int i = 0; i < 50; i++) {
		LLEXT_RES_CACHE_put(&cache, TEST_KEY(i % (int)LLEXT_RES_CACHE_MAX_ENTRIES), data, sizeof(data));
	}

	LLEXT_RES_CACHE_clear(&cache);
	TEST_ASSERT_EQUAL_INT(0, (int)cache.access_counter);
}

/** @brief Cache should be fully functional after a clear. */
static void T_LLEXT_RES_CACHE_CHECK_clear_cache_functional_after(void) {
	UTIL_print_string("LLEXT_RES_CACHE clear cache functional after\n");

	fill_cache(1);
	LLEXT_RES_CACHE_clear(&cache);

	/* Put and get should work normally after clear */
	unsigned char data[] = { 0xAA, 0xBB };
	struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_put(&cache, TEST_KEY(42), data, sizeof(data));
	TEST_ASSERT_NOT_NULL(entry);

	struct llext_res_cache_entry *get_entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(42), sizeof(data));
	TEST_ASSERT_NOT_NULL(get_entry);
	TEST_ASSERT_MESSAGE(0 == memcmp(get_entry->data, data, sizeof(data)), "data mismatch after clear");
}

/** @brief Clearing an already empty cache should not crash. */
static void T_LLEXT_RES_CACHE_CHECK_clear_empty_cache(void) {
	UTIL_print_string("LLEXT_RES_CACHE clear empty cache\n");

	LLEXT_RES_CACHE_clear(&cache);
	TEST_ASSERT_EQUAL_INT(0, (int)get_entries_used(&cache));
	TEST_ASSERT_EQUAL_INT(0, (int)cache.access_counter);
}

/* ========================================================================== */
/* =                       Cache Key Addressing Tests                       = */
/* ========================================================================== */

/** @brief generate_key for resource 0 at offset 0 should produce expected key. */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_basic(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key basic\n");

	llext_res_cache_entry_key key = LLEXT_RES_CACHE_generate_key(0, 0);
	/* resource_id 0 + 1 = 1, shifted left 24 bits = 0x01000000 */
	TEST_ASSERT_MESSAGE(key == (llext_res_cache_entry_key)0x01000000u, "key for resource 0, offset 0 mismatch");
}

/** @brief Offsets within the same block should produce the same key. */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_block_alignment(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key block alignment\n");

	/* Two offsets within the same block should produce the same key */
	llext_res_cache_entry_key key1 = LLEXT_RES_CACHE_generate_key(0, 0);
	llext_res_cache_entry_key key2 = LLEXT_RES_CACHE_generate_key(0, (int64_t)CACHE_BLOCK_SIZE - 1);
	TEST_ASSERT_MESSAGE(key1 == key2, "offsets within same block should produce same key");

	/* Offset at next block boundary should produce different key */
	llext_res_cache_entry_key key3 = LLEXT_RES_CACHE_generate_key(0, (int64_t)CACHE_BLOCK_SIZE);
	TEST_ASSERT_MESSAGE(key1 != key3, "offsets in different blocks should produce different keys");
}

/** @brief Different resources at the same offset should produce different keys. */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_different_resources(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key different resources\n");

	llext_res_cache_entry_key key1 = LLEXT_RES_CACHE_generate_key(0, 0);
	llext_res_cache_entry_key key2 = LLEXT_RES_CACHE_generate_key(1, 0);
	TEST_ASSERT_MESSAGE(key1 != key2, "different resources should produce different keys");
}

/** @brief Generated key should never be 0 (EMPTY_CACHE_KEY). */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_nonzero(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key nonzero\n");

	/* Key should never be 0 for any valid resource, since 0 is EMPTY_CACHE_KEY */
	for (int32_t rid = 0; rid < 10; rid++) {
		llext_res_cache_entry_key key = LLEXT_RES_CACHE_generate_key(rid, 0);
		TEST_ASSERT_MESSAGE(key != (llext_res_cache_entry_key)0u, "generated key must not be EMPTY_CACHE_KEY");
	}
}

/** @brief generate_key_prefix should produce expected prefix values. */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_prefix_basic(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key prefix basic\n");

	llext_res_cache_entry_key prefix0 = LLEXT_RES_CACHE_generate_key_prefix(0);
	TEST_ASSERT_MESSAGE(prefix0 == (llext_res_cache_entry_key)0x01000000u, "prefix for resource 0 mismatch");

	llext_res_cache_entry_key prefix5 = LLEXT_RES_CACHE_generate_key_prefix(5);
	TEST_ASSERT_MESSAGE(prefix5 == (llext_res_cache_entry_key)0x06000000u, "prefix for resource 5 mismatch");
}

/** @brief Key prefix should match the upper bits of all keys generated for the same resource. */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_prefix_matches_key(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key prefix matches key\n");

	int32_t resource_id = 3;
	llext_res_cache_entry_key prefix = LLEXT_RES_CACHE_generate_key_prefix(resource_id);

	/* All keys for this resource should match the prefix when masked */
	for (int64_t offset = 0; offset < (int64_t)(CACHE_BLOCK_SIZE * 5);
	     offset += (int64_t)CACHE_BLOCK_SIZE) {
		llext_res_cache_entry_key key = LLEXT_RES_CACHE_generate_key(resource_id, offset);
		TEST_ASSERT_MESSAGE((key & CACHE_KEY_RESOURCE_MASK) == prefix,
		                    "key upper bits should match prefix");
	}

	/* Keys from a different resource should NOT match */
	llext_res_cache_entry_key other_key = LLEXT_RES_CACHE_generate_key(resource_id + 1, 0);
	TEST_ASSERT_MESSAGE((other_key & CACHE_KEY_RESOURCE_MASK) != prefix,
	                    "key from different resource should not match prefix");
}

/** @brief get_block_offset at block boundaries should return 0. */
static void T_LLEXT_RES_CACHE_CHECK_get_block_offset_at_boundary(void) {
	UTIL_print_string("LLEXT_RES_CACHE get block offset at boundary\n");

	TEST_ASSERT_EQUAL_INT(0, (int)LLEXT_RES_CACHE_get_block_offset(0));
	TEST_ASSERT_EQUAL_INT(0, (int)LLEXT_RES_CACHE_get_block_offset((int64_t)CACHE_BLOCK_SIZE));
	TEST_ASSERT_EQUAL_INT(0, (int)LLEXT_RES_CACHE_get_block_offset((int64_t)CACHE_BLOCK_SIZE * 2));
}

/** @brief get_block_offset within a block should return the correct sub-block offset. */
static void T_LLEXT_RES_CACHE_CHECK_get_block_offset_within_block(void) {
	UTIL_print_string("LLEXT_RES_CACHE get block offset within block\n");

	TEST_ASSERT_EQUAL_INT(1, (int)LLEXT_RES_CACHE_get_block_offset(1));
	TEST_ASSERT_EQUAL_INT((int)(CACHE_BLOCK_SIZE - 1u),
	                      (int)LLEXT_RES_CACHE_get_block_offset((int64_t)CACHE_BLOCK_SIZE - 1));

	/* Offset within the second block */
	TEST_ASSERT_EQUAL_INT(10, (int)LLEXT_RES_CACHE_get_block_offset((int64_t)CACHE_BLOCK_SIZE + 10));
}

/* ========================================================================== */
/* =                   Access Counter Renormalization Tests                  = */
/* ========================================================================== */

/** @brief LRU ordering should be preserved after access counter renormalization. */
static void T_LLEXT_RES_CACHE_CHECK_renormalization_preserves_lru(void) {
	UTIL_print_string("LLEXT_RES_CACHE renormalization preserves LRU order\n");

	/* Set access counter near the renormalization threshold (UINT32_MAX - 1000) */
	cache.access_counter = UINT32_MAX - 1000u - 5u;

	/* Fill cache */
	fill_cache(1);

	/* Access key 1 to make it the most recently used */
	struct llext_res_cache_entry *entry1 = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1);
	TEST_ASSERT_NOT_NULL(entry1);

	/* Key 2 should now be the LRU. Adding a new entry should evict key 2, not key 1. */
	unsigned char new_data[] = { 0xFF };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(100), new_data, sizeof(new_data));

	/* Key 1 should still exist (was accessed recently) */
	TEST_ASSERT_NOT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1));

	/* Key 2 should have been evicted (was LRU) */
	TEST_ASSERT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(2), 1));
}

/** @brief Cache should remain functional after renormalization resets the counter. */
static void T_LLEXT_RES_CACHE_CHECK_renormalization_counter_reset(void) {
	UTIL_print_string("LLEXT_RES_CACHE renormalization counter reset\n");

	/* Set counter near threshold */
	cache.access_counter = UINT32_MAX - 1000u - 2u;

	unsigned char data[] = { 0x42 };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(2), data, sizeof(data));

	/* After renormalization the counter should have been reduced */
	TEST_ASSERT_MESSAGE(cache.access_counter < (UINT32_MAX - 500u),
	                    "access counter should have been renormalized");

	/* Cache should still be functional */
	struct llext_res_cache_entry *entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1);
	TEST_ASSERT_NOT_NULL(entry);
	entry = LLEXT_RES_CACHE_get(&cache, TEST_KEY(2), 1);
	TEST_ASSERT_NOT_NULL(entry);
}

/* ========================================================================== */
/* =                      Key Integration with Cache Tests                  = */
/* ========================================================================== */

/** @brief Verify that generate_key + generate_key_prefix work end-to-end with invalidation. */
static void T_LLEXT_RES_CACHE_CHECK_key_integration_with_invalidation(void) {
	UTIL_print_string("LLEXT_RES_CACHE key integration with invalidation\n");

	/* Insert entries using generated keys for two resources */
	unsigned char data[] = { 0xDE, 0xAD };
	int32_t rid_a = 0;
	int32_t rid_b = 1;

	llext_res_cache_entry_key key_a0 = LLEXT_RES_CACHE_generate_key(rid_a, 0);
	llext_res_cache_entry_key key_a1 = LLEXT_RES_CACHE_generate_key(rid_a, (int64_t)CACHE_BLOCK_SIZE);
	llext_res_cache_entry_key key_b0 = LLEXT_RES_CACHE_generate_key(rid_b, 0);

	LLEXT_RES_CACHE_put(&cache, key_a0, data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, key_a1, data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, key_b0, data, sizeof(data));

	TEST_ASSERT_EQUAL_INT(3, (int)get_entries_used(&cache));

	/* Invalidate resource A using its prefix */
	llext_res_cache_entry_key prefix_a = LLEXT_RES_CACHE_generate_key_prefix(rid_a);
	LLEXT_RES_CACHE_invalidate(&cache, prefix_a, CACHE_KEY_RESOURCE_MASK);

	/* Resource A entries should be gone */
	TEST_ASSERT_NULL(LLEXT_RES_CACHE_get(&cache, key_a0, 1));
	TEST_ASSERT_NULL(LLEXT_RES_CACHE_get(&cache, key_a1, 1));

	/* Resource B entry should still exist */
	TEST_ASSERT_NOT_NULL(LLEXT_RES_CACHE_get(&cache, key_b0, 1));

	TEST_ASSERT_EQUAL_INT(1, (int)get_entries_used(&cache));
}

/* ========================================================================== */
/* =                     Boundary Condition Tests                           = */
/* ========================================================================== */

/**
 * @brief Using key=EMPTY_CACHE_KEY (0) creates a fragile entry overwritten by the next put.
 *
 * The cache reserves key=0 as EMPTY_CACHE_KEY to mark free slots. Storing data
 * with key=0 makes the entry indistinguishable from an empty slot, so subsequent
 * put operations for a different key will overwrite it.
 */
static void T_LLEXT_RES_CACHE_CHECK_put_empty_cache_key_overwritten(void) {
	UTIL_print_string("LLEXT_RES_CACHE put empty cache key overwritten\n");

	unsigned char data[] = { 0xAA, 0xBB };

	/* Put with key=0: finds an "empty" slot (all keys are 0 initially),
	 * writes data but key stays 0. */
	struct llext_res_cache_entry *entry =
		LLEXT_RES_CACHE_put(&cache, (llext_res_cache_entry_key)0, data, sizeof(data));
	TEST_ASSERT_NOT_NULL(entry);

	/* The entry can be retrieved with key=0 */
	struct llext_res_cache_entry *get_entry =
		LLEXT_RES_CACHE_get(&cache, (llext_res_cache_entry_key)0, sizeof(data));
	TEST_ASSERT_NOT_NULL(get_entry);

	/* Now put a different key: it searches for empty slots (key=0) and finds
	 * our entry because its key is also 0. Overwrites it. */
	unsigned char data2[] = { 0xCC };
	struct llext_res_cache_entry *entry2 =
		LLEXT_RES_CACHE_put(&cache, TEST_KEY(50), data2, sizeof(data2));
	TEST_ASSERT_NOT_NULL(entry2);

	/* The key=0 entry is now gone (overwritten by TEST_KEY(50)) */
	struct llext_res_cache_entry *lost =
		LLEXT_RES_CACHE_get(&cache, (llext_res_cache_entry_key)0, sizeof(data));
	TEST_ASSERT_NULL(lost);
}

/**
 * @brief get(key=0, size=0) on an empty cache returns a ghost entry.
 *
 * All initial entries have key=0 and size=0, so get(key=0, size=0) matches
 * the first slot even though no data was stored.
 */
static void T_LLEXT_RES_CACHE_CHECK_get_empty_key_zero_size_ghost(void) {
	UTIL_print_string("LLEXT_RES_CACHE get empty key zero size ghost\n");

	struct llext_res_cache_entry *ghost =
		LLEXT_RES_CACHE_get(&cache, (llext_res_cache_entry_key)0, 0);
	TEST_ASSERT_NOT_NULL(ghost);
	TEST_ASSERT_EQUAL_INT(0, (int)ghost->size);
}

/** @brief Invalidate with mask=0 and prefix=0 removes all non-empty entries. */
static void T_LLEXT_RES_CACHE_CHECK_invalidate_mask_zero(void) {
	UTIL_print_string("LLEXT_RES_CACHE invalidate mask zero\n");

	fill_cache(1);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_CACHE_MAX_ENTRIES, (int)get_entries_used(&cache));

	/* mask=0 means (entry->key & 0) == 0 == prefix for all entries */
	LLEXT_RES_CACHE_invalidate(&cache, (llext_res_cache_entry_key)0, (uintptr_t)0);

	TEST_ASSERT_EQUAL_INT(0, (int)get_entries_used(&cache));
}

/** @brief Invalidate with mask=0xFFFFFFFF removes only the exact matching key. */
static void T_LLEXT_RES_CACHE_CHECK_invalidate_exact_match_mask(void) {
	UTIL_print_string("LLEXT_RES_CACHE invalidate exact match mask\n");

	unsigned char data[] = { 0x11 };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(2), data, sizeof(data));
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(3), data, sizeof(data));

	/* Full mask: only the exact key matches */
	LLEXT_RES_CACHE_invalidate(&cache, TEST_KEY(2), (uintptr_t)0xFFFFFFFFu);

	TEST_ASSERT_NOT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1));
	TEST_ASSERT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(2), 1));
	TEST_ASSERT_NOT_NULL(LLEXT_RES_CACHE_get(&cache, TEST_KEY(3), 1));
}

/**
 * @brief generate_key with resource_id=-1 produces EMPTY_CACHE_KEY (0).
 *
 * resource_id=-1 → rid_inc=0 → (0 << 24) | 0 = 0 = EMPTY_CACHE_KEY.
 * This is a dangerous boundary: entries stored with this key behave as empty.
 */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_minus_one_collision(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key minus one collision\n");

	llext_res_cache_entry_key key = LLEXT_RES_CACHE_generate_key(-1, 0);
	TEST_ASSERT_EQUAL_INT(0, (int)key);
}

/** @brief generate_key at maximum valid offset (CACHE_MAX_OFFSET - 1). */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_max_offset(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key max offset\n");

	llext_res_cache_entry_key key = LLEXT_RES_CACHE_generate_key(0, CACHE_MAX_OFFSET - 1);
	TEST_ASSERT_MESSAGE(EMPTY_CACHE_KEY != key, "key at max offset should not be empty");

	/* Verify key structure: resource part = (0+1) << 24, offset part = aligned & 0xFFFFFF */
	int64_t aligned_offset = (CACHE_MAX_OFFSET - 1) & ~((int64_t)CACHE_BLOCK_SIZE - 1);
	uintptr_t expected_offset_part = (uintptr_t)aligned_offset & (uintptr_t)0xFFFFFFu;
	uintptr_t expected = ((uintptr_t)1u << 24U) | expected_offset_part;
	TEST_ASSERT_EQUAL_INT((int)expected, (int)key);
}

/** @brief generate_key at maximum valid resource_id (LLEXT_MAX_OPEN_RESOURCES - 1). */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_max_resource_id(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key max resource id\n");

	int32_t max_rid = LLEXT_MAX_OPEN_RESOURCES - 1;
	llext_res_cache_entry_key key = LLEXT_RES_CACHE_generate_key(max_rid, 0);
	TEST_ASSERT_MESSAGE(EMPTY_CACHE_KEY != key, "key at max resource id should not be empty");

	uintptr_t expected = (uintptr_t)(max_rid + 1) << 24U;
	TEST_ASSERT_EQUAL_INT((int)expected, (int)key);
}

/** @brief generate_key_prefix with resource_id=-1 produces prefix=0. */
static void T_LLEXT_RES_CACHE_CHECK_generate_key_prefix_minus_one(void) {
	UTIL_print_string("LLEXT_RES_CACHE generate key prefix minus one\n");

	llext_res_cache_entry_key prefix = LLEXT_RES_CACHE_generate_key_prefix(-1);
	TEST_ASSERT_EQUAL_INT(0, (int)prefix);
}

/**
 * @brief Renormalization triggered on an empty cache should be a safe no-op.
 *
 * With no valid entries (all keys=EMPTY_CACHE_KEY), renormalization finds
 * min_access=UINT32_MAX and skips adjustment. Cache must remain functional.
 */
static void T_LLEXT_RES_CACHE_CHECK_renormalization_empty_cache(void) {
	UTIL_print_string("LLEXT_RES_CACHE renormalization empty cache\n");

	/* Force access_counter above the renormalization threshold with empty cache */
	cache.access_counter = UINT32_MAX - 100u;

	/* Put triggers get_next_access_timestamp which checks the threshold.
	 * With no valid entries, renormalize is a no-op. */
	unsigned char data[] = { 0x42 };
	struct llext_res_cache_entry *entry =
		LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	TEST_ASSERT_NOT_NULL(entry);

	/* The entry should be retrievable after renormalization */
	struct llext_res_cache_entry *get_entry =
		LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), sizeof(data));
	TEST_ASSERT_NOT_NULL(get_entry);
}

/* ========================================================================== */
/* =                          Test Suite Declaration                        = */
/* ========================================================================== */

TestRef T_LLEXT_RES_CACHE_tests(void) {
	EMB_UNIT_TESTFIXTURES(fixtures) {
		/* Basic Operations */
		new_TestFixture("T_LLEXT_RES_CACHE_put_and_get", T_LLEXT_RES_CACHE_CHECK_put_and_get),
		new_TestFixture("T_LLEXT_RES_CACHE_get_nonexistent_key", T_LLEXT_RES_CACHE_CHECK_get_nonexistent_key),
		new_TestFixture("T_LLEXT_RES_CACHE_put_updates_existing", T_LLEXT_RES_CACHE_CHECK_put_updates_existing),
		/* Size Handling */
		new_TestFixture("T_LLEXT_RES_CACHE_get_size_check", T_LLEXT_RES_CACHE_CHECK_get_size_check),
		new_TestFixture("T_LLEXT_RES_CACHE_put_size_too_large", T_LLEXT_RES_CACHE_CHECK_put_size_too_large),
		new_TestFixture("T_LLEXT_RES_CACHE_put_exact_max_size", T_LLEXT_RES_CACHE_CHECK_put_exact_max_size),
		new_TestFixture("T_LLEXT_RES_CACHE_put_zero_size", T_LLEXT_RES_CACHE_CHECK_put_zero_size),
		/* LRU Eviction */
		new_TestFixture("T_LLEXT_RES_CACHE_lru_eviction_order", T_LLEXT_RES_CACHE_CHECK_lru_eviction_order),
		new_TestFixture("T_LLEXT_RES_CACHE_lru_access_updates_timestamp",
		                T_LLEXT_RES_CACHE_CHECK_lru_access_updates_timestamp),
		new_TestFixture("T_LLEXT_RES_CACHE_lru_multiple_evictions", T_LLEXT_RES_CACHE_CHECK_lru_multiple_evictions),
		/* Cache Invalidation */
		new_TestFixture("T_LLEXT_RES_CACHE_invalidate_single_entry", T_LLEXT_RES_CACHE_CHECK_invalidate_single_entry),
		new_TestFixture("T_LLEXT_RES_CACHE_invalidate_multiple_entries",
		                T_LLEXT_RES_CACHE_CHECK_invalidate_multiple_entries),
		new_TestFixture("T_LLEXT_RES_CACHE_invalidate_no_match", T_LLEXT_RES_CACHE_CHECK_invalidate_no_match),
		new_TestFixture("T_LLEXT_RES_CACHE_invalidate_empty_cache", T_LLEXT_RES_CACHE_CHECK_invalidate_empty_cache),
		new_TestFixture("T_LLEXT_RES_CACHE_invalidate_all_entries", T_LLEXT_RES_CACHE_CHECK_invalidate_all_entries),
		/* Cache Clear */
		new_TestFixture("T_LLEXT_RES_CACHE_clear_empties_cache", T_LLEXT_RES_CACHE_CHECK_clear_empties_cache),
		new_TestFixture("T_LLEXT_RES_CACHE_clear_resets_access_counter",
		                T_LLEXT_RES_CACHE_CHECK_clear_resets_access_counter),
		new_TestFixture("T_LLEXT_RES_CACHE_clear_cache_functional_after",
		                T_LLEXT_RES_CACHE_CHECK_clear_cache_functional_after),
		new_TestFixture("T_LLEXT_RES_CACHE_clear_empty_cache", T_LLEXT_RES_CACHE_CHECK_clear_empty_cache),
		/* Cache Key Addressing */
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_basic", T_LLEXT_RES_CACHE_CHECK_generate_key_basic),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_block_alignment",
		                T_LLEXT_RES_CACHE_CHECK_generate_key_block_alignment),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_different_resources",
		                T_LLEXT_RES_CACHE_CHECK_generate_key_different_resources),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_nonzero", T_LLEXT_RES_CACHE_CHECK_generate_key_nonzero),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_prefix_basic",
		                T_LLEXT_RES_CACHE_CHECK_generate_key_prefix_basic),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_prefix_matches_key",
		                T_LLEXT_RES_CACHE_CHECK_generate_key_prefix_matches_key),
		new_TestFixture("T_LLEXT_RES_CACHE_get_block_offset_at_boundary",
		                T_LLEXT_RES_CACHE_CHECK_get_block_offset_at_boundary),
		new_TestFixture("T_LLEXT_RES_CACHE_get_block_offset_within_block",
		                T_LLEXT_RES_CACHE_CHECK_get_block_offset_within_block),
		/* Key Integration */
		new_TestFixture("T_LLEXT_RES_CACHE_key_integration_with_invalidation",
		                T_LLEXT_RES_CACHE_CHECK_key_integration_with_invalidation),
		/* Edge Cases */
		new_TestFixture("T_LLEXT_RES_CACHE_empty_cache_get", T_LLEXT_RES_CACHE_CHECK_empty_cache_get),
		new_TestFixture("T_LLEXT_RES_CACHE_fill_exact_capacity", T_LLEXT_RES_CACHE_CHECK_fill_exact_capacity),
		new_TestFixture("T_LLEXT_RES_CACHE_replace_same_key_different_size",
		                T_LLEXT_RES_CACHE_CHECK_replace_same_key_different_size),
		new_TestFixture("T_LLEXT_RES_CACHE_multiple_keys_same_data", T_LLEXT_RES_CACHE_CHECK_multiple_keys_same_data),
		new_TestFixture("T_LLEXT_RES_CACHE_put_no_duplicate_with_empty_slot_before",
		                T_LLEXT_RES_CACHE_CHECK_put_no_duplicate_with_empty_slot_before),
		/* Entries Used */
		new_TestFixture("T_LLEXT_RES_CACHE_entries_used", T_LLEXT_RES_CACHE_CHECK_entries_used),
		new_TestFixture("T_LLEXT_RES_CACHE_entries_used_after_invalidation",
		                T_LLEXT_RES_CACHE_CHECK_entries_used_after_invalidation),
		new_TestFixture("T_LLEXT_RES_CACHE_entries_used_at_capacity_with_eviction",
		                T_LLEXT_RES_CACHE_CHECK_entries_used_at_capacity_with_eviction),
		/* Access Counter Renormalization */
		new_TestFixture("T_LLEXT_RES_CACHE_renormalization_preserves_lru",
		                T_LLEXT_RES_CACHE_CHECK_renormalization_preserves_lru),
		new_TestFixture("T_LLEXT_RES_CACHE_renormalization_counter_reset",
		                T_LLEXT_RES_CACHE_CHECK_renormalization_counter_reset),
		/* Boundary Conditions */
		new_TestFixture("T_LLEXT_RES_CACHE_put_empty_cache_key_overwritten",
		                T_LLEXT_RES_CACHE_CHECK_put_empty_cache_key_overwritten),
		new_TestFixture("T_LLEXT_RES_CACHE_get_empty_key_zero_size_ghost",
		                T_LLEXT_RES_CACHE_CHECK_get_empty_key_zero_size_ghost),
		new_TestFixture("T_LLEXT_RES_CACHE_invalidate_mask_zero",
		                T_LLEXT_RES_CACHE_CHECK_invalidate_mask_zero),
		new_TestFixture("T_LLEXT_RES_CACHE_invalidate_exact_match_mask",
		                T_LLEXT_RES_CACHE_CHECK_invalidate_exact_match_mask),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_minus_one_collision",
		                T_LLEXT_RES_CACHE_CHECK_generate_key_minus_one_collision),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_max_offset",
		                T_LLEXT_RES_CACHE_CHECK_generate_key_max_offset),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_max_resource_id",
		                T_LLEXT_RES_CACHE_CHECK_generate_key_max_resource_id),
		new_TestFixture("T_LLEXT_RES_CACHE_generate_key_prefix_minus_one",
		                T_LLEXT_RES_CACHE_CHECK_generate_key_prefix_minus_one),
		new_TestFixture("T_LLEXT_RES_CACHE_renormalization_empty_cache",
		                T_LLEXT_RES_CACHE_CHECK_renormalization_empty_cache),
	};
	EMB_UNIT_TESTCALLER(llext_res_cache_tests, "LLEXT_RES_CACHE tests",
	                    T_LLEXT_RES_CACHE_setUp, T_LLEXT_RES_CACHE_tearDown, fixtures);

	return (TestRef) & llext_res_cache_tests;
}

/* ========================================================================== */
/* =                         Statistics Tests                               = */
/* ========================================================================== */

#if defined(LLEXT_RES_CACHE_ENABLE_STATS)

/** @brief Stats should be zeroed after cache initialization. */
static void T_LLEXT_RES_CACHE_CHECK_stats_initial_values(void) {
	UTIL_print_string("LLEXT_RES_CACHE stats initial values\n");

	struct llext_res_cache_stats stats;
	LLEXT_RES_CACHE_get_stats(&cache, &stats);

	TEST_ASSERT_EQUAL_INT(0, (int)stats.hits);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.misses);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.evictions);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.puts);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.invalidations);
}

/** @brief Stats should track hits and misses correctly. */
static void T_LLEXT_RES_CACHE_CHECK_stats_hit_miss_tracking(void) {
	UTIL_print_string("LLEXT_RES_CACHE stats hit miss tracking\n");

	unsigned char data[] = { 0x11, 0x22 };

	/* Put should increment puts counter */
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));

	struct llext_res_cache_stats stats;
	LLEXT_RES_CACHE_get_stats(&cache, &stats);
	TEST_ASSERT_EQUAL_INT(1, (int)stats.puts);

	/* Successful get should increment hits */
	LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), sizeof(data));
	LLEXT_RES_CACHE_get_stats(&cache, &stats);
	TEST_ASSERT_EQUAL_INT(1, (int)stats.hits);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.misses);

	/* Failed get should increment misses */
	LLEXT_RES_CACHE_get(&cache, TEST_KEY(999), 1);
	LLEXT_RES_CACHE_get_stats(&cache, &stats);
	TEST_ASSERT_EQUAL_INT(1, (int)stats.hits);
	TEST_ASSERT_EQUAL_INT(1, (int)stats.misses);
}

/** @brief Stats should track evictions correctly. */
static void T_LLEXT_RES_CACHE_CHECK_stats_eviction_tracking(void) {
	UTIL_print_string("LLEXT_RES_CACHE stats eviction tracking\n");

	/* Fill cache completely */
	fill_cache(1);

	struct llext_res_cache_stats stats;
	LLEXT_RES_CACHE_get_stats(&cache, &stats);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.evictions);
	TEST_ASSERT_EQUAL_INT((int)LLEXT_RES_CACHE_MAX_ENTRIES, (int)stats.puts);

	/* Adding one more should cause eviction */
	unsigned char data[] = { 0xFF };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(100), data, sizeof(data));

	LLEXT_RES_CACHE_get_stats(&cache, &stats);
	TEST_ASSERT_EQUAL_INT(1, (int)stats.evictions);
	TEST_ASSERT_EQUAL_INT((int)(LLEXT_RES_CACHE_MAX_ENTRIES + 1u), (int)stats.puts);
}

/** @brief Stats should track invalidations correctly. */
static void T_LLEXT_RES_CACHE_CHECK_stats_invalidation_tracking(void) {
	UTIL_print_string("LLEXT_RES_CACHE stats invalidation tracking\n");

	/* Add entries for multiple resources */
	llext_res_cache_entry_key key1 = (llext_res_cache_entry_key)0x01000000;
	llext_res_cache_entry_key key2 = (llext_res_cache_entry_key)0x01000100;
	llext_res_cache_entry_key key3 = (llext_res_cache_entry_key)0x02000000;

	unsigned char data[] = { 0xAA };
	LLEXT_RES_CACHE_put(&cache, key1, data, 1);
	LLEXT_RES_CACHE_put(&cache, key2, data, 1);
	LLEXT_RES_CACHE_put(&cache, key3, data, 1);

	struct llext_res_cache_stats stats;
	LLEXT_RES_CACHE_get_stats(&cache, &stats);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.invalidations);

	/* Invalidate resource 1 (2 entries) */
	LLEXT_RES_CACHE_invalidate(&cache, (llext_res_cache_entry_key)0x01000000, 0xFF000000);

	LLEXT_RES_CACHE_get_stats(&cache, &stats);
	TEST_ASSERT_EQUAL_INT(2, (int)stats.invalidations);
}

/** @brief Stats should be reset to zero after LLEXT_RES_CACHE_clear. */
static void T_LLEXT_RES_CACHE_CHECK_stats_reset_after_clear(void) {
	UTIL_print_string("LLEXT_RES_CACHE stats reset after clear\n");

	/* Perform operations to generate stats */
	unsigned char data[] = { 0x11 };
	LLEXT_RES_CACHE_put(&cache, TEST_KEY(1), data, sizeof(data));
	LLEXT_RES_CACHE_get(&cache, TEST_KEY(1), 1);
	LLEXT_RES_CACHE_get(&cache, TEST_KEY(999), 1); /* miss */

	LLEXT_RES_CACHE_clear(&cache);

	struct llext_res_cache_stats stats;
	LLEXT_RES_CACHE_get_stats(&cache, &stats);

	TEST_ASSERT_EQUAL_INT(0, (int)stats.hits);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.misses);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.evictions);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.puts);
	TEST_ASSERT_EQUAL_INT(0, (int)stats.invalidations);
}

TestRef T_LLEXT_RES_CACHE_stats_tests(void) {
	EMB_UNIT_TESTFIXTURES(stats_fixtures) {
		new_TestFixture("T_LLEXT_RES_CACHE_stats_initial_values", T_LLEXT_RES_CACHE_CHECK_stats_initial_values),
		new_TestFixture("T_LLEXT_RES_CACHE_stats_hit_miss_tracking", T_LLEXT_RES_CACHE_CHECK_stats_hit_miss_tracking),
		new_TestFixture("T_LLEXT_RES_CACHE_stats_eviction_tracking", T_LLEXT_RES_CACHE_CHECK_stats_eviction_tracking),
		new_TestFixture("T_LLEXT_RES_CACHE_stats_invalidation_tracking",
		                T_LLEXT_RES_CACHE_CHECK_stats_invalidation_tracking),
		new_TestFixture("T_LLEXT_RES_CACHE_stats_reset_after_clear",
		                T_LLEXT_RES_CACHE_CHECK_stats_reset_after_clear),
	};
	EMB_UNIT_TESTCALLER(llext_res_cache_stats_tests, "LLEXT_RES_CACHE statistics tests",
	                    T_LLEXT_RES_CACHE_setUp, T_LLEXT_RES_CACHE_tearDown, stats_fixtures);

	return (TestRef) & llext_res_cache_stats_tests;
}

#endif /* LLEXT_RES_CACHE_ENABLE_STATS */
