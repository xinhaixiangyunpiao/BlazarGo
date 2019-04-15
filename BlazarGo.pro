#-------------------------------------------------
#
# Project created by QtCreator 2019-04-11T21:55:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlazarGo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    board/board_difference.cc \
    board/board_output.cc \
    board/force.cc \
    board/position.cc \
    player/node_record.cc \
    util/rand.cc \
    util/SGFParser.cpp \
    util/vector_util.cc

HEADERS += \
    mainwindow.h \
    def.h \
    search.h \
    board/board.h \
    board/board_difference.h \
    board/board_output.h \
    board/force.h \
    board/full_board.h \
    board/full_board_hasher.h \
    board/pos_cal.h \
    board/position.h \
    board/zob_hasher.h \
    game/fresh_game.h \
    game/game.h \
    game/game_info.h \
    game/monte_carlo_game.h \
    game/sgf_game.h \
    player/input_player.h \
    player/node_record.h \
    player/passable_player.h \
    player/player.h \
    player/random_player.h \
    player/sgf_player.h \
    player/transposition_table.h \
    player/uct_player.h \
    piece_structure/chain_set.h \
    piece_structure/eye_set.h \
    util/bitset_util.h \
    util/cxxopts.hpp \
    util/rand.h \
    util/SGFParser.h \
    util/vector_util.h \
    convey.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
