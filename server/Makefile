CC = gcc
CFLAGS =

SRCDIR := src
BUILDDIR := build
LIBDIR := lib
LDFLAGS := -lpq -lrt

LIBS = -I${LIBDIR}
SRCFILES := $(wildcard ${SRCDIR}/*.c)
OBJFILES := $(patsubst ${SRCDIR}/%.c, ${BUILDDIR}/%.o, ${SRCFILES})

TARGET = server




all : ${BUILDDIR} $(TARGET)

${TARGET} : ${OBJFILES}
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $^

${BUILDDIR}/%.o : ${SRCDIR}/%.c
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $^

clean :
	rm -r ${BUILDDIR}
	rm $(TARGET)

${BUILDDIR} :
	mkdir ${BUILDDIR}

