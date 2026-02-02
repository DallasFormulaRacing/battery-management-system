add_executable(ThermistorTest_app
    TestThermistor.c
)

target_link_libraries(ThermistorTest_app
  PRIVATE
    bms_api
    Unity
    bms_thermistor
)

target_include_directories(ThermistorTest_app
  PUBLIC
    ${CMAKE_SOURCE_DIR}/App/lib/api/include
    ${CMAKE_SOURCE_DIR}/App/algorithms/thermal
)

add_test(NAME Thermistor_test COMMAND ThermistorTest_app)
