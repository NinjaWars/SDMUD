default: build sd
clean:
	 rm -f $(O_FILES)
all:	 clean build sd

CC      = gcc
PROF    = -ggdb
C_FLAGS = -Wall -O $(PROF) $(NOCRYPT)
L_FLAGS = $(PROF)
LIBS = -lz -lpthread

C_FILES := $(wildcard *.c)
O_FILES := $(patsubst %.c, %.o, $(C_FILES))

#	$(CC) $(L_FLAGS) -o ../data/sdmud $(O_FILES) $(LIBS)


build:
	@if [ -f .build ]; then \
	   expr `cat .build` + 1 > .build; \
	else \
	   echo '1' > .build; \
	fi
	@if [ -f .version ]; then \
	    expr `cat .version` + 1 > .version; \
	else \
	    echo '10001' > .version; \
	fi
	@if [ `cat .build` -ge 51 ]; then \
	    echo '1' > .build; \
	fi
	@echo '#ifndef BUILD_H' > build.h
	@echo '#define BUILD_H' >> build.h
	@echo "#define LINKCOUNT   `cat .build`" >> build.h
	@echo '#endif' >> build.h
	@echo '#ifndef VERSION_H' > version.h
	@echo '#define VERSION_H' >> version.h
	@echo "#define VERSION   `cat .version`" >> version.h
	@echo '#endif' >> version.h
	@echo "Build number `cat .build`, version `cat .version` set."

sd: $(O_FILES)
	rm -f ../data/sdmud
	$(CC) $(L_FLAGS) -o ../data/sdmud $(O_FILES) $(LIBS)

.c.o: sd.h
	$(CC) -c $(C_FLAGS) $<
