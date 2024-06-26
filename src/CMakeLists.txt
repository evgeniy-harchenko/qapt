set(qapt_SRCS
    backend.cpp
    cache.cpp
    package.cpp
    config.cpp
    history.cpp
    debfile.cpp
    dependencyinfo.cpp
    changelog.cpp
    transaction.cpp
    downloadprogress.cpp
    markingerrorinfo.cpp
    sourceentry.cpp
    sourceslist.cpp)

add_subdirectory(worker)

set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/worker/${QAPT_WORKER_RDN_VERSIONED}.transaction.xml
    APPEND PROPERTY
        INCLUDE downloadprogress.h
)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/worker/${QAPT_WORKER_RDN_VERSIONED}.transaction.xml
    APPEND PROPERTY
        NO_NAMESPACE 1
)

message(WARNING "terrible use of transaction xml file")
qt6_add_dbus_interface(qapt_SRCS
                       ${CMAKE_CURRENT_BINARY_DIR}/worker/${QAPT_WORKER_RDN_VERSIONED}.transaction.xml
                       transactioninterface_p)

set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/worker/${QAPT_WORKER_RDN_VERSIONED}.xml
    APPEND PROPERTY
        NO_NAMESPACE 1
)

message(WARNING "terrible use of  worker xml file")
qt6_add_dbus_interface(qapt_SRCS
                       ${CMAKE_CURRENT_BINARY_DIR}/worker/${QAPT_WORKER_RDN_VERSIONED}.xml
                       workerinterface_p)

configure_file(dbusinterfaces_p.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/dbusinterfaces_p.h)

ecm_create_qm_loader(qapt_SRCS libqapt)

add_library(QApt SHARED ${qapt_SRCS})
add_library(QApt::Main ALIAS QApt)

# Set header paths for target so the utils can find them.
target_include_directories(QApt PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..;${CMAKE_CURRENT_BINARY_DIR}/..>")
target_include_directories(QApt INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/QApt>" )

set_target_properties(QApt
    PROPERTIES
    VERSION ${QAPT_VERSION}
    SOVERSION ${QAPT_SOVERSION}
    EXPORT_NAME Main)

target_link_libraries(QApt
    PUBLIC
        Qt6::Widgets
        ${APTPKG_LIBRARIES}
    PRIVATE
        Qt6::DBus
        ${XAPIAN_LIBRARIES})

install(TARGETS QApt EXPORT QAptTargets LIBRARY DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_headers(QAPT_CAMEL_CASE_HEADERS
    HEADER_NAMES
        Backend
        Changelog
        Config
        DebFile
        DependencyInfo
        DownloadProgress
        Globals
        History
        MarkingErrorInfo
        Package
        SourceEntry
        SourcesList
        Transaction

  REQUIRED_HEADERS QAPT_HEADERS
  PREFIX QApt)

install(FILES ${QAPT_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR}/qapt COMPONENT Devel)
install(FILES ${QAPT_CAMEL_CASE_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR}/QApt COMPONENT Devel)
