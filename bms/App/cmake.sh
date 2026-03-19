#!/bin/bash


gen_lib_cmake() {
    local dir=$1
    local name=$2
    cat <<EOF > "$dir/CMakeLists.txt"

add_library($name STATIC)

file(GLOB SRC_FILES "src/*.c")
target_sources($name PRIVATE \${SRC_FILES})

target_include_directories($name PUBLIC inc)
EOF
}

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

for d in lib/*/; do
    [ -d "$d" ] || continue
    mod=$(basename "$d")
    gen_lib_cmake "$d" "$mod"
done

for folder in api math core; do
    for d in "$folder"/*/; do
        [ -d "$d" ] || continue
        mod=$(basename "$d")
        gen_flat_cmake "$d" "$mod"
    done
done