file(GLOB GLEE_LIB_SRC_FILES glee_core/*.cpp)
add_library(glee_core SHARED ${GLEE_LIB_SRC_FILES})
target_link_libraries(glee_core ${Boost_LIBRARIES} ${SQLITE_LIBRARIES} -lvlc)

file(GLOB CMD_GLEE_LIB_SRC_FILES glee_cmd/*.cpp)
add_library(glee_cmd SHARED ${CMD_GLEE_LIB_SRC_FILES})
target_link_libraries(glee_cmd glee_core)

file(GLOB EXE_SRC_FILES exe/*.cpp)
foreach(EXE_SRC_FILE ${EXE_SRC_FILES})
        get_filename_component(EXE_DEST_FILE ${EXE_SRC_FILE} NAME_WE)
        add_executable(${EXE_DEST_FILE} ${EXE_SRC_FILE})
        target_link_libraries(${EXE_DEST_FILE} glee_cmd)
        install(TARGETS ${EXE_DEST_FILE} DESTINATION /usr/bin)
endforeach()

install(TARGETS glee_core DESTINATION /usr/lib)
install(TARGETS glee_cmd DESTINATION /usr/lib)
