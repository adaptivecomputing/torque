PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -DUT_SENDMAIL_CMD=\"./fakemail.sh\" -I${PROG_ROOT}/ --coverage `xml2-config --cflags`
AM_LIBS = `xml2-config --libs`

lib_LTLIBRARIES = libsvr_mail.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_svr_mail

libsvr_mail_la_SOURCES = scaffolding.c ${PROG_ROOT}/svr_mail.c ${PROG_ROOT}/svr_format_job.c \
    ${PROG_ROOT}/../lib/Liblog/pbs_messages.c ${PROG_ROOT}/mail_throttler.cpp

#libsvr_mail_la_SOURCES = scaffolding.c ${PROG_ROOT}/svr_mail.c
libsvr_mail_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_svr_mail_SOURCES = test_svr_mail.c 

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
