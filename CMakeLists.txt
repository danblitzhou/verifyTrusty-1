cmake_minimum_required(VERSION 3.18.4)
project(verifyTrusty C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvmir)
include(LLVMIRUtil)
include(CTest)

# Enable CTest
enable_testing()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
  message (FATAL_ERROR
    "In-source builds are not allowed. Please clean your source tree and try again.")
endif()

# Default is release with debug info
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(SEAHORN_ROOT "/usr" CACHE PATH "Path to SeaHorn installation")
set(SEA_LINK "llvm-link" CACHE STRING "Path to llvm-link")
set(LLVMIR_LINK ${SEA_LINK})
set(SEA_OPT "${SEAHORN_ROOT}/bin/seaopt" CACHE STRING  "Path to seaopt binary")
set(SEA_PP "${SEAHORN_ROOT}/bin/seapp" CACHE STRING  "Path to seapp binary")
set(LLVMIR_OPT ${SEA_OPT})

set(TRUSTY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/trusty)
set(TRUSTY_TARGET "arm32" CACHE STRING "Trusty target for verification")
set(EXTERNAL_ROOT ${TRUSTY_ROOT}/external)

# Set gcc-toolchain directory
if (${TRUSTY_TARGET} STREQUAL "arm32")
  set(GCC_TOOLCHAIN_DIR
    ${TRUSTY_BUILD_ROOT}/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/)
  set(TARGET_ARCH "arm")
elseif(${TRUSTY_TARGET} STREQUAL "x86_64")
  set(TARGET_ARCH "${TRUSTY_TARGET}")
  set(GCC_TOOLCHAIN_DIR
  ${TRUSTY_BUILD_ROOT}/prebuilts/gcc/linux-x86/x86/x86_64-linux-android-4.9/)
else() # arm64 = aarch64
  set(TARGET_ARCH "aarch64")
  set(GCC_TOOLCHAIN_DIR
  ${TRUSTY_BUILD_ROOT}/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/)
endif()

set(SEA_LIB ${CMAKE_CURRENT_SOURCE_DIR}/seahorn/lib)

configure_file(verify.py.in verify @ONLY)
set(VERIFY_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/verify-trusty.sh)
set(VERIFY_CMD ${CMAKE_CURRENT_BINARY_DIR}/verify)

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include)

include_directories(seahorn/include)

# Include header's directories
include_directories(${TRUSTY_ROOT}/trusty/user/app/storage)
include_directories(${TRUSTY_ROOT}/trusty/user/base/interface/storage/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/include/uapi)
include_directories(${TRUSTY_ROOT}/trusty/user/base/include/shared)
include_directories(${TRUSTY_ROOT}/trusty/user/base/include/user)
include_directories(${TRUSTY_ROOT}/trusty/kernel/include/uapi/)
include_directories(${TRUSTY_ROOT}/external/lk/include/shared/)
include_directories(${TRUSTY_ROOT}/external/lk/include/uapi/)

include_directories(${SEAHORN_ROOT}/include)
add_subdirectory(seahorn)
