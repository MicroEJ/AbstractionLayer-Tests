# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2025-10-08

### Added

- Add C Runtime Mock Tests for Core validation.

### Changed

- Update the Core tests documentation to include C Runtime Mock Tests.

## [1.1.0] - 2025-07-16

### Added

- Add Git hooks configuration file: ``.pre-commit-config.yaml``.
- Add Uncrustify analysis to the CI.

### Changed

- Core C Tests: execute RAM speed tests on 32 bit accesses by default.
- Core C Tests: update include paths to not use relative paths.
- Format sources using Uncrustify.

### Fixed

- Core C Tests: Fix RAM speed unit computation in [t_core_ram.c](tests/core/c/src/t_core_ram.c) and [x_ram_speed.c](tests/core/c/src/x_ram_speed.c).
- Core C Tests: Fix ``T_CORE_TIME_BASE_check_time_evolution()`` in [t_core_time_base.c](tests/core/c/src/t_core_time_base.c) to not fail when code optimizations are enabled.

## [1.0.0] - 2025-04-07

### Changed

- Update llkernel tests to version 1.2.0. Fix T_LLKERNEL_CHECK_resource_allocate_overflow and T_LLKERNEL_CHECK_resource_install_out_of_bounds tests.

### Added

- Initial revision.

---
_Copyright 2024-2025 MicroEJ Corp. All rights reserved._
_Use of this source code is governed by a BSD-style license that can be found with this software._  
