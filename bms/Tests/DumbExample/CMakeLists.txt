add_executable(DumbExample_app
    TestDumbExample.c
)

target_link_libraries(DumbExample_app
  PRIVATE
    DumbExample_lib
    Unity
)

add_test(NAME DumbExample_test COMMAND DumbExample_app)
