set(qaptbatch_SRCS
    main.cpp
    qaptbatch.cpp
    detailswidget.cpp
)

add_executable(qapt-batch ${qaptbatch_SRCS})

target_link_libraries(qapt-batch
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOCore
    KF6::WidgetsAddons
    KF6::WindowSystem
    QApt::Main)

install(TARGETS qapt-batch ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
