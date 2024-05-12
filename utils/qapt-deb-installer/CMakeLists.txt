add_definitions(-DTRANSLATION_DOMAIN=\"qapt-deb-installer\")

set(qapt-deb-installer_SRCS
    main.cpp
    DebCommitWidget.cpp
    DebInstaller.cpp
    DebViewer.cpp
    ChangesDialog.cpp)

add_executable(qapt-deb-installer ${qapt-deb-installer_SRCS})

target_link_libraries(qapt-deb-installer
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOCore
    KF6::TextWidgets
    KF6::WidgetsAddons
    #DebconfKDE::Main
    QApt::Main)

install(TARGETS qapt-deb-installer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS qapt-deb-installer.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
