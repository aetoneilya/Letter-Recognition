QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lib/matrixplus/s21_matrix_oop.cc \
    main.cc \
    model/model.cc \
    model/neural_network/graph_network/graph_network.cc \
    model/neural_network/graph_network/layer.cc \
    model/neural_network/graph_network/neuron.cc \
    model/neural_network/io/weight_reader.cc \
    model/neural_network/io/weight_writer.cc \
    model/neural_network/matrix_network/matrix_network.cc \
    model/neural_network/neural_network.cc \
    model/neural_network/utility.cc \
    model/reader/csv_reader.cc \
    view/main_window.cc \
    view/scribblearea/scribblearea.cc \
    view/qcustomplot/qcustomplot.cc

HEADERS += \
    controller/controller.h \
    lib/matrixplus/s21_matrix_oop.h \
    model/configuration.h \
    model/image.h \
    model/model.h \
    model/neural_network/graph_network/graph_network.h \
    model/neural_network/graph_network/layer.h \
    model/neural_network/graph_network/neuron.h \
    model/neural_network/io/weight_reader.h \
    model/neural_network/io/weight_writer.h \
    model/neural_network/matrix_network/matrix_network.h \
    model/neural_network/network_interface.h \
    model/neural_network/neural_network.h \
    model/neural_network/utility.h \
    model/reader/base_file_reader.h \
    model/reader/csv_reader.h \
    view/main_window.h \
    view/scribblearea/scribblearea.h \
    view/qcustomplot/qcustomplot.h    

FORMS += \
    view/resources/main_window.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
