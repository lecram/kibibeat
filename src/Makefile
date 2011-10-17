-include env

ifeq ($(CC), cc)
CFLAGS = -Wall -Werror -O2 -I include
CLINK = -lm
else ifeq ($(CC), gcc)
CFLAGS = -Wall -Werror -O2 -I include
CLINK = -lm
else ifeq ($(CC), tcc)
CFLAGS = -Wall -Werror -Wunsupported -Iinclude
endif

I = include/

objects = ker.o kob0.o kob1.o kps.o kop.o krt.o kib.o

dist : $(objects)
	$(CC) -o kib$(BINEXT) $(objects) $(CLINK)

ker.o : ker.c $(I)ker.h
	$(CC) -c $(CFLAGS) ker.c

kob0.o : kob0.c $(I)kob0.h
	$(CC) -c $(CFLAGS) kob0.c

kob1.o : kob1.c $(I)kob0.h $(I)kob1.h
	$(CC) -c $(CFLAGS) kob1.c

kps.o : kps.c $(I)kob0.h $(I)kps.h
	$(CC) -c $(CFLAGS) kps.c

kop.o : kop.c $(I)ker.h $(I)kob0.h $(I)kob1.h $(I)krt.h $(I)kop.h
	$(CC) -c $(CFLAGS) kop.c

krt.o : krt.c $(I)ker.h $(I)kob0.h $(I)kob1.h $(I)kps.h $(I)krt.h $(I)kop.h
	$(CC) -c $(CFLAGS) krt.c

kib.o : kib.c $(I)ker.h $(I)kob0.h $(I)kob1.h $(I)krt.h
	$(CC) -c $(CFLAGS) kib.c

.PHONY : clean
clean :
	$(RM) $(objects)
