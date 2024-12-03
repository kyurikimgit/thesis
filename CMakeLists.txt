# 최소 CMake 버전 설정
cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름 및 C++ 표준 설정
project(BoostGraphExample)
set(CMAKE_CXX_STANDARD 17)

# vcpkg를 사용한 Boost 경로 설정 (Boost 라이브러리 경로를 명시적으로 지정)
set(BOOST_ROOT "C:/Users/rlarb/boost_1_86_0/vcpkg/installed/x64-windows")
set(Boost_INCLUDE_DIR "C:/Users/rlarb/boost_1_86_0/vcpkg/installed/x64-windows/include")
set(Boost_LIBRARY_DIR "C:/Users/rlarb/boost_1_86_0/vcpkg/installed/x64-windows/lib")

# Boost 라이브러리 찾기
find_package(Boost 1.86 REQUIRED COMPONENTS graph)

# 실행 파일 만들기
add_executable(main main.cpp)

# Boost 라이브러리 링크
target_link_libraries(main Boost::graph)

# CMakeLists.txt에 vcpkg 툴체인 경로 추가
#set(CMAKE_TOOLCHAIN_FILE "C:/Users/rlarb/boost_1_86_0/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

