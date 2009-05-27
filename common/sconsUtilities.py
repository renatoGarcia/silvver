from string import maketrans

def makeHeaderDefine(environment, header_path):
    header_path = '-DHAVE_' + \
                  header_path.translate(maketrans('/.','__')).upper()
    environment.Append(CXXFLAGS = header_path)

class RelatedLibs(object):
    def __init__(self, long_name, short_name, libraries_list):
        self.__members = libraries_list
        self.long_name = long_name
        self.short_name = short_name

        self.__count = 0

    def __iter__(self):
        return self

    def next(self):
        if self.__count >= len(self.__members):
            self.__count = 0
            raise StopIteration
        else:
            member = self.__members[self.__count]
            self.__count += 1
            return member

    def add_suffix(self, suffix):
        for index in range(len(self.__members)):
            self.__members[index] += suffix

class RelatedLibsGroup(object):

    def __init__(self, related_libs_list):
        self.__members = {}
        for related_libs in related_libs_list:
            self.__members[related_libs.short_name] = related_libs

        self.__count = 0

    def __iter__(self):
        return self

    def next(self):
        if self.__count >= len(self.__members):
            self.__count = 0
            raise StopIteration
        else:
            member = self.__members.keys()[self.__count]
            self.__count += 1
            return self.__members[member]

    def __getitem__(self, key):
        return self.__members[key]

    def add_item(self, related_libs):
        self.__members[related_libs.short_name] = related_libs
