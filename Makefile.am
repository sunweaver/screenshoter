NULL =

DISTCLEANFILES =

bin_PROGRAMS = screenshoter

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
