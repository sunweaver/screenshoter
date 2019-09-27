NULL =

bin_PROGRAMS = screenshoter

DISTCLEANFILES =			\
	Makefile.in			\
	aclocal.m4			\
	compile				\
	configure			\
	depcomp				\
	install-sh			\
	missing				\
	$(NULL)

screenshoter_SOURCES =			\
	screenshoter.c

screenshoter_CFLAGS =			\
	$(GTK3_CFLAGS)			\
	$(POPT_CFLAGS)			\
	-Wall -Werror			\
	-DBUILD_DIR="\"$(builddir)\""	\
	$(NULL)

screenshoter_LDADD =			\
	$(GTK3_LIBS)			\
	$(POPT_LIBS)			\
	$(NULL)
