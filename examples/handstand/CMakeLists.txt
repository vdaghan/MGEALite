function(prepareExample TargetName FileName)
    add_executable(${TargetName} "${CMAKE_CURRENT_SOURCE_DIR}/${FileName}.cpp")

    target_sources(${TargetName}  PUBLIC ${MGEALite_SOURCES_ALL})
    target_include_directories(${TargetName}  SYSTEM INTERFACE ${MGEALite_INCLUDES_SYSINT})
    target_include_directories(${TargetName}  PUBLIC ${MGEALite_INCLUDES_PUBLIC})
    target_link_libraries(${TargetName}  ${MGEALite_LIBRARIES})

    add_custom_command(TARGET ${TargetName}  PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FileName}.json" ${CMAKE_BINARY_DIR}
        COMMENT "Copying ${FileName}.json to ${CMAKE_BINARY_DIR}"
    )
endfunction()

prepareExample(Handstand_00_standstill "00_standstill")
prepareExample(Handstand_01_standstill "01_standstill")
prepareExample(Handstand_02_standstill "02_standstill")
prepareExample(Handstand_03_standstill "03_standstill")



