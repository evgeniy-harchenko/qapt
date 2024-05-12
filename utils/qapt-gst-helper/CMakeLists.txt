add_definitions(-DTRANSLATION_DOMAIN=\"qapt-gst-helper\")

set(qapt-gst-helper_SRCS
    main.cpp
    GstMatcher.cpp
    PluginFinder.cpp
    PluginHelper.cpp
    PluginInfo.cpp)

include_directories(${GSTREAMER_INCLUDE_DIR}
    ${GLIB2_INCLUDE_DIR})

add_executable(qapt-gst-helper ${qapt-gst-helper_SRCS})

target_link_libraries(qapt-gst-helper
    ${GSTREAMER_LIBRARIES}
    ${GLIB2_LIBRARIES}
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOCore
    KF6::WidgetsAddons
    KF6::WindowSystem
    QApt::Main)

install(TARGETS qapt-gst-helper DESTINATION ${LIBEXEC_INSTALL_DIR})
