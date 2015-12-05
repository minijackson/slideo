QT  += core gui widgets multimedia multimediawidgets

CONFIG += c++14 link_pkgconfig
PKGCONFIG += yaml-cpp

TARGET = slideo
TEMPLATE = app

SOURCES += mainwindow.cpp videoplayermanager.cpp projectmanager.cpp timeselectdialog.cpp doubleclickablelabel.cpp history.cpp main.cpp
HEADERS += mainwindow.hpp videoplayermanager.hpp projectmanager.hpp timeselectdialog.hpp doubleclickablelabel.hpp history.hpp
