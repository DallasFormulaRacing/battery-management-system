add_library(Unity STATIC
    CMock/vendor/unity/src/unity.c
)

target_include_directories(Unity PUBLIC
    CMock/vendor/unity/src
)
