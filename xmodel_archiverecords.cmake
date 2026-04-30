include(${CMAKE_CURRENT_LIST_DIR}/xmodel.cmake)

include_directories(${CMAKE_CURRENT_LIST_DIR})

set(XMODEL_ARCHIVERECORDS_SOURCES
    ${XMODEL_ARCHIVERECORDS_SOURCES}
    ${XMODEL_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_archiverecords.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_archiverecords.h
)
