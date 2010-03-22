#!/usr/bin/env python
#
# Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>,
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3, as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import sys

try:
    import opencv.cv
    sys.exit(1)
except ImportError:
    pass

try:
    import cv
    sys.exit(2)
except ImportError:
    pass

sys.exit(0)
