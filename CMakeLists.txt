cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 23)
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
project(MGEALite LANGUAGES CXX VERSION 0.3.0.0)
configure_file(mgealite_version.cpp.in mgealite_version.cpp @ONLY)

FetchContent_Declare(DEvA GIT_REPOSITORY https://github.com/vdaghan/DEvA.git GIT_TAG 470ee031eb03be74961e0ce95e97cae5dd0c3029 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/DEvA) # 0.4.4.0
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG eb3220622e73a4889eee355ffa37972b3cac3df5 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/spdlog) # release-1.9.2
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG 4f8fba14066156b73f1189a2b8bd568bde5284c5 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/json) # release-3.10.5
set(DEvA_BUILD_TESTS FALSE)
FetchContent_MakeAvailable(spdlog DEvA json)

add_executable(MGEALite)
set(Logging_SOURCES "")
set(MotionGeneration_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionGenerator.cpp")
set(Variations_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/CrossoverAll.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/CrossoverSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/CutAndCrossfillAll.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/CutAndCrossfillSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/SNV.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/WaveletSNV.cpp")
set(Wavelet_SOURCES "${CMAKE_SOURCE_DIR}/src/Wavelet/wavelet.cpp")
set(MGEALite_SOURCES "${CMAKE_BINARY_DIR}/mgealite_version.cpp"
					"${CMAKE_SOURCE_DIR}/src/MGEALite.cpp"
					"${CMAKE_SOURCE_DIR}/src/SimulationData.cpp"
					"${CMAKE_SOURCE_DIR}/src/SimulationLog.cpp"
					"${CMAKE_SOURCE_DIR}/src/Database.cpp"
					"${CMAKE_SOURCE_DIR}/src/Datastore.cpp")
target_sources(MGEALite PUBLIC ${Logging_SOURCES})
target_sources(MGEALite PUBLIC ${MotionGeneration_SOURCES})
target_sources(MGEALite PUBLIC ${Variations_SOURCES})
#target_sources(MGEALite PUBLIC ${Wavelet_SOURCES})
target_sources(MGEALite PUBLIC ${MGEALite_SOURCES})
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
