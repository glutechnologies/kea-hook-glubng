KEA_MSG_COMPILER ?= kea-msg-compiler
KEA_INCLUDE ?= /usr/local/include/kea
KEA_LIB ?= /usr/local/lib

OBJECTS = src/messages.o src/logger.o src/load.o src/callouts.o src/version.o src/socket.o
CXXFLAGS = -I $(KEA_INCLUDE) -fPIC -Wno-deprecated -std=c++11
LDFLAGS = -L $(KEA_LIB) -shared -lkea-dhcpsrv -lkea-dhcp++ -lkea-hooks -lkea-log -lkea-util -lkea-exceptions

kea-hook-glubng.so: $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# Compile logging messages, generates messages.h and messages.cc.
# Only needs to be done when changing messages.mes.
# You need to build kea with `./configure --enable-generate-messages` to obtain `kea-msg-compiler`
build-messages: src/messages.mes
	$(KEA_MSG_COMPILER) -d src/ $<

clean:
	rm -f src/*.o
	rm -f kea-hook-glubng.so