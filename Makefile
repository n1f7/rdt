prog:=read
prog_version:=0.0

tgz:=tar.gz

distname:=$(prog)-$(prog_version)
dist:=$(distname).$(tgz)
overlay?=$(HOME)/overlay/app-text/$(prog)

srcdir:=src
builddir=build

sources:=\
	$(srcdir)/read.cpp

objects:=$(patsubst %.cpp,$(builddir)/%.o,$(notdir $(sources)))

common_cxxflags:=-std=c++1z -stdlib=libc++ -Wall -pedantic -march=native

debug_cxxflags:=-ggdb -O0
release_cxxflags:=-g0 -O3

CXX?=clang++

compile=$(CXX) $(CXXFLAGS) $(CPPFLAGS)
link=$(CXX) $(CXXFLAGS) $(LDLIBS) $(LDFLAGS)

.PHONY: all debug release clean dist distclean allclean strip
.PHONY: localise format

debug: CXXFLAGS?=$(debug_cxxflags) $(common_cxxflags)
debug: CPPFLAGS?=$(common_cppflags)
debug: LDLIBS?=
debug: $(builddir) $(builddir)/$(prog)

release: CXXFLAGS?=$(release_cxxflags) $(common_cxxflags)
release: CPPFLAGS?=$(common_cppflags) -DNDEBUG
release: LDLIBS?=
release: $(builddir) strip

all: release dist

$(builddir)/read.o:\
	$(srcdir)/bo.h\
	$(srcdir)/rdt.h\
	$(srcdir)/argx.h\
	$(srcdir)/error.h\

format: ; clang-format -i $(srcdir)/*

$(builddir)/%.o: $(srcdir)/%.cpp
	$(compile) -c $< -o $(builddir)/$(notdir $@)

$(builddir)/$(prog): $(objects)
	$(link) $(patsubst %,$(builddir)/%,$(notdir $(objects))) -o $@ 

$(builddir): ; mkdir $@

strip: $(builddir)/$(prog)
	strip $(builddir)/$(prog)

clean:
	rm -f $(builddir)/$(prog)
	rm -f $(patsubst %,$(builddir)/%,$(notdir $(objects)))

dist:
	mkdir -p $(builddir)/$(distname)/$(srcdir)
	cp $(srcdir)/* $(builddir)/$(distname)/$(srcdir)/
	cp Makefile $(builddir)/$(distname)/
	tar cvzf $(builddir)/$(dist) $(builddir)/$(distname)
	rm -rf $(builddir)/$(distname)

distclean: ; rm -f $(builddir)/$(dist)

allclean: clean distclean
