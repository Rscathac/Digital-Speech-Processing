# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
SRIPATH ?= /home/student/05/b05902115/dsp/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
.PHONY: all clean map run

RESULTDIR = result1
ANSDIR = result2
TESTDATA = testdata

all: $(TARGET) 

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

run:
	@#TODO How to run your code toward different txt? 
	@if [ ! -d $(RESULTDIR) ]; then \
		mkdir -p $(RESULTDIR); \
	fi;
	@for i in $(shell seq 1 10) ; do \
	    echo "Running $$i.txt"; \
	    ./mydisambig -text $(TESTDATA)/seg_$$i.txt -map $(TO) -lm $(LM) -order 2 > $(RESULTDIR)/$$i.txt; \
	done;

ans:
	@if [ ! -d $(ANSDIR) ]; then \
		mkdir -p $(ANSDIR); \
	fi;
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt"; \
		$(SRIPATH)/bin/i686-m64/disambig -text $(TESTDATA)/seg_$$i.txt -map $(TO) -lm $(LM) -order 2 > $(ANSDIR)/$$i.txt; \
	done;

map:
	@#TODO How to map?
	echo "Mapping!"
	@#./mapping $(FROM) $(TO)
	@#matlab mapping.m $(FROM) $(TO)
	python3 ZhuYin-Big5.py $(FROM) $(TO)
	@#sh mapping.sh $(FROM) $(TO)
	@#perl mapping.pl Big5-ZhuYin.map ZhuYin-Big5.map

build_lm:
	perl separator_big5.pl corpus.txt > corpus_seg.txt
	$(SRIPATH)/bin/i686-m64/ngram-count -text corpus_seg.txt -write lm.cnt -order 2
	$(SRIPATH)/bin/i686-m64/ngram-count -read lm.cnt -lm bigram.lm -unk -order 2

ngram:
	@for i in $(shell seq 1 10); do \
		perl separator_big5.pl $(TESTDATA)/$$i.txt > $(TESTDATA)/seg_$$i.txt; \
	done;
	

clean:
	$(RM) $(OBJ) $(TARGET)

