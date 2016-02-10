TESTS_ENVIRONMENT =
AUTOMAKE_OPTIONS = parallel-tests

include $(top_srcdir)/platform/@with_platform@/Makefile.inc
LIB   = $(top_builddir)/lib

#in the following line, the libs using the symbols should come before
#the libs containing them! The includer is given a chance to add things
#before libodp by setting PRE_LDADD before the inclusion.
LDADD = $(PRE_LDADD) $(LIB)/libodphelper.la $(LIB)/libodp.la

INCFLAGS = -I$(top_srcdir)/test \
	-I$(top_srcdir)/platform/@with_platform@/include \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/helper/include
AM_CFLAGS += $(INCFLAGS)
AM_CXXFLAGS = $(INCFLAGS)

AM_LDFLAGS += -L$(LIB)

@VALGRIND_CHECK_RULES@
valgrind_tools = memcheck drd sgcheck

TESTS_ENVIRONMENT+= ODP_PLATFORM=${with_platform} EXEEXT=${EXEEXT}
bindir      = $(datarootdir)/$(subst $(top_srcdir)/,,$(srcdir))

install-exec-local:
	mkdir -p $(bindir)
	( echo "#!/bin/bash -ex"; \
	  echo "export $(TESTS_ENVIRONMENT)";\
	  echo 'cd $$(readlink -e $$(dirname $$0))';\
	  for test in $(TESTS); do \
		echo "$$(echo $(subst $(top_srcdir)/,,$(srcdir)) | sed -e 's/[a-zA-Z0-9]*/../g')/../../ktest-wrapper.sh $${test}"; \
	  done) > $(bindir)/runtests.sh
	chmod +x $(bindir)/runtests.sh

