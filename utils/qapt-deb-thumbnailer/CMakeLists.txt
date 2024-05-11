set(debthumbnailer_SRCS DebThumbnailer.cpp)

add_library(debthumbnailer MODULE ${debthumbnailer_SRCS})

target_link_libraries(debthumbnailer
    KF6::KIOWidgets
    QApt::Main)

message(WARNING "make sure thumbnailer actually works")

install(TARGETS debthumbnailer DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/thumbcreator)

#install(TARGETS debthumbnailer DESTINATION ${KDE_INSTALL_PLUGINDIR})
#install(FILES debthumbnailer.desktop DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/thumbcreator)
