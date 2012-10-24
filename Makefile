#############################################################################
# Makefile for building: libPoKeys.a
# Generated by qmake (2.01a) (Qt 4.8.1) on: Wed Oct 24 13:36:08 2012
# Project:  PoKeysLib.pro
# Template: lib
# Command: /usr/bin/qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile PoKeysLib.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_WEBKIT
CFLAGS        = -pipe -g -Wall -W -fPIC -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -g -Wall -W -fPIC -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4 -I/usr/include/libusb-1.0 -I.
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = PoKeysLibCore.c \
		PoKeysLibEncoders.c \
		PoKeysLibPulseEngine.c \
		PoKeysLibMatrixLED.c \
		PoKeysLibMatrixKB.c \
		PoKeysLibLCD.c \
		PoKeysLibIO.c \
		PoKeysLibDeviceData.c \
		PoKeysLibCoreSockets.c \
		hid-libusb.c 
OBJECTS       = PoKeysLibCore.o \
		PoKeysLibEncoders.o \
		PoKeysLibPulseEngine.o \
		PoKeysLibMatrixLED.o \
		PoKeysLibMatrixKB.o \
		PoKeysLibLCD.o \
		PoKeysLibIO.o \
		PoKeysLibDeviceData.o \
		PoKeysLibCoreSockets.o \
		hid-libusb.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/staticlib.prf \
		/usr/share/qt4/mkspecs/features/static.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		PoKeysLib.pro
QMAKE_TARGET  = PoKeys
DESTDIR       = 
TARGET        = libPoKeys.a

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET) 

staticlib: $(TARGET)

$(TARGET):  $(OBJECTS) $(OBJCOMP) 
	-$(DEL_FILE) $(TARGET)
	$(AR) $(TARGET) $(OBJECTS)


Makefile: PoKeysLib.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/staticlib.prf \
		/usr/share/qt4/mkspecs/features/static.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile PoKeysLib.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/staticlib.prf:
/usr/share/qt4/mkspecs/features/static.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile PoKeysLib.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/PoKeys1.0.0 || $(MKDIR) .tmp/PoKeys1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/PoKeys1.0.0/ && $(COPY_FILE) --parents PoKeysLibCoreSockets.h PoKeysLibCore.h hidapi.h PoKeysLib.h /usr/include/libusb-1.0/libusb.h .tmp/PoKeys1.0.0/ && $(COPY_FILE) --parents PoKeysLibCore.c PoKeysLibEncoders.c PoKeysLibPulseEngine.c PoKeysLibMatrixLED.c PoKeysLibMatrixKB.c PoKeysLibLCD.c PoKeysLibIO.c PoKeysLibDeviceData.c PoKeysLibCoreSockets.c hid-libusb.c .tmp/PoKeys1.0.0/ && (cd `dirname .tmp/PoKeys1.0.0` && $(TAR) PoKeys1.0.0.tar PoKeys1.0.0 && $(COMPRESS) PoKeys1.0.0.tar) && $(MOVE) `dirname .tmp/PoKeys1.0.0`/PoKeys1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/PoKeys1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all:
compiler_moc_header_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

PoKeysLibCore.o: PoKeysLibCore.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h \
		PoKeysLibCoreSockets.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibCore.o PoKeysLibCore.c

PoKeysLibEncoders.o: PoKeysLibEncoders.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibEncoders.o PoKeysLibEncoders.c

PoKeysLibPulseEngine.o: PoKeysLibPulseEngine.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibPulseEngine.o PoKeysLibPulseEngine.c

PoKeysLibMatrixLED.o: PoKeysLibMatrixLED.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibMatrixLED.o PoKeysLibMatrixLED.c

PoKeysLibMatrixKB.o: PoKeysLibMatrixKB.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibMatrixKB.o PoKeysLibMatrixKB.c

PoKeysLibLCD.o: PoKeysLibLCD.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibLCD.o PoKeysLibLCD.c

PoKeysLibIO.o: PoKeysLibIO.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibIO.o PoKeysLibIO.c

PoKeysLibDeviceData.o: PoKeysLibDeviceData.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibDeviceData.o PoKeysLibDeviceData.c

PoKeysLibCoreSockets.o: PoKeysLibCoreSockets.c PoKeysLib.h \
		PoKeysLibCore.h \
		hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o PoKeysLibCoreSockets.o PoKeysLibCoreSockets.c

hid-libusb.o: hid-libusb.c hidapi.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o hid-libusb.o hid-libusb.c

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

