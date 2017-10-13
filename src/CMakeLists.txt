file(GLOB GLEE_LIB_SRC_FILES xglee/*.cpp)
add_library(xglee SHARED ${GLEE_LIB_SRC_FILES})
target_link_libraries(xglee ${Boost_LIBRARIES} ${SQLITE_LIBRARIES})

file(GLOB GUI_GLEE_LIB_SRC_FILES gui_glee/*.cpp)
add_library(guiglee SHARED ${GUI_GLEE_LIB_SRC_FILES})
target_link_libraries(guiglee xglee ${GTK3_LIBRARIES})

file(GLOB EXE_SRC_FILES exe/*.cpp)
foreach(EXE_SRC_FILE ${EXE_SRC_FILES})
        get_filename_component(EXE_DEST_FILE ${EXE_SRC_FILE} NAME_WE)
        add_executable(${EXE_DEST_FILE} ${EXE_SRC_FILE})
        target_link_libraries(${EXE_DEST_FILE} guiglee)
endforeach()
