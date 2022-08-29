cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(ExternalProject)
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# *.*.*.X : incremental updates, not necessarily compiling
# *.*.X.0 : bugfix updates, guaranteed compiling "on my machine"
# *.X.0.0 : minor updates, guaranteed compiling "on my machine"
# X.0.0.0 : major updates, guaranteed compiling on my desktop & laptop
project(MGEALite LANGUAGES CXX VERSION 0.0.0.2)
configure_file(mgealite_version.cpp.in mgealite_version.cpp @ONLY)

FetchContent_Declare(DEvA GIT_REPOSITORY https://github.com/vdaghan/DEvA.git GIT_TAG 97d18c31e6f8fe127ec4954563888d8914747153 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/DEvA) # 0.1.1.0
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG eb3220622e73a4889eee355ffa37972b3cac3df5 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/spdlog) # release-1.9.2
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG 4f8fba14066156b73f1189a2b8bd568bde5284c5 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/json) # release-3.10.5
FetchContent_MakeAvailable(spdlog DEvA json)


set(MGEALite_SOURCES "${CMAKE_BINARY_DIR}/mgealite_version.cpp" "src/MGEALite.cpp" "src/SimulationData.cpp" "src/SimulationLog.cpp" "src/Database.cpp")
add_executable(MGEALite ${MGEALite_SOURCES})
target_include_directories(MGEALite PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(MGEALite PUBLIC "${CMAKE_SOURCE_DIR}/include")

if(WIN32)
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
endif(WIN32)

target_include_directories(MGEALite SYSTEM INTERFACE "${spdlog_SOURCE_DIR}/include")
target_include_directories(MGEALite SYSTEM INTERFACE "${json_SOURCE_DIR}/include")

target_link_libraries(MGEALite DEvA::DEvA)
target_link_libraries(MGEALite spdlog::spdlog)
target_link_libraries(MGEALite nlohmann_json::nlohmann_json)

file(GLOB Data_Directory "${CMAKE_SOURCE_DIR}/data/*")
file(COPY ${Data_Directory} DESTINATION ${CMAKE_BINARY_DIR}/data)
