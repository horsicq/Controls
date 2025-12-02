include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XHEXVIEW_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XHexView/xhexview.cmake)
    set(XBINARYVIEW_SOURCES ${XBINARYVIEW_SOURCES} ${XHEXVIEW_SOURCES})
endif()

set(XBINARYVIEW_SOURCES
    ${XBINARYVIEW_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xbinaryview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xbinaryview.h
)
