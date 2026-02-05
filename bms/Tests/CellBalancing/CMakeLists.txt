add_executable(CellBalancingTest_app
    TestCellBalancing.c
)

target_link_libraries(CellBalancingTest_app
  PRIVATE
    bms_api
    Unity
)

target_include_directories(CellBalancingTest_app
  PUBLIC
    ${CMAKE_SOURCE_DIR}/App/lib/api/include
)

add_test(NAME CellBalancing_test COMMAND CellBalancingTest_app)
