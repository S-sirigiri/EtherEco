CC = g++
CFLAGS = -g -Wall -Werror

SRCDIR := src
BUILDDIR := build
INCDIR := inc
LDFLAGS :=

INC = -I${INCDIR}
SRCFILES := $(wildcard ${SRCDIR}/*.cpp)
OBJFILES := $(patsubst ${SRCDIR}/%.cpp, ${BUILDDIR}/%.o, ${SRCFILES})

TARGET = client




all : ${BUILDDIR} $(TARGET)

${TARGET} : ${OBJFILES}
	$(CC) $(CFLAGS) $(LDFLAGS) $(INC) -o $@ $^

${BUILDDIR}/%.o : ${SRCDIR}/%.cpp
	$(CC) $(CFLAGS) $(INC) -c -o $@ $^

clean :
	rm -r ${BUILDDIR}
	rm $(TARGET)

${BUILDDIR} :
	mkdir ${BUILDDIR}

