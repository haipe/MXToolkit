QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../src/attribute.c \
    ../src/char_ref.c \
    ../src/error.c \
    ../src/parser.c \
    ../src/string_buffer.c \
    ../src/string_piece.c \
    ../src/tag.c \
    ../src/tokenizer.c \
    ../src/utf8.c \
    ../src/util.c \
    ../src/vector.c

HEADERS += \
    ../src/attribute.h \
    ../src/char_ref.h \
    ../src/error.h \
    ../src/gumbo.h \
    ../src/insertion_mode.h \
    ../src/parser.h \
    ../src/string_buffer.h \
    ../src/string_piece.h \
    ../src/strings_util.h \
    ../src/tag_enum.h \
    ../src/tag_gperf.h \
    ../src/tag_sizes.h \
    ../src/tag_strings.h \
    ../src/token_type.h \
    ../src/tokenizer.h \
    ../src/tokenizer_states.h \
    ../src/utf8.h \
    ../src/util.h \
    ../src/vector.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
