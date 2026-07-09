set(PUBLIC_HEADERS
    ${CMAKE_SOURCE_DIR}/include/LingmoIcon/IconTheme.h
    ${CMAKE_SOURCE_DIR}/include/LingmoIcon/IconLoader.h
)
set(PRIVATE_HEADERS
    LingmoIcon/private/IconTheme_p.h
)
set(SOURCES
    LingmoIcon/IconTheme.cpp
    LingmoIcon/IconLoader.cpp
)
set(CMAKE_AUTOMOC ON)
add_library(LingmoIcon SHARED ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
add_library(Lingmo::Icon ALIAS LingmoIcon)
target_include_directories(LingmoIcon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(LingmoIcon PUBLIC Qt6::Core Qt6::Gui Qt6::Svg)
target_compile_definitions(LingmoIcon PRIVATE LINGMOICON_LIBRARY)
set_target_properties(LingmoIcon PROPERTIES
    SOVERSION 1 VERSION ${PROJECT_VERSION}
    DEFINE_SYMBOL LINGMOICON_LIBRARY OUTPUT_NAME LingmoIcon
)
configure_file("${CMAKE_SOURCE_DIR}/include/LingmoIcon/LingmoIconExport.h.in"
    "${CMAKE_BINARY_DIR}/include/LingmoIcon/LingmoIconExport.h")
configure_file("${CMAKE_SOURCE_DIR}/include/LingmoIcon/LingmoIconVersion.h.in"
    "${CMAKE_BINARY_DIR}/include/LingmoIcon/LingmoIconVersion.h")
install(TARGETS LingmoIcon EXPORT LingmoIconTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/LingmoIcon/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LingmoIcon
    FILES_MATCHING PATTERN "*.h" PATTERN "*.h.in" EXCLUDE)
install(FILES
    "${CMAKE_BINARY_DIR}/include/LingmoIcon/LingmoIconExport.h"
    "${CMAKE_BINARY_DIR}/include/LingmoIcon/LingmoIconVersion.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LingmoIcon)
install(EXPORT LingmoIconTargets FILE LingmoIconTargets.cmake
    NAMESPACE Lingmo:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoIcon)
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_SOURCE_DIR}/cmake/LingmoIconConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/LingmoIconConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoIcon)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/LingmoIconConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_BINARY_DIR}/LingmoIconConfig.cmake"
    "${CMAKE_BINARY_DIR}/LingmoIconConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoIcon)
