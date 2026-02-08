#!/bin/bash

# 1. Function for Libs (Modules with /inc and /src)
gen_lib_cmake() {
    local dir=$1
    local name=$2
    cat <<EOF > "$dir/CMakeLists.txt"
# Auto-generated CMake for Library Module: $name
add_library($name STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources($name PRIVATE \${SRC_FILES})

target_include_directories($name PUBLIC inc)
EOF
}

# 2. Function for APIs/Math/Core (Flat structure)
gen_flat_cmake() {
    local dir=$1
    local name=$2
    cat <<EOF > "$dir/CMakeLists.txt"
# Auto-generated CMake for Flat Module: $name
add_library($name STATIC)

file(GLOB SRC_FILES "*.c")
target_sources($name PRIVATE \${SRC_FILES})

target_include_directories($name PUBLIC .)
EOF
}

# --- EXECUTION ---

# Generate for LIB folder
for d in lib/*/; do
    [ -d "$d" ] || continue
    mod=$(basename "$d")
    gen_lib_cmake "$d" "$mod"
done

# Generate for API, MATH, and CORE
# This loop now specifically looks inside the core/ subdirectories
for folder in api math core; do
    for d in "$folder"/*/; do
        [ -d "$d" ] || continue
        mod=$(basename "$d")
        gen_flat_cmake "$d" "$mod"
    done
done

echo "Success: Core peripherals (spi, can, etc.) and all other modules updated."