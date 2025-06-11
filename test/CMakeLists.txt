# テストソースファイル
set(PARAMETERTEST_SOURCES
    test_parameter_manager.cpp
    mock/EEPROM.cpp  # モックもビルド対象に追加
)

# CMakeのバージョンを指定
cmake_minimum_required(VERSION 3.10)

#ParameterManagerのテストを行うための設定
# テスト実行ファイルを作成
add_executable(ParameterManTest
    ${PARAMETERTEST_SOURCES}
)

# モック用ヘッダをインクルードパスに追加
target_include_directories(ParameterManTest PRIVATE
    ${CMAKE_SOURCE_DIR}/test/mock
    ${CMAKE_SOURCE_DIR}/src
)

# ソースコードをライブラリとして追加
add_library(ParameterManLib
    ../src/EepromManager.cpp
    ../src/LogManager.cpp
    ../src/LogStorage.cpp
    ../src/ParameterManager.cpp
    ../src/ParameterStorage.cpp
    ../src/SerialCommandProcessor.cpp
    ../src/SerialCommandProcessor.cpp     # 追加
    ../src/WiFiManager.cpp
    ./mock/EepromRawAccessor.cpp
    ./mock/I2CBusManager.cpp
    ./mock/EEPROM.cpp
)

# ←ここでモックパス追加！！
target_include_directories(ParameterManLib PUBLIC
    ${CMAKE_SOURCE_DIR}/test/mock
)

# ライブラリリンク
target_link_libraries(ParameterManTest
    ParameterManLib
    gtest
    gtest_main
)

# マクロ定義
target_compile_definitions(ParameterManTest PRIVATE UNIT_TEST)
target_compile_definitions(ParameterManLib PRIVATE UNIT_TEST)

# SerialCommandProcessorのテスト
set(SERIALCOMMANDPROCESSOR_SOURCES
    test_SerialCommandProcessor.cpp
)
add_executable(SerialCommandProcessorTest
    ${SERIALCOMMANDPROCESSOR_SOURCES}
)

target_include_directories(SerialCommandProcessorTest PRIVATE
    ${CMAKE_SOURCE_DIR}/test/mock
)

target_compile_definitions(SerialCommandProcessorTest PRIVATE UNIT_TEST)

target_link_libraries(SerialCommandProcessorTest
    ParameterManLib
    gmock
    gmock_main
    gtest
    gtest_main
)

#target_compile_definitions(EepromManTest PRIVATE UNIT_TEST)

#
#set(EEPROMTEST_SOURCES
#    test_eeprom_manager.cpp
#)
#add_executable(EepromManTest
#    ${EEPROMTEST_SOURCES}
#)
#target_include_directories(EepromManTest PRIVATE
#    ${CMAKE_SOURCE_DIR}/test/mock
#)

#target_link_libraries(EepromManTest
#    ParameterManLib
#    gtest
#    gtest_main
#)

#target_compile_definitions(EepromManTest PRIVATE UNIT_TEST)

# テスト有効化
enable_testing()
add_test(NAME ParameterManTest COMMAND ParameterManTest)
#add_test(NAME EepromManTest COMMAND EepromManTest)
