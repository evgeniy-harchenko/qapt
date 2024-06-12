# QApt Qt6

## Build instructions

### Build dependencies:
```bash
cmake build-essential extra-cmake-modules qt6-base-dev libqt6core5compat6-dev libxapian-dev libapt-pkg-dev libpolkit-qt6-1-dev libkf6i18n-dev gettext
```
### Additional build dependencies for qapt-utils and qapt-gst-helper:
```bash
libkf6auth-dev libkf6coreaddons-dev libkf6kio-dev libkf6runner-dev libkf6textwidgets-dev libkf6widgetsaddons-dev libkf6windowsystem-dev libkf6iconthemes-dev libkf6service-dev libgstreamer-plugins-bad1.0-dev
```
### Runtime dependencies:
```bash
apt-xapian-index
```
### Install
```bash
cd /where/your/downloaded/qapt/source
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install
```
