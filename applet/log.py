__author__ = 'novokrest'

import time

class Log:

    def __init__(self, *args):
        if len(args) == 0:
            self.log_path = '/var/log/ydisk/log.lg'
        else:
            self.log_path = args[0]

    def PrintLog(self, message):
        with open(self.log_path, 'a') as log:
            log.write('\n' + time.asctime() + '\n' + message)