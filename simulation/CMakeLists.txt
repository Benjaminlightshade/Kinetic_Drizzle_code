cmake_minimum_required(VERSION 3.16)

project(rain_compute_pc LANGUAGES CXX)

# -----------------------------
# Global settings
# -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Automatically export symbols on Windows if not manually exported
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# -----------------------------
# Library target
# -----------------------------
add_library(rain_compute SHARED
    ../main/rain_compute.cpp
)

# -----------------------------
# Include paths
# -----------------------------
target_include_directories(rain_compute
    PUBLIC
        ../main
)

# -----------------------------
# Compile definitions
# -----------------------------
target_compile_definitions(rain_compute
    PRIVATE
        PLATFORM_PC
        RAIN_COMPUTE_EXPORTS
)

# -----------------------------
# Static GCC runtime (important for Python ctypes)
# -----------------------------
if(MINGW)
    target_link_options(rain_compute PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

# -----------------------------
# Output naming
# -----------------------------
set_target_properties(rain_compute PROPERTIES
    OUTPUT_NAME "rain_compute"
)