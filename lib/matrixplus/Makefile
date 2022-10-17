CXX=g++
CXXFLAGS=-c -O2 -pedantic -Wall -Wextra -Wshadow -Wconversion -Wunreachable-code -std=c++17
LDFLAGS=-lm

LIBSRC=$(wildcard *.cc)
LIBOBJ=$(LIBSRC:.cc=.o)
STATICLIB=s21_matrix_oop.a

.PHONY: all build clean rebuild $(STATICLIB)

all: build

build: $(STATICLIB)

%.o: %.cc
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf *.o $(EXECUTABLE) $(STATICLIB)

rebuild: clean all

$(STATICLIB): $(LIBOBJ)
	ar rcs $@ $^
