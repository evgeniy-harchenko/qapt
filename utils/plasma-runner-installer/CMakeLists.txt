# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"plasma-runner-installer\")

add_library(krunner_installer MODULE installerrunner.cpp)

target_link_libraries(krunner_installer
    Qt::Gui
    KF6::I18n
    KF6::Service
    KF6::Runner)

install(TARGETS krunner_installer DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/krunner)

#install(TARGETS krunner_installer DESTINATION ${KDE_INSTALL_PLUGINDIR} )
#install(FILES plasma-runner-installer.desktop DESTINATION ${KDE_INSTALL_DATADIR}/kio/servicemenus)
