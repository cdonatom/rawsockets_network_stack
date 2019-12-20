 SRC_PATH=src
 OBJ_PATH=obj
 LIB_PATH=lib
 BIN_PATH=bin
INCL_PATH=include
 MAN_PATH=man
 MAN_ES_PATH=$(MAN_PATH)/es

CC=gcc
CFLAGS=-I$(INCL_PATH) -Wall $(RPM_OPT_FLAGS)

_DEPS = rawnet.h timerms.h
RAWNET_DEPS = $(patsubst %,$(INCL_PATH)/%,$(_DEPS))

_OBJ = rawnet.o timerms.o
OBJ = $(patsubst %,$(OBJ_PATH)/%,$(_OBJ))

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(DEPS)
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$(LIB_PATH)/librawnet.a: $(OBJ)
	ar -cq $@ $^

$(LIB_PATH)/librawnet.so.1.0: $(OBJ)
	$(CC) $(CFLAGS) -shared -Wl,-soname,librawnet.so.1 -o $@ $^

$(BIN_PATH)/rawnetcc: $(SRC_PATH)/rawnetcc.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean all install uninstall

all: $(LIB_PATH)/librawnet.so.1.0 $(LIB_PATH)/librawnet.a $(BIN_PATH)/rawnetcc

clean:
	rm -f $(LIB_PATH)/librawnet.so.1.0 \
          $(LIB_PATH)/librawnet.a \
          $(BIN_PATH)/rawnetcc \
          $(OBJ_PATH)/*.o \
          *~ \
          $(SRC_PATH)/*~ \
          $(INCL_PATH)/*~ \
          $(MAN_PATH)/*~ \
          $(MAN_ES_PATH)/*~

PREFIX=/usr
INSTALL_BIN_PATH=$(PREFIX)/bin
INSTALL_LIB_PATH=$(PREFIX)/lib
INSTALL_INCL_PATH=$(PREFIX)/include
INSTALL_MAN3_PATH=$(PREFIX)/share/man/man3
INSTALL_ES_MAN3_PATH=$(PREFIX)/share/man/es/man3

install: all
	install -m 755 -o root -g root -d $(INSTALL_BIN_PATH)/
	install -m 4755 $(BIN_PATH)/rawnetcc $(INSTALL_BIN_PATH)/

	install -m 755 -o root -g root -d $(INSTALL_LIB_PATH)/
	install -m 644  $(LIB_PATH)/librawnet.a      $(INSTALL_LIB_PATH)/
	install -m 644  $(LIB_PATH)/librawnet.so.1.0 $(INSTALL_LIB_PATH)/
	/sbin/ldconfig -n $(INSTALL_LIB_PATH)
	ln -sf librawnet.so.1 $(INSTALL_LIB_PATH)/librawnet.so

	install -m 755 -o root -g root -d $(INSTALL_INCL_PATH)/
	install -m 644  $(INCL_PATH)/rawnet.h  $(INSTALL_INCL_PATH)/
	install -m 644  $(INCL_PATH)/timerms.h $(INSTALL_INCL_PATH)/

	install -m 755 -o root -g root -d $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawnet.3           $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawiface.3         $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawiface_open.3    $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawiface_getname.3 $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawiface_getaddr.3 $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawiface_getmtu.3  $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawiface_close.3   $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawnet_send.3      $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawnet_recv.3      $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawnet_poll.3      $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawnet_strerror.3  $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/timerms.3          $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/timerms_reset.3    $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/timerms_elapsed.3  $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/timerms_left.3     $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/timerms_time.3     $(INSTALL_MAN3_PATH)/
	install -m 644  $(MAN_PATH)/rawnetcc.3         $(INSTALL_MAN3_PATH)/

	install -m 755 -o root -g root -d $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawnet.3           $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawiface.3         $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawiface_open.3    $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawiface_getname.3 $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawiface_getaddr.3 $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawiface_getmtu.3  $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawiface_close.3   $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawnet_send.3      $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawnet_recv.3      $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawnet_poll.3      $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawnet_strerror.3  $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/timerms.3          $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/timerms_reset.3    $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/timerms_elapsed.3  $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/timerms_left.3     $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/timerms_time.3     $(INSTALL_ES_MAN3_PATH)/
	install -m 644  $(MAN_ES_PATH)/rawnetcc.3         $(INSTALL_ES_MAN3_PATH)/

uninstall:
	rm -f $(INSTALL_BIN_PATH)/rawnetcc

	rm -f $(INSTALL_LIB_PATH)/librawnet.a
	rm -f $(INSTALL_LIB_PATH)/librawnet.so.1.0
	rm -f $(INSTALL_LIB_PATH)/librawnet.so.1
	rm -f $(INSTALL_LIB_PATH)/librawnet.so

	rm -f $(INSTALL_INCL_PATH)/rawnet.h
	rm -f $(INSTALL_INCL_PATH)/timerms.h

	rm -f $(INSTALL_MAN3_PATH)/rawnet.3
	rm -f $(INSTALL_MAN3_PATH)/rawiface.3
	rm -f $(INSTALL_MAN3_PATH)/rawiface_open.3
	rm -f $(INSTALL_MAN3_PATH)/rawiface_getname.3
	rm -f $(INSTALL_MAN3_PATH)/rawiface_getaddr.3
	rm -f $(INSTALL_MAN3_PATH)/rawiface_getmtu.3
	rm -f $(INSTALL_MAN3_PATH)/rawiface_close.3
	rm -f $(INSTALL_MAN3_PATH)/rawnet_send.3
	rm -f $(INSTALL_MAN3_PATH)/rawnet_recv.3
	rm -f $(INSTALL_MAN3_PATH)/rawnet_poll.3
	rm -f $(INSTALL_MAN3_PATH)/rawnet_strerror.3
	rm -f $(INSTALL_MAN3_PATH)/timerms.3
	rm -f $(INSTALL_MAN3_PATH)/timerms_reset.3
	rm -f $(INSTALL_MAN3_PATH)/timerms_elapsed.3
	rm -f $(INSTALL_MAN3_PATH)/timerms_left.3
	rm -f $(INSTALL_MAN3_PATH)/timerms_time.3
	rm -f $(INSTALL_MAN3_PATH)/rawnetcc.3

	rm -f $(INSTALL_ES_MAN3_PATH)/rawnet.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawiface.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawiface_open.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawiface_getname.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawiface_getaddr.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawiface_getmtu.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawiface_close.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawnet_send.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawnet_recv.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawnet_poll.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawnet_strerror.3
	rm -f $(INSTALL_ES_MAN3_PATH)/timerms.3
	rm -f $(INSTALL_ES_MAN3_PATH)/timerms_reset.3
	rm -f $(INSTALL_ES_MAN3_PATH)/timerms_elapsed.3
	rm -f $(INSTALL_ES_MAN3_PATH)/timerms_left.3
	rm -f $(INSTALL_ES_MAN3_PATH)/timerms_time.3
	rm -f $(INSTALL_ES_MAN3_PATH)/rawnetcc.3
