INCLUDES  = @CHECK_CFLAGS@
AUTOMAKE_OPTIONS = foreign
if HAVE_CHECK
TESTS = check_o5mreader
else
TESTS =
endif
check_PROGRAMS = check_o5mreader
check_o5mreader_SOURCES = check_o5mreader.c $(top_builddir)/src/o5mreader.h
check_o5mreader_CFLAGS = @CHECK_CFLAGS@
check_o5mreader_LDADD = $(top_builddir)/src/libo5mreader.la @CHECK_LIBS@
CLEANFILES = check_o5mreader.log
