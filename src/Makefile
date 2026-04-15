WXDIR = /home/Administrator/wxWidgets-3.2.10
WXINCLUDE = $(WXDIR)/include
WXLIB = $(WXDIR)/build-release/lib
EXTDIR = /home/Administrator/extlib

SRCS = App.cpp BaseFrame.cpp FrameImpl.cpp MiscWx.cpp tstring.cpp RegistryHive.cpp\
       Widgets/PrevNextPanel.cpp Widgets/ProgressPanel.cpp Widgets/SimpleButton.cpp\
       Worker/Events.cpp Worker/MakePEImage.cpp Worker/ISO.cpp Worker/WIM.cpp Worker/MakeISO.cpp\
       Worker/CheckISOImage.cpp
RESOURCES = file.rc icon.rc version.rc

OBJS = $(SRCS:.cpp=.o)
RESOURCE_OBJS = $(RESOURCES:.rc=.o)

CFLAGS = -I./ -I$(EXTDIR)/include -I$(WXINCLUDE) \
                   -I$(WXLIB)/wx/include/msw-unicode-static-3.2 \
                    -std=c++11 -Wno-deprecated\
                   -D_FILE_OFFSET_BITS=64  -D__WXMSW__  -D_UNICODE -DUNICODE -D__MINGW_USE_VC2005_COMPAT -D_WINDOWS\
                   -DNDEBUG
#                   -DDEBUG -g -O0

#LFLAGS = -L$(WXLIB) -L$(EXTDIR)/lib -mwindows -mthreads -static -static-libgcc
LFLAGS = -L$(WXLIB) -L$(EXTDIR)/lib -mwindows -mthreads -static -static-libgcc -s\
 -Wl,--major-os-version=6 -Wl,--minor-os-version=1 -Wl,--major-subsystem-version=6 -Wl,--minor-subsystem-version=1
LIBS =          -lwx_mswu_core-3.2 \
                -lwx_baseu-3.2 \
                -lwxjpeg-3.2 -lwxregexu-3.2 -lwxscintilla-3.2 -lpng -ltiff -lrpcrt4 \
                -loleaut32 -lole32 -luuid -luxtheme -lwinspool -lshell32 -lshlwapi -lcomctl32 \
                -lcomdlg32 -ladvapi32 -lversion -lws2_32 -lgdi32 -loleacc -lwinhttp -lkernel32 -luser32\
                -lwim -lckfilesystem -lckcore -lz -lzstd -lntdll\
                -ludf -liso9660 -lcdio -liconv -lwinmm

CC = g++
TARGET = PEMaker.exe

$(TARGET): $(OBJS) $(RESOURCE_OBJS)
	$(CC) $(OBJS) $(RESOURCE_OBJS) $(LFLAGS) $(LIBS) -o $(TARGET)

$(OBJS): %.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(RESOURCE_OBJS): %.o:%.rc
	windres.exe -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) $(RESOURCE_OBJS)
