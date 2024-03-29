# Makefile

include config.mk

all: prepare compile run

prepare:
	mkdir -p ${BUILD_DIR}
	cp -dr ${RES_DIR} ${BUILD_DIR}/${RES_DIR}

compile:
	${CC} ${FLAGS} ${LIB} ${O_RELEASE}

debug:
	gdb ./${BUILD_DIR}/${PROG}

run:
	./${BUILD_DIR}/${PROG}

clean:
	rm -r ${BUILD_DIR}
