# TODO guard
include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogtextinfo.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../XShortcuts/xshortcuts.cmake)

set(XABSTRACTTABLEVIEW_SOURCES
    ${XSHORTCUTS_SOURCES}
    ${DIALOGTEXTINFO_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xabstracttableview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableview.cpp
)
