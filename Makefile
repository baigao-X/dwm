# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = drw.c dwm.c util.c
OBJ = ${SRC:.c=.o}

all: check options dwm

check:
	@ [ -f "config.h" ] || echo -e "\033[31mconfig.h not found .'\033[0m"
	@ [ -f "scripts/autostart.sh" ] || echo -e "\033[31mscripts/autostart.sh not found.'\033[0m"
	@ [ -d "scripts/statusbar" ] || echo -e "\033[31mscripts/statusbar/ not found.'\033[0m"
	@ ([ -f "config.h" ] && [ -f "scripts/autostart.sh" ] && [ -d "scripts/statusbar" ]) || exit 1

options:
	@echo dwm build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

dwm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f dwm ${OBJ} dwm-${VERSION}.tar.gz

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwm ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < dwm.1 > ${DESTDIR}${MANPREFIX}/man1/dwm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/dwm.1
	rm -f dwm ${OBJ} dwm-${VERSION}.tar.gz

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/dwm\
		${DESTDIR}${MANPREFIX}/man1/dwm.1

.PHONY: all check options clean install uninstall
