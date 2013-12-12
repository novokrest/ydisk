__author__ = 'novokrest'

import getpass

class Config:
    def __init__(self, **args):
        self.config_buf = ""
        self.exclude_dirs = []
        if "path" not in args:
            self.config_path = '/home/' + getpass.getuser() + '/.config/yandex-disk/config.cfg'
        else:
            self.config_path = args["path"]
        self.read_config()

    def read_config(self):
         with open(self.config_path, mode='r') as config:
            for line in config.readlines():
                if 'exclude-dirs=' in line:
                    self.exclude_dirs = line.split('\"')[1].split(',')
                else:
                    self.config_buf += line

    def write_config(self):
        with open(self.config_path, mode='w') as config:
            exclude_line = 'exclude-dirs="' + ','.join(filter(lambda str: str != '', self.exclude_dirs)) + '"\n'
            config.write(exclude_line)
            config.write(self.config_buf)

    def include(self, path):
        if path in self.exclude_dirs:
            self.exclude_dirs.remove(path)
            self.write_config()

    def exclude(self, path):
        if path not in self.exclude_dirs:
            self.exclude_dirs.append(path)
            self.write_config()