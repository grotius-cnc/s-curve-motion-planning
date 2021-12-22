QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    libocct/draw_primitives.cpp \
    libocct/opencascade.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    libocct/draw_primitives.h \
    libocct/opencascade.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH +=  /usr/include/ \
                /usr/local/include/opencascade/ \
                /usr/include/eigen3/ \
                /usr/local/lib/ \

LIBS += -L/usr/local/lib/ \

# project
INCLUDEPATH+=   libspline/ \
                libocct/ \

#Opencascade
LIBS+= -L/usr/local/lib/ \

INCLUDEPATH +=  /usr/local/include/kdl/ \
                /usr/local/include/opencascade/ \
                /usr/include/eigen3/ \
                /usr/include/ \
                /usr/local/lib/ \

LIBS += -lorocos-kdl

# Opencascade
LIBS += -lTKPrim
LIBS += -lTKernel -lTKMath -lTKTopAlgo -lTKService
LIBS += -lTKG2d -lTKG3d -lTKV3d -lTKOpenGl
LIBS += -lTKBRep -lTKXSBase -lTKGeomBase
LIBS += -lTKMeshVS -lTKXSDRAW
LIBS += -lTKLCAF -lTKXCAF -lTKCAF
LIBS += -lTKCDF -lTKBin -lTKBinL -lTKBinXCAF -lTKXml -lTKXmlL -lTKXmlXCAF
# -- IGES support
LIBS += -lTKIGES
# -- STEP support
LIBS += -lTKSTEP -lTKXDESTEP -lTKXDEIGES
# -- STL support
LIBS += -lTKSTL
# -- OBJ/glTF support

LIBS += -lTKRWMesh

#src/base/io_occ_base_mesh.cpp \
#src/base/io_occ_gltf.cpp \
#src/base/io_occ_obj.cpp

# -- VRML support
LIBS += -lTKVRML

# this copies the configuration files etc to the build direcory. So user has only to edit the source directory.
copydata.commands = $(COPY_DIR) $$PWD/* $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    libicons.qrc
