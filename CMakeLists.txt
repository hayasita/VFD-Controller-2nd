cmake_minimum_required(VERSION 3.10)
project(SandProject)

# C++の標準を設定
set(CMAKE_CXX_STANDARD 17)

# Arduinoのヘッダファイルパスを指定（使わないならコメントアウト）
# include_directories("C:/Users/hayas/.platformio/packages/framework-arduino-avr/cores/arduino")
# include_directories("C:/Users/hayas/.platformio/packages/framework-arduino-avr/variants")

# GoogleTestをダウンロードしてビルド
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)


# テストフォルダのCMakeLists.txtを読み込む
add_subdirectory(test)
