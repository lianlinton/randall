# Make x86-64 random byte generators.

# Copyright 2015, 2020, 2021 Paul Eggert

# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

# Optimization level.  Change this -O2 to -Og or -O0 or whatever.
OPTIMIZE =

# The C compiler and its options.
CC = gcc
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra -fanalyzer \
  -march=native -mtune=native -mrdrnd

# The archiver command, its options and filename extension.
TAR = tar
TARFLAGS = --gzip --transform 's,^,randall/,'
TAREXT = tgz

default: randall

randall: randall.o options.o output.o rand64-hw.o rand64-sw.o rand64-file.o 
	$(CC) $(CFLAGS) -o $@ $^

randall.o: randall.c options.h output.h rand64-hw.h rand64-sw.h rand64-file.h
	$(CC) $(CFLAGS) -c $<

options.o: options.c options.h
	$(CC) $(CFLAGS) -c $<

output.o: output.c output.h
	$(CC) $(CFLAGS) -c $<

rand64-hw.o: rand64-hw.c rand64-hw.h
	$(CC) $(CFLAGS) -c $<
	
rand64-sw.o: rand64-sw.c rand64-sw.h
	$(CC) $(CFLAGS) -c $<

rand64-file.o: rand64-file.c rand64-file.h
	$(CC) $(CFLAGS) -c $<

assignment: randall-assignment.$(TAREXT)
assignment-files = COPYING Makefile randall.c
randall-assignment.$(TAREXT): $(assignment-files)
	$(TAR) $(TARFLAGS) -cf $@ $(assignment-files)

submission-tarball: randall-submission.$(TAREXT)
submission-files = $(assignment-files) \
  notes.txt \
  options.c options.h \
  output.c output.h \
  rand64-hw.c rand64-hw.h \
  rand64-sw.c rand64-sw.h \
  rand64-file.c rand64-file.h \
  randall.c \
  test-llm.txt

randall-submission.$(TAREXT): $(submission-files)
	$(TAR) $(TARFLAGS) -cf $@ $(submission-files)

repository-tarball:
	$(TAR) -czf randall-git.tgz .git

.PHONY: default clean assignment submission-tarball repository-tarball

clean:
	rm -f *.o *.$(TAREXT) randall

check:
	@./randall 0 > check-zero.out
	@wc -c < check-zero.out | grep -q '^0$$' && echo "Passed zero byte input check." || echo "Failed zero byte input check."
	@rm -f check-zero.out