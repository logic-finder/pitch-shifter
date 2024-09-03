## Settings
srcdir := src
objdir := $(srcdir)/.object
depdir := $(srcdir)/.depend
headir := $(srcdir)/header

SHELL := /bin/sh
CC := gcc
CFLAGS := -O -Wall -W -pedantic -g
CPPFLAGS := -I $(headir)
SUFFIXES :=
SUFFIXES := .c .o .h

define color_str
"\033[$1m$2\033[0m"
endef
name_str = $(call color_str,38;5;49;1,$1)
build_completed_str = $(call color_str,48;5;222;30, [ BUILD COMPLETED ] )
notice_str = $(call color_str,48;5;150;30, [ NOTICE ] )

## Compilations
program := pitsh
sources := $(wildcard $(srcdir)/*.c)
objects := $(sources:$(srcdir)/%.c=$(objdir)/%.o)

### This is the default goal. ###
$(program): $(objects)
	$(CC) $^ $(CFLAGS) -o $@
	@echo $(build_completed_str) $(call name_str,./$@)

# .d file contains a list of .h files on which the .c file depends.
include $(sources:$(srcdir)/%.c=$(depdir)/%.d)

$(objdir)/%.o:
	$(CC) $< $(CPPFLAGS) $(CFLAGS) -c -o $@
	@echo $(notice_str) $(call name_str,$@) has been created.

# $$$$ expands to $$, which means "the process ID of the shell." 
$(depdir)/%.d: $(srcdir)/%.c
	@set -e; rm -f $@; \
	 $(CC) $< $(CPPFLAGS) -MM > $@.$$$$; \
	 printf "%s%s" "$(objdir)/" "$$(cat $@.$$$$)" > $@; \
	 rm -f $@.$$$$

## Miscellaneous Tasks
.PHONY: clean
clean:
	rm -f pitsh
	rm -f $(depdir)/* $(objdir)/*

.PHONY: help
cmd_group = $(call color_str,48;5;148;30, $1 )
cmd_color = $(call color_str,38;5;220,$1)
cmd_arg_color = $(call color_str,36,$1)
help:
	@echo The below are the list of available commands from this Makefile.
	@echo
	@echo $(call cmd_group,1. MAKING ACTIONS)
	@echo "	make"
	@echo $(call color_str,90,	or )make $(call cmd_color,$(program))"	builds this program."
	@echo
	@echo $(call cmd_group,2. CLEANING ACTIONS)
	@echo "	make "$(call cmd_color,clean)"	deletes ./pitsh, $(depdir)/* and $(objdir)/*."
	@echo
	@echo $(call cmd_group,3. MISCELLANEOUS)
	@echo "	make "$(call cmd_color,help)"	prints this long manual on the screen that you are reading now."
