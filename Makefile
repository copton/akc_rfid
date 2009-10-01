TARGETS=$(patsubst %.c, %.o, $(wildcard *.c))

all: akc_rfid

-include depend.mk

INCLUDE_FLAGS=-I ../3rd/mxml/install/include/

%.o: %.c
	gcc  -Wall -g -c $< $(INCLUDE_FLAGS)

akc_rfid: $(TARGETS)
	gcc -g -o $@ $^ -lmxml -L ../3rd/mxml/install/lib -lmysqlclient

clean:
	rm -f *.o akc_rfid
	rm -f depend.mk

depend.mk:
	for i in $(wildcard *.c); do gcc -M $(INCLUDE_FLAGS) $$i; done > $@
