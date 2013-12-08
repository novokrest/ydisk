__author__ = 'novokrest'

import getpass

class Config:

    config_file_buf = ""
    config_file_path = ""

    exclude_dirs_list = []
    default_dir = ""

    def __init__(self, *args):
        if len(args) == 0:
            self.config_file_path = '/home/' + getpass.getuser() + '/.config/yandex-disk/config.cfg'
        else:
            self.config_file_path = args[0]

    def ReadConfig(self):

         with open(self.config_file_path, mode='r') as config_file:
            for line in config_file.readlines():
                if 'exclude-dirs=' in line:
                    self.exclude_dirs_list = line.split('\"')[1].split(',')
                    if '' in self.exclude_dirs_list:
                        self.exclude_dirs_list.remove('')
                    continue
                if 'dir=' in line:
                    self.default_dir = line.split('\"')[1]
                    continue
                if line == "\n":
                    continue
                self.config_file_buf += line


    def WriteConfig(self):

        with open(self.config_file_path, mode='w') as config_file:

            default_dir_line = 'dir=\"' + self.default_dir + '\"\n'
            config_file.write(default_dir_line)

            exclude_line = 'exclude-dirs=\"' + ','.join(self.exclude_dirs_list) + '\"\n'
            config_file.write(exclude_line)

            config_file.write(self.config_file_buf)