MARLIB=marlex.o \
	my.o \
	ml.o \
	LocaleApp.o

LEX=flex
YACC=bison

MACHINE=$(shell uname -m)
ifeq ($(MACHINE), BePC)
	LDFLAG = -cru
	LINKER = ar
	CXX = gcc
else
	LDFLAG = mwldppc
	LINKER = -dead on -xml
	CXX = mwcc
endif

all: liblocale.a

liblocale.a: $(MARLIB)
	$(LINKER) $(LDFLAG) liblocale.a $(MARLIB)
	mimeset -f $@

clean:
	rm *.o

.l.o:
	$(LEX) -t $< > ${<:l=cc}
	$(CXX) -o $@ $(CXXFLAGS) -c ${<:l=cc}
	@$(RM) ${<:l=cc}

.y.o:
	$(YACC) -d -p mar $<
	mv ${<:y=tab.c} ${<:y=tab.cc}
	$(CXX) -o $@ -c $(CXXFLAGS) ${<:y=tab.cc}
	$(RM) ${<:y=tab.cc}
.cpp.o:
	$(CC) -c $< $(INCLUDES) $(CFLAGS) -o $@

main.o : main.cc marcontrol.h reading.h writing.h
marlex.o : marlex.cc marlex.h
reading.o : reading.cc reading.h
writing.o : writing.cc marcontrol.h writing.h
ml.o : ml.l marlex.h minfo.h my.tab.h marcontrol.h
my.o : my.y marcontrol.h minfo.h marlex.h
LocaleApp.o : LocaleApp.h
