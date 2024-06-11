include(ECMConfiguredInstall)

ecm_install_configured_files(INPUT org.debian.AptXapianIndex.service DESTINATION ${KDE_INSTALL_DBUSSYSTEMSERVICEDIR} @ONLY)
install(FILES org.debian.AptXapianIndex.conf DESTINATION ${KDE_INSTALL_DBUSDIR}/system.d)
install(FILES org.debian.aptxapianindex.policy DESTINATION ${POLKITQT-1_POLICY_FILES_INSTALL_DIR})
install(PROGRAMS update-apt-xapian-index-dbus DESTINATION ${KDE_INSTALL_DATADIR}/apt-xapian-index)