cmake_minimum_required(VERSION 3.27)
project(VerleEngine VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")


find_package(SFML COMPONENTS system window graphics audio)
find_package(ImGui-SFML)
find_package(imgui CONFIG REQUIRED)

add_executable(main src/main.cpp)


target_include_directories(main PUBLIC "${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include")
# target_compile_definitions(main PRIVATE PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")


target_link_libraries(main
  PRIVATE ImGui-SFML::ImGui-SFML
  PRIVATE ${SFML_LIBRARIES}
  PRIVATE ${SFML_DEPENDENCIES}
  )
