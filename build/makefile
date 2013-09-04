#-- globale defs
TARGET = echo_srv.exe
BOOSTLIB = E:\boost_1_53_0\stage\lib
BOOSTINCLUDE = E:\boost_1_53_0
#BOOST_THREAD_USE_LIB=1
#	  -DBOOST_THREAD_PLATFORM_WIN32 

#-- compil pout win32 avec mingw
#CC = i686-w64-mingw32-g++
CC = mingw32-g++
#RM = rm
RM = del

CFLAGS = -I ..\include \
	 -I $(BOOSTINCLUDE) \
	 -DBOOST_THREAD_USE_LIB

LDFLAGS = -L$(BOOSTLIB) \
	-lboost_thread_win32 \
    -lboost_system \
          -lboost_filesystem \
          -lws2_32 \
          -lwsock32 \
          -lpsapi \
	-lpowrprof \
	  -static 


SRCDIR = ..\src
SRC = main.cpp

OBJ = $(SRC:%.cpp=%.o)

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $< -o $@ $(LDFLAGS)

%.o : $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	$(RM) -f \
		*.o \
		$(TARGET)
