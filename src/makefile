MARLIB=marlex.o \
	my.o \
	ml.o \
	LocaleApp.o

LEX=flex
YACC=bison
NAME = liblocale.a
MACHINE=$(shell uname -m)
ifeq ($(MACHINE), BePC)
	LDFLAG = -cru
	LINKER = ar
	CXX = gcc
else
	LINKER = mwldppc
	LDFLAG = -dead on -xml -o
	NAME = liblocale_ppc.a
	CXX = mwcc
endif

all: $(NAME) 

$(NAME): $(MARLIB)
	$(LINKER) $(LDFLAG) $(NAME) $(MARLIB)
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
	$(CXX) -c $< $(INCLUDES) $(CFLAGS) -o $@

main.o : main.cc marcontrol.h reading.h writing.h
marlex.o : marlex.cc marlex.h
reading.o : reading.cc reading.h
writing.o : writing.cc marcontrol.h writing.h
ml.o : ml.l marlex.h minfo.h my.tab.h marcontrol.h
my.o : my.y marcontrol.h minfo.h marlex.h
LocaleApp.o : LocaleApp.h
