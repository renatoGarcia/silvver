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

from string import maketrans

def makeHeaderDefine(environment, header_path):
    header_path = '-DHAVE_' + \
                  header_path.translate(maketrans('/.','__')).upper()
    environment.Append(CXXFLAGS = header_path)

class NotCheckedError(Exception):
    pass

class Library:
    def __init__(self, long_name, short_name, headers=None,
                 libraries=None):
        self.LONG_NAME = long_name
        self.SHORT_NAME = short_name
        self._headers = headers
        self._libraries = libraries
        self._found = None

    def __nonzero__(self):
        if self._found == None:
            raise NotCheckedError
        return self._found

    def add_options(self, function):
        function('--lib_' + self.SHORT_NAME + '_suffix',
                 nargs = 1,
                 type = 'string',
                 action = 'callback',
                 callback = self._add_suffix,
                 metavar = 'SUFFIX',
                 help = 'Suffix of ' + self.LONG_NAME + ' libraries')

    def _add_suffix(self, option, opt, value, parser):
        for index in range(len(self._libraries)):
            self._libraries[index] += value

    def check(self, conf):
        for lib in self._libraries:
            if not conf.CheckLib(lib):
                self._found = False
                return False
        for header in self._headers:
            if not conf.CheckCXXHeader(header):
                self._found = False
                return False

        self._found = True
        return True

class PkgConfig:
    def __init__(self, long_name, short_name, pkg_config):
        self.LONG_NAME = long_name
        self.SHORT_NAME = short_name
        self._pkg_config = pkg_config
        self._found = None

    def __nonzero__(self):
        if self._found == None:
            raise NotCheckedError
        return self._found

    def add_options(self, function):
        function('--' + self.SHORT_NAME + '_pkgconfig',
                 nargs = 1,
                 type = 'string',
                 action = 'callback',
                 callback = self._rename_pkgconfig,
                 metavar= 'FILENAME',
                 help = 'Name of pkg-config file of ' + self.LONG_NAME +
                        ' library')

    def _rename_pkgconfig(self, option, opt, value, parser):
        self._pkg_config = value

    def check(self, conf):
        print 'Checking for pkg-config %s...' %  self.LONG_NAME,
        try:
            conf.env.ParseConfig("pkg-config " + self._pkg_config +
                                 " --cflags --libs")
        except OSError:
            print 'no'
            self._found = False
            return False

        print 'yes'
        self._found = True
        return True

class LibraryDict(dict):

    def __init__(self):
        dict.__init__(self)
        self._conflicts = []

    def __getitem__(self, key):
        try:
            return dict.__getitem__(self,key)
        except KeyError:
            return None

    def add_options(self, function):
        for key in self:
            self[key].add_options(function)
        for conflict in self._conflicts:
            conflict.add_options(function)

    def add_conflict(self, conflict):
        self._conflicts.append(conflict)

    def check_libs(self, conf):
        found_all = True
        for key in self:
            if not self[key].check(conf):
                found_all =  False

        for conflict in self._conflicts:
            lib_result = conflict.resolve(conf)
            if lib_result:
                self[lib_result.SHORT_NAME] = lib_result
            else:
                found_all = False

        return found_all

class Conflict(dict):
    def __init__(self, long_name, short_name):
        dict.__init__(self)
        self.LONG_NAME = long_name
        self.SHORT_NAME = short_name

    def add_options(self, function):
        function('--enable_' + self.SHORT_NAME,
                 dest = 'enable_' + self.SHORT_NAME,
                 nargs = 1,
                 type = 'choice',
                 choices = ['any', 'none'] + self.keys(),
                 action = 'callback',
                 callback = self._select_lib,
                 metavar = '(any|none|'+'|'.join(self.keys())+')',
                 help = 'Enable one of ' + self.LONG_NAME + ' libraries')
        for lib in self.itervalues():
            lib.add_options(function)

    def _select_lib(self, option, opt, value, parser):
        if value == 'any':
            return
        elif value == 'none':
            self.clear()
        else:
            keys = self.keys()
            for key in keys:
                if key != value:
                    del self[key]

    def resolve(self, conf):
        for key in self:
            if self[key].check(conf):
                return self[key]

        return None
