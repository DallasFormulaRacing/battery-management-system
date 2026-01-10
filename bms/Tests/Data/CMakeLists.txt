add_executable(DataTest_app
    TestData.c
)

target_link_libraries(DataTest_app
  PRIVATE
    bms_api
    Unity
)

target_include_directories(DataTest_app
  PUBLIC
    ${CMAKE_SOURCE_DIR}/App/lib/api/include
)

add_test(NAME Data_test COMMAND DataTest_app)
