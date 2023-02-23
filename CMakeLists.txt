cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(ExternalProject)
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
find_package(OpenGL REQUIRED)

# *.*.*.X : incremental updates, not necessarily compiling
# *.*.X.0 : bugfix updates, guaranteed compiling "on my machine"
# *.X.0.0 : minor updates, guaranteed compiling "on my machine"
# X.0.0.0 : major updates, guaranteed compiling on my desktop & laptop
project(MGEALite LANGUAGES CXX VERSION 0.5.0.2)
configure_file(mgealite_version.cpp.in mgealite_version.cpp @ONLY)

add_custom_target(MGEADependencies COMMAND dot -Tsvg ${CMAKE_BINARY_DIR}/MGEALite.dot -o ${CMAKE_BINARY_DIR}/MGEALite.svg)

FetchContent_Declare(DEvA GIT_REPOSITORY https://github.com/vdaghan/DEvA.git GIT_TAG 5bdc0edb8e1fc4d35f12058854026252e79e6504 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/DEvA) # 0.5.0.13
FetchContent_Declare(DTimer GIT_REPOSITORY https://github.com/vdaghan/DTimer.git GIT_TAG 03c3a8cd8f3d0c5976a2d596c41979318c5a6a08 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/DTimer) # 0.1.0.1
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG 76fb40d95455f249bd70824ecfcae7a8f0930fa3 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/spdlog) # release-1.10.0
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 7482de6071d21db77a7236155da44c172a7f6c9e GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/glfw) # release-3.3.8
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG 9aae45eb4a05a5a1f96be1ef37eb503a12ceb889 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/imgui) # release-1.88
FetchContent_Declare(implot GIT_REPOSITORY https://github.com/epezent/implot.git GIT_TAG 15e494b76a78b44ae2c1b76608ff9bc39a661409 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/implot) # release-0.14
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG 4f8fba14066156b73f1189a2b8bd568bde5284c5 GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/json) # release-3.10.5
set(DEvA_BUILD_TESTS FALSE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog DEvA DTimer json glfw imgui implot)

set(Logging_SOURCES "${CMAKE_SOURCE_DIR}/src/Logging/SpdlogCommon.cpp")
set(JSON_SOURCES "${CMAKE_SOURCE_DIR}/src/JSON/SimulationDataPtr.cpp")
set(MotionGeneration_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/ContactParameters.cpp"
							 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionGenerator.cpp"
							 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionGenerator_EAFunctions.cpp"
							 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionGenerator_MetricFunctors.cpp"
							 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/MotionParameters.cpp")
set(Initialisers_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Initialisers/Boundary.cpp"
						 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Initialisers/BoundaryWavelet.cpp"
						 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Initialisers/Random.cpp"
						 "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Initialisers/Zero.cpp")
set(Metrics_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Metrics/AngularVelocitySign.cpp"
                    "${CMAKE_SOURCE_DIR}/src/MotionGeneration/Metrics/Metrics.cpp")
set(ParentSelectors_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/ParentSelectors/MetricProportional.cpp")
set(SurvivorSelectors_SOURCES "${CMAKE_SOURCE_DIR}/src/MotionGeneration/SurvivorSelectors/CullEquals.cpp"
                              "${CMAKE_SOURCE_DIR}/src/MotionGeneration/SurvivorSelectors/CullPartiallyDominated.cpp"
                              "${CMAKE_SOURCE_DIR}/src/MotionGeneration/SurvivorSelectors/ParetoFront.cpp"
                              "${CMAKE_SOURCE_DIR}/src/MotionGeneration/SurvivorSelectors/SurvivorSelectionOverMetric.cpp")
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
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/WaveletSNV.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/HalfSineAsynchronous.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/HalfSineSingle.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/HalfSineSynchronous.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/DeletionLInt.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/DirectionalLInt.cpp"
						"${CMAKE_SOURCE_DIR}/src/MotionGeneration/Variations/SNVLInt.cpp")
set(Wavelet_SOURCES "${CMAKE_SOURCE_DIR}/src/Wavelet/HaarWavelet.cpp")
set(MGEALite_SOURCES "${CMAKE_BINARY_DIR}/mgealite_version.cpp"
					"${CMAKE_SOURCE_DIR}/src/Common.cpp"
					"${CMAKE_SOURCE_DIR}/src/Metric.cpp"
					"${CMAKE_SOURCE_DIR}/src/SimulationData.cpp"
					"${CMAKE_SOURCE_DIR}/src/SimulationLog.cpp"
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
set(GUI_SOURCES "${CMAKE_SOURCE_DIR}/src/GUI/GUIStateDrawer.cpp"
				"${CMAKE_SOURCE_DIR}/src/GUI/Initialisation.cpp"
				"${CMAKE_SOURCE_DIR}/src/GUI/ImGuiExtensions.cpp"
				"${CMAKE_SOURCE_DIR}/src/GUI/ImPlotExtensions.cpp"
				"${CMAKE_SOURCE_DIR}/src/GUI/PlotData.cpp")
set(MGEALite_SOURCES_ALL ${Logging_SOURCES} ${JSON_SOURCES} ${MotionGeneration_SOURCES}
						 ${Initialisers_SOURCES} ${Metrics_SOURCES} ${ParentSelectors_SOURCES}
						 ${SurvivorSelectors_SOURCES} ${Variations_SOURCES} ${Wavelet_SOURCES}
						 ${MGEALite_SOURCES} ${IMGUI_SOURCES} ${GUI_SOURCES})

set(MGEALite_INCLUDES_SYSINT "${spdlog_SOURCE_DIR}/include" "${json_SOURCE_DIR}/include")

set(MGEALite_INCLUDES_PUBLIC "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/include"
							 "${glfw_SOURCE_DIR}/include"
							 "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends"
							 "${implot_SOURCE_DIR}")

set(MGEALite_LIBRARIES DEvA::DEvA DTimer spdlog::spdlog nlohmann_json::nlohmann_json glfw opengl32 gdi32)

add_executable(MGEALite "${CMAKE_SOURCE_DIR}/src/MGEALite.cpp")
target_sources(MGEALite PUBLIC ${MGEALite_SOURCES_ALL})
target_include_directories(MGEALite SYSTEM INTERFACE ${MGEALite_INCLUDES_SYSINT})
target_include_directories(MGEALite PUBLIC ${MGEALite_INCLUDES_PUBLIC})
target_link_libraries(MGEALite ${MGEALite_LIBRARIES})

if(WIN32)
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
   #set_target_properties(MGEALite PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   #set_target_properties(MGEALite PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
   set_target_properties(MGEALite PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:CONSOLE")
endif(WIN32)

file(GLOB Data_Directory "${CMAKE_SOURCE_DIR}/data/*")
file(COPY ${Data_Directory} DESTINATION ${CMAKE_BINARY_DIR}/data)

add_custom_command(TARGET MGEALite POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/EASetup.json" ${CMAKE_BINARY_DIR}
    COMMENT "Copying EASetup.json to ${CMAKE_BINARY_DIR}"
)

add_subdirectory("tests")
