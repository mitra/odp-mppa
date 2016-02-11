include $(top_srcdir)/test/Makefile.inc

ODP_MODULES = buffer \
	      classification \
	      config \
	      cpumask \
	      errno \
	      hash \
	      init \
	      queue \
	      packet \
	      pktio \
	      pool \
	      scheduler \
	      std_clib \
	      synchronizers \
	      thread \
	      time \
	      timer \
	      shmem \
	      system

if crypto
  ODP_MODULES  += crypto random
endif


SUBDIRS = common $(ODP_MODULES)

