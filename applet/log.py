__author__ = 'novokrest'

import time
import cStringIO
import logging

class Log:

    def __init__(self, *args):
        self.log = cStringIO.StringIO()
        if len(args) == 0:
            self.log_path = '/var/log/ydisk/log.lg'
        else:
            self.log_path = args[0]




    def PrintLog(self):
        logging.basicConfig(filename=self.log_path, level=logging.INFO, format='%(asctime)s%(levelname)s:%(message)s', datefmt='%d/%m/%Y %I:%M:%S %p\n')
        logging.info(self.log.getvalue())
        self.log.truncate(0)
