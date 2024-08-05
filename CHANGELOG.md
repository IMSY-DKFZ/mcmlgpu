# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

### Added

- Adds Cpack to CMake to package .deb file

### Changed

-

### Removed

-

### Fixed

-

## [0.0.4]

### Added
- Added new macro definition for CUDA_SAFE_CALL_INFO to print additional information when an error occurs.
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/b957acc240fd0b014c1136b13811d7ec260fccdd)
- MCML now expects an MCO file instead of an MCO FOLDER as input.
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/9689ad1fab47407d5fe649677420f9e1fd35c561)
- Added conan package
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/a5573e170f1795ca3dfb523386557649e509a545)
- Adds editor config and pre-commit hooks
- Added `-V` flag to print software version and build information

### Changed
- Modified path for safe_primes installation to the same path as MCML
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/c310827b19735bb81cd6c9ed08d3a82acbd5feff)
- Defined path to safeprimes relative to the executable path
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/91759c9207f714db4274bf5a2499684b7dce8a36)

### Removed
- Removed elapsed time computation as it was not used anymore.
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/b957acc240fd0b014c1136b13811d7ec260fccdd)
- Removed deprecated information from README.md
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/85ada768be4a3fc447726294912af4da619c9390)
- Removed dependency from the definition of __CUDA_ARCH__
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/078ef4a7e0a62585347baa2c3677177b234eb26b)
- Removed unused include statements.
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/078ef4a7e0a62585347baa2c3677177b234eb26b)

### Fixed
- Fixed an issue of illegal memory access by removing duplicated memory allocation for the number of photons left in the
  host state struct (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/1eb40b22e0828ddc43ac55a5b9eee01c22d4ea08)
- Fixed path to safeprimes in init_RNG
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/ad34a281cc74303134c07421024bb5e3a53995b2)
- Fixed penetration depth calculations (issue #13).
  (https://git.dkfz.de/imsy/issi/mcmlgpu/-/commit/a0c99b3b8d2659a07324dbef73cafe0b4602e005)

## [v0.0.3]

### Added

- Modern CMake compilation and installation
- Added the possibility to package GPUMCML using conan

### Changed

- Refactored project to contain all code inside src/

### Fixed

- Computation of penetration depth

### Removed

- dropped support for older versions of CUDA architectures

## [v0.0.2]

### Added

- Created a dockerfile to build GPUMCML

### Changed

- Included tqdm for progress tracking isntead of printing the progress

### Removed

-

## [v0.0.1]

- This is the original GPUMCML version. Only minor changes have been changed in this version, such changes are described below.
- Penetration depth calculations have been added.
- Progress bar is also displayed when running the simulations instead of printing all the outputs after each simulation
