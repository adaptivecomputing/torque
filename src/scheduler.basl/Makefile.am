include $(top_srcdir)/buildutils/config.mk

PBS_LIBS = ../lib/Libpbs/libtorque.la

sbin_PROGRAMS = pbs_sched
noinst_PROGRAMS = basl2c

AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

pbs_sched_LDADD = $(PBS_LIBS) $(SCHD_LIBS) pbs_sched.o
pbs_sched_SOURCES = af.c af_cnode.c af_cnodemap.c af_config.c af_job.c \
		    af_que.c af_resmom.c af_server.c af_system.c af.h

basl2c_LDADD = $(SCHD_LIBS)
basl2c_SOURCES = af.c Basl2c.c CodeGen.c List.c Node.c Semantic.c SymTab.c \
		 Lexer.l Parser.y CodeGen.h

EXTRA_DIST = Lexer.h List.h af_system.h Node.h ParLexGlob.h Semantic.h \
	     SymTab.h SymTabGlob.h af_cnode.h af_cnodemap.h af_config.h \
	     af_job.h af_que.h af_resmom.h af_server.h \
	     sample.programs/af.basl sample.programs/af2.basl \
	     sample.programs/loadave1.basl sample.programs/loadave2.basl \
	     sample.programs/loadave3.basl sample.programs/loadave4.basl \
	     sample.programs/nas.basl sample.programs/nqs.basl \
	     samples/af.basl samples/af2.basl \
	     samples/fifo_byqueue.basl samples/loadave1.basl \
	     samples/loadave2.basl samples/loadave3.basl \
	     samples/loadave4.basl samples/nas.basl \
	     samples/nodes.basl samples/nodes2.basl \
	     samples/nqs.basl

dist-hook:
	rm -f $(distdir)/Lexer.c $(distdir)/Parser.c

AM_YFLAGS = -d

pbs_sched.c: samples/$(SCHD_CODE) basl2c
	./basl2c $(srcdir)/samples/$(SCHD_CODE)
		
Lexer.o: Lexer.c y.tab.h
	
Lexer.c: Lexer.l
	$(LEXCOMPILE) $< && mv $(LEX_OUTPUT_ROOT).c $@

y.tab.h: Parser.c

Parser.c: Parser.y
	$(YACCCOMPILE) $< && mv y.tab.c $@

MOSTLYCLEANFILES = Parser.c y.tab.h Lexer.c pbs_sched.c
MOSTLYCLEANFILES += *.gcda *.gcno *.gcov

install-exec-hook:
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qschedd$(program_suffix)$(EXEEXT)
	ln -s $(program_prefix)pbs_sched$(program_suffix)$(EXEEXT) \
	  $(DESTDIR)$(sbindir)/$(program_prefix)qschedd$(program_suffix)$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qschedd$(program_suffix)$(EXEEXT)
