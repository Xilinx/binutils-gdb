#!/bin/sh

# incremental_test.sh -- test that incremental linking information is correct.

# Copyright 2009 Free Software Foundation, Inc.
# Written by Rafael Avila de Espindola <espindola@google.com>

# This file is part of gold.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.

set -e
cat incremental_test.cmdline | grep "gcctestdir/ld " | cut -d ' ' -f 2- > \
  actual

cat incremental_test.stdout | grep "Link command line" | cut -d : -f 2- | \
  cut -d ' ' -f 3- | sed "s/'//g" > recorded

diff actual recorded
rm actual recorded

exit 0