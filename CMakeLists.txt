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
find_package(OpenGL REQUIRED)

# *.*.*.X : incremental updates, not necessarily compiling
# *.*.X.0 : bugfix updates, guaranteed compiling "on my machine"
# *.X.0.0 : minor updates, guaranteed compiling "on my machine"
# X.0.0.0 : major updates, guaranteed compiling on my desktop & laptop
project(MGEALite LANGUAGES CXX VERSION 0.4.0.2)
configure_file(mgealite_version.cpp.in mgealite_version.cpp @ONLY)

FetchContent_Declare(DEvA GIT_REPOSITORY https://github.com/vdaghan/DEvA.git GIT_TAG 2fe9f4e5b559c3cc7a5717b4869c42c4ab6a883a GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/DEvA) # 0.4.5.2
FetchContent_Declare(DTimer GIT_REPOSITORY https://github.com/vdaghan/DTimer.git GIT_TAG 03c3a8cd8f3d0c5976a2d596c41979318c5a6a08 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/DTimer) # 0.1.0.1
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG eb3220622e73a4889eee355ffa37972b3cac3df5 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/spdlog) # release-1.9.2
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 7482de6071d21db77a7236155da44c172a7f6c9e GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/glfw) # release-3.3.8
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG 9aae45eb4a05a5a1f96be1ef37eb503a12ceb889 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/imgui) # release-1.88
FetchContent_Declare(implot GIT_REPOSITORY https://github.com/epezent/implot.git GIT_TAG 15e494b76a78b44ae2c1b76608ff9bc39a661409 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/implot) # release-0.14
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG 4f8fba14066156b73f1189a2b8bd568bde5284c5 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/json) # release-3.10.5
set(DEvA_BUILD_TESTS FALSE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog DEvA DTimer json glfw imgui implot)

add_executable(MGEALite)
set(Logging_SOURCES "${CMAKE_SOURCE_DIR}/src/Logging/SpdlogCommon.cpp")
set(MotionGeneration_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionGenerationState.cpp"
							 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionGenerator.cpp"
							 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionGenerator_EAFunctions.cpp"
							 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionParameters.cpp")
set(Initialisers_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Initialisers/Boundary.cpp"
						 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Initialisers/BoundaryWavelet.cpp"
						 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Initialisers/Random.cpp")
set(Variations_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/CrossoverAll.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/CrossoverSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/CutAndCrossfillAll.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/CutAndCrossfillSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/DeletionAll.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/DeletionSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/InsertionAll.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/InsertionSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/kPointCrossoverAll.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/kPointCrossoverSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/SNV.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/UniformCrossoverAll.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/UniformCrossoverSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/WaveletSNV.cpp")
set(Wavelet_SOURCES "${CMAKE_SOURCE_DIR}/src/Wavelet/HaarWavelet.cpp")
set(MGEALite_SOURCES "${CMAKE_BINARY_DIR}/mgealite_version.cpp"
					"${CMAKE_SOURCE_DIR}/src/MGEALite.cpp"
					"${CMAKE_SOURCE_DIR}/src/SimulationData.cpp"
					"${CMAKE_SOURCE_DIR}/src/SimulationLog.cpp"
					"${CMAKE_SOURCE_DIR}/src/Database.cpp"
					"${CMAKE_SOURCE_DIR}/src/Datastore.cpp"
					"${CMAKE_SOURCE_DIR}/src/SharedSynchronisation.cpp")
set(IMGUI_SOURCES   "${imgui_SOURCE_DIR}/imgui.cpp"
					"${imgui_SOURCE_DIR}/imgui_demo.cpp"
					"${imgui_SOURCE_DIR}/imgui_draw.cpp"
					"${imgui_SOURCE_DIR}/imgui_tables.cpp"
					"${imgui_SOURCE_DIR}/imgui_widgets.cpp"
					"${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
					"${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
					"${implot_SOURCE_DIR}/implot.cpp"
					"${implot_SOURCE_DIR}/implot_items.cpp"
					"${implot_SOURCE_DIR}/implot_demo.cpp")
set(GUI_SOURCES "${CMAKE_SOURCE_DIR}/src/GUI/GUIState.cpp"
				"${CMAKE_SOURCE_DIR}/src/GUI/GUIStateDrawer.cpp"
				"${CMAKE_SOURCE_DIR}/src/GUI/Initialisation.cpp"
				"${CMAKE_SOURCE_DIR}/src/GUI/ImGuiExtensions.cpp"
				"${CMAKE_SOURCE_DIR}/src/GUI/ImPlotExtensions.cpp")
target_sources(MGEALite PUBLIC ${Logging_SOURCES})
target_sources(MGEALite PUBLIC ${MotionGeneration_SOURCES})
target_sources(MGEALite PUBLIC ${Initialisers_SOURCES})
target_sources(MGEALite PUBLIC ${Variations_SOURCES})
target_sources(MGEALite PUBLIC ${Wavelet_SOURCES})
target_sources(MGEALite PUBLIC ${MGEALite_SOURCES})
target_sources(MGEALite PUBLIC ${IMGUI_SOURCES})
target_sources(MGEALite PUBLIC ${GUI_SOURCES})
target_include_directories(MGEALite PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(MGEALite PUBLIC "${CMAKE_SOURCE_DIR}/include")

if(WIN32)
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
   #set_target_properties(MGEALite PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   #set_target_properties(MGEALite PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
endif(WIN32)

#target_include_directories(MGEALite SYSTEM INTERFACE "${DTimer_SOURCE_DIR}/include")
target_include_directories(MGEALite SYSTEM INTERFACE "${spdlog_SOURCE_DIR}/include")
target_include_directories(MGEALite SYSTEM INTERFACE "${json_SOURCE_DIR}/include")
target_include_directories(MGEALite PUBLIC "${glfw_SOURCE_DIR}/include")
target_include_directories(MGEALite PUBLIC "${imgui_SOURCE_DIR}")
target_include_directories(MGEALite PUBLIC "${imgui_SOURCE_DIR}/backends")
target_include_directories(MGEALite PUBLIC "${implot_SOURCE_DIR}")

target_link_libraries(MGEALite DEvA::DEvA)
target_link_libraries(MGEALite DTimer)
target_link_libraries(MGEALite spdlog::spdlog)
target_link_libraries(MGEALite nlohmann_json::nlohmann_json)
#target_link_libraries(MGEALite OpenGL::GL)
target_link_libraries(MGEALite glfw opengl32 gdi32)

file(GLOB Data_Directory "${CMAKE_SOURCE_DIR}/data/*")
file(COPY ${Data_Directory} DESTINATION ${CMAKE_BINARY_DIR}/data)
