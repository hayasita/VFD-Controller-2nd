# 本体ライブラリ
add_library(ParameterManLib
    ../src/EepromManager.cpp
    ../src/LogManager.cpp
    ../src/LogStorage.cpp
    ../src/ParameterManager.cpp
    ../src/ParameterStorage.cpp
    ../src/SerialCommandProcessor.cpp
    ../src/systemManager.cpp
    ../src/LedManager.cpp
    ../src/WiFiManager.cpp
)
target_include_directories(ParameterManLib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)
target_compile_definitions(ParameterManLib PRIVATE UNIT_TEST)

# 未使用パラメータだけ警告を許容
target_compile_options(ParameterManLib PRIVATE -Wno-error=unused-parameter)

# モックライブラリ
add_library(MockLib
    mock/EEPROM.cpp
    mock/EepromRawAccessor.cpp
    mock/I2CBusManager.cpp
)
target_include_directories(MockLib PUBLIC
    ${CMAKE_SOURCE_DIR}/test/mock
)
target_compile_definitions(MockLib PRIVATE UNIT_TEST)

# モックは全警告を許容
target_compile_options(MockLib PRIVATE -Wno-error)

# 本体ライブラリにモックをリンク
target_link_libraries(ParameterManLib PUBLIC MockLib)

# 共通関数でテスト追加
function(add_unit_test name sources use_gmock)
  add_executable(${name} ${sources})
  # テストから本体ソースやモックへアクセスできるようにする
  target_include_directories(${name} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/test/mock
  )

  target_compile_definitions(${name} PRIVATE UNIT_TEST)
  if(use_gmock)
    target_link_libraries(${name} PRIVATE GTest::gmock GTest::gmock_main)
  else()
    target_link_libraries(${name} PRIVATE GTest::gtest_main GTest::gtest)
  endif()

  # 本体ライブラリとモックもリンク
  target_link_libraries(${name} PRIVATE ParameterManLib MockLib)

  add_test(NAME ${name} COMMAND ${name})
endfunction()

# テスト追加
add_unit_test(ParameterManTest "test_parameter_manager.cpp" OFF)
add_unit_test(EepromManTest "test_eeprom_manager.cpp" OFF)
add_unit_test(SerialCmdProcTest "test_SerialCommandProcessor.cpp" ON)
#add_unit_test(SystemControlerTest "test_system_controller.cpp" OFF)

