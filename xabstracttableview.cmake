include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED DIALOGRESIZE_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogresize.cmake)
    set(XABSTRACTTABLEVIEW_SOURCES ${XABSTRACTTABLEVIEW_SOURCES} ${DIALOGRESIZE_SOURCES})
endif()
if (NOT DEFINED DIALOGREMOVE_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogremove.cmake)
    set(XABSTRACTTABLEVIEW_SOURCES ${XABSTRACTTABLEVIEW_SOURCES} ${DIALOGREMOVE_SOURCES})
endif()
if (NOT DEFINED DIALOGTEXTINFO_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogtextinfo.cmake)
    set(XABSTRACTTABLEVIEW_SOURCES ${XABSTRACTTABLEVIEW_SOURCES} ${DIALOGTEXTINFO_SOURCES})
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../XShortcuts/xshortcuts.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../XInfoDB/xinfodb.cmake)

set(XABSTRACTTABLEVIEW_SOURCES
    ${XABSTRACTTABLEVIEW_SOURCES}
    ${DIALOGRESIZE_SOURCES}
    ${DIALOGREMOVE_SOURCES}
    ${DIALOGTEXTINFO_SOURCES}
    ${XSHORTCUTS_SOURCES}
    ${XINFODB_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xabstracttableview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xabstracttableview.h
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableview.h
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableeditview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdevicetableeditview.h
)
