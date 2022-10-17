UNAME := $(shell uname -s)

PROJECT_NAME = MLP

CXX=g++
CXXFLAGS=-c -pedantic -Wall -Wextra -Wshadow -Wconversion -Wunreachable-code -std=c++17
LDFLAGS=-lm -lpthread

DEBUG ?= 1

ifeq ($(DEBUG),1)
	CXXFLAGS+=-g
	CXXFLAGS+=-DDEBUG
else
	CXXFLAGS+=-O2
	CXXFLAGS+=-Werror
endif

MAINSRC=main.cc
MAINOBJ=$(MAINSRC:.cc=.o)
TESTSRC=tests.cc
TESTOBJ=$(TESTSRC:.cc=.o)
SRC=$(filter-out ./$(MAINSRC) ./$(TESTSRC), $(shell find . -type f -name "*.cc" -not -path "./view/*"))
OBJ=$(SRC:.cc=.o)

LIBDIR = lib

LIB_MATRIXPLUS_DIR = $(LIBDIR)/matrixplus
LIB_MATRIXPLUS = $(LIB_MATRIXPLUS_DIR)/s21_matrix_oop.a

BUILDDIR=build
EXECUTABLE=result_file

ifeq ($(UNAME), Linux)
TMPEXECUTABLE=$(PROJECT_NAME)
else ifeq ($(UNAME), Darwin)
TMPEXECUTABLE=$(PROJECT_NAME).app/Contents/MacOS/$(PROJECT_NAME)
endif

DOCUMENTS_DIR=docs
DOCUMENT_FILE=documentation
CONTAINER_DIR=mlp

.PHONY: all build build_gcc run install uninstall dvi dist tests gcov_report style cpplint leaks clean rebuild

all: build

build:
	mkdir -p $(BUILDDIR)
	cd $(BUILDDIR) && \
	qmake ../ && \
	make
	ln -sf $(BUILDDIR)/$(TMPEXECUTABLE) $(EXECUTABLE)

build_gcc: $(MAINOBJ) $(OBJ)
	$(CXX) $^ -o $(EXECUTABLE) $(LDFLAGS)

$(LIB_MATRIXPLUS):
	cd $(LIB_MATRIXPLUS_DIR) && make

run: build
	./$(EXECUTABLE)

install: build dvi
	mkdir -p $(CONTAINER_DIR)
	cp $(BUILDDIR)/$(TMPEXECUTABLE) $(CONTAINER_DIR)/$(EXECUTABLE)
	mkdir -p $(CONTAINER_DIR)/$(DOCUMENTS_DIR)
	cp $(DOCUMENT_FILE).pdf $(CONTAINER_DIR)/$(DOCUMENTS_DIR)

uninstall:
	rm -rf $(CONTAINER_DIR)

dvi:
	rubber --pdf $(DOCUMENTS_DIR)/$(DOCUMENT_FILE).tex

dist: install
	tar -cvf $(CONTAINER_DIR).tar $(CONTAINER_DIR)

tests: LDFLAGS+=-lgtest
tests: $(TESTOBJ) $(OBJ)
	$(CXX) $^ -o $(EXECUTABLE) $(LDFLAGS)
	./$(EXECUTABLE)

gcov_report: CXXFLAGS+=--coverage
gcov_report: LDFLAGS+=--coverage
gcov_report: tests
	lcov -t $(EXECUTABLE) -o $(LCOVEXEC) -c -d .
	lcov -r $(LCOVEXEC) "/usr*" -o $(LCOVEXEC)
	genhtml -o $(REPORTDIR) $(LCOVEXEC)

%.o: %.cc
	$(CXX) $(CXXFLAGS) $^ -o $@

style:
	cppcheck --std=c++17 --enable=all --suppress=missingInclude --suppress=unusedFunction -i ./$(BUILDDIR)/ -i ./view/qcustomplot/ -i ./view/scribblearea/ ./
	clang-format -n $(shell find . -type f \( -name "*.h" -o -name "*.cc" \) -not -path "./$(BUILDDIR)/*" -not -path "./view/qcustomplot/*" -not -path "./view/scribblearea/*")

cpplint:
	cpplint --extensions=cc,h --recursive --exclude=./$(BUILDDIR)/ --exclude=./view/qcustomplot/ --exclude=./view/scribblearea/ ./

leaks: tests
	CK_FORK=no valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXECUTABLE)

clean:
	cd $(LIB_MATRIXPLUS_DIR) && make clean
	rm -rf $(BUILDDIR) $(MAINOBJ) $(TESTOBJ) $(OBJ) $(EXECUTABLE) $(shell find . -name "*.gcno") $(shell find . -name "*.gcda") *.gcov $(LCOVEXEC) $(REPORTDIR) $(DOCUMENT_FILE).* $(CONTAINER_DIR) *.tar

rebuild: clean all
