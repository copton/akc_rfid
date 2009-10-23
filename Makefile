TARGETS=$(patsubst %.c, %.o, $(wildcard *.c))

all: akc_rfid

-include depend.mk
include config.mk


%.o: %.c
	gcc  -Wall -g -c $< $(INCLUDE_FLAGS)

akc_rfid: $(TARGETS)
	gcc -g -o $@ $^ $(LIB_FLAGS) -lmxml -lmysqlclient

clean:
	rm -f *.o akc_rfid
	rm -f depend.mk

depend.mk:
	for i in $(wildcard *.c); do gcc -M $(INCLUDE_FLAGS) $$i; done > $@
