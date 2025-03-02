include(ECMAddTests)

find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)

ecm_add_test(dependencyinfotest.cpp
    LINK_LIBRARIES
        Qt6::Test
        QApt::Main)

ecm_add_test(sourceslisttest.cpp
    LINK_LIBRARIES
        Qt6::Test
        QApt::Main)

ecm_add_test(transactionerrorhandlingtest.cpp
    LINK_LIBRARIES
        Qt6::Test
        QApt::Main)


target_compile_definitions(dependencyinfotest PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
target_compile_definitions(sourceslisttest PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
target_compile_definitions(transactionerrorhandlingtest PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
