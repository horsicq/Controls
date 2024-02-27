include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogtextinfo.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogresize.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogremove.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../XShortcuts/xshortcuts.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../XInfoDB/xinfodb.cmake)

set(XABSTRACTTABLEVIEW_SOURCES
    ${DIALOGRESIZE_SOURCES}
    ${DIALOGREMOVE_SOURCES}
    ${XSHORTCUTS_SOURCES}
    ${DIALOGTEXTINFO_SOURCES}
    ${XINFODB_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xabstracttableview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xabstracttableview.h
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableview.h
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableeditview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableeditview.h
)
