# LLEXT_RES_CACHE Test Suite

## Overview

This folder contains sources to test the LRU cache implementation used by the
External Resource Loader implementation.

The ``LLEXT_RES_CACHE`` Test Suite validates the `LLEXT_RES_cache_configuration.h`
cache API by executing tests covering the following:

- Basic operations, 
- Size handling, 
- LRU eviction,
- Cache invalidation, 
- Cache clear, 
- Cache key addressing & mechanism,
- Edge cases, 
- Entries used tracking, 
- Access counter renormalization, 
- And optionally, statistics tracking. 

It also validates the `LLEXT_RES_utils` helper functions
covering:

- Resource initialization,
- Free slot lookup, 
- Resource validation,
- Path construction. 

Both suites include boundary condition tests that pinpoint
KO limits of the API (reserved key collisions, extreme parameter values,
mask-based invalidation edge cases, path length overflow).

## Requirements

- Follow the [main Readme](https://github.com/MicroEJ/Tool-Project-Template-VEEPort/blob/master/vee-port/validation/README.md).

## Configuration

1. In your BSP project, add all files of these folders as source files:

   * `tests/llext-res-cache/src/`
   * `framework/utils/src/`
   * `framework/embunit/embUnit/`

2. In your BSP project, add these folders as include paths:

   * `tests/llext-res-cache/inc/`
   * `framework/utils/inc/`
   * `framework/embunit/embUnit/`
   * The folder containing `LLEXT_RES_cache_configuration.h`

3. Locate the call to `microej_main` in the BSP project. Include the `t_llext_res_cache_main.h` header file in this file, and add a call to the function `T_LLEXT_RES_CACHE_main` just before the call to `microej_main`.
4. Build the BSP and link it with the Architecture library and the Application.

## Expected Results

```
T_LLEXT_RES_CACHE v1.0.0
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE put and get
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE get nonexistent key
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE put updates existing
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE get size check
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE put size too large
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE put exact max size
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE put zero size
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE lru eviction order
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE lru access updates timestamp
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE lru multiple evictions
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE invalidate single entry
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE invalidate multiple entries
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE invalidate no match
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE invalidate empty cache
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE invalidate all entries
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE clear empties cache
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE clear resets access counter
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE clear cache functional after
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE clear empty cache
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key basic
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key block alignment
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key different resources
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key nonzero
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key prefix basic
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key prefix matches key
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE get block offset at boundary
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE get block offset within block
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE key integration with invalidation
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE empty cache get
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE fill exact capacity
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE replace same key different size
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE multiple keys same data
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE put no duplicate with empty slot before
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE entries used
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE entries used after invalidation
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE entries used at capacity with eviction
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE renormalization preserves LRU order
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE renormalization counter reset
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE put empty cache key overwritten
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE get empty key zero size ghost
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE invalidate mask zero
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE invalidate exact match mask
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key minus one collision
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key max offset
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key max resource id
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE generate key prefix minus one
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE renormalization empty cache
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS initialize resources
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS initialize resources idempotent
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS find free slot all free
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS find free slot some used
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS find free slot all used
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS find free slot last free
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS validate resource valid
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS validate resource last slot
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS validate resource id zero
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS validate resource negative id
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS validate resource id too large
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS validate resource slot closed
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS build full path basic
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS build full path leading slash
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS validate resource INT32_MIN
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS validate resource INT32_MAX
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS build full path empty
	T_LLEXT_RES_UTILS_tearDown
	.
	T_LLEXT_RES_UTILS_setUp
	LLEXT_RES_UTILS build full path too long
	T_LLEXT_RES_UTILS_tearDown

	OK (65 tests)
```

When `LLEXT_RES_CACHE_ENABLE_STATS` is defined, 5 additional statistics tests are
executed between the cache tests and the utils tests:

```
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE stats initial values
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE stats hit miss tracking
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE stats eviction tracking
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE stats invalidation tracking
	T_LLEXT_RES_CACHE_tearDown
	.
	T_LLEXT_RES_CACHE_setUp
	LLEXT_RES_CACHE stats reset after clear
	T_LLEXT_RES_CACHE_tearDown

	OK (70 tests)
```

No tests should fail when executing this test.

---
_Copyright 2026 MicroEJ Corp. All rights reserved._
_Use of this source code is governed by a BSD-style license that can be found with this software._
_Build: 7E4D1F7C_
