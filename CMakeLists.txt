cmake_minimum_required(VERSION 3.10)
project(SandProject)

# C++の標準を設定
set(CMAKE_CXX_STANDARD 17)

#ビルドタイプと警告設定
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
add_compile_options(-Wall -Wextra -Werror)

#GoogleTest/GoogleMockの取得方法を選択
#option (USE_FETCHCONTENT "Use FetchContent to get GoogleTest" OFF)
set(USE_FETCHCONTENT ON)  # ← ここを ON に変更

if(USE_FETCHCONTENT)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )

  FetchContent_MakeAvailable(googletest)
else()
  # MSYS2でインストール済みのGTest/GMockを利用
  find_package(GTest REQUIRED)
endif()

enable_testing()

# テストフォルダのCMakeLists.txtを読み込む
add_subdirectory(test)
