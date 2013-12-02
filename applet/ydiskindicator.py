import gobject
import gtk
import appindicator
import threading

import dbus
import dbus.service
import subprocess
import getpass
from re import split


class YdiskDBus(dbus.service.Object):
    def __init__(self):
        self.bus = dbus.SessionBus()
        self.busName = dbus.service.BusName('edu.ydisk.Service', self.bus)
        dbus.service.Object.__init__(self, self.busName, '/edu/ydisk/YdiskObject')

        self.bus.add_signal_receiver(self.my_func, signal_name='for_ydisk',
                                     dbus_interface='edu.ydisk_glib.interface',
                                     bus_name='edu.ydisk_glib.service',
                                     path='/edu/ydisk_glib/object')

    def my_func(receiver=None, *args, **kwargs):
        print "Receive signal%s" % receiver



class YdiskTray(threading.Thread, dbus.service.Object):

    #Icons
    path = "/home/novokrest/PycharmProjects/SRW/icons/"
    mainIcon = path + "Ydisk_UFO_main.png"
    attentionIcon = path + "Ydisk_sync.png"

    messageIcon = path + "Ydisk_message3.png"

    syncIcon = path + "Ydisk_sync.png"
    quitIcon = path + "Ydisk_quit.png"

    yInd = appindicator.Indicator("YdiskIndicator", "ydiskindicator", appindicator.CATEGORY_APPLICATION_STATUS)

    #Commands

    sync_cmd = "yandex-disk sync -d "
    start_cmd = 'yandex-disk start'
    stop_cmd = 'yandex-disk stop'

    config_file_buf = ""
    config_file_path = ""

    exclude_dirs_list = []
    default_dir = ""


    def __init__(self):
        threading.Thread.__init__(self)

        self.yInd.set_icon(self.mainIcon)
        self.yInd.set_attention_icon(self.attentionIcon)

        self.yInd.set_status(appindicator.STATUS_ACTIVE)

        self.menu = gtk.Menu()

        syncIcon = gtk.Image()
        syncIcon.set_from_file(self.syncIcon)
        syncIcon.show()

        self.syncIconItem = gtk.ImageMenuItem("Synchronize")
        self.syncIconItem.set_image(syncIcon)
        self.syncIconItem.set_always_show_image(True)
        #self.syncIconItem.connect("activate", self.__Syncronization)
        self.syncIconItem.show()
        self.menu.append(self.syncIconItem)

        quitIcon = gtk.Image()
        quitIcon.set_from_file(self.quitIcon)
        quitIcon.show()

        self.quitIconItem = gtk.ImageMenuItem("Quit")
        self.quitIconItem.set_image(quitIcon)
        self.quitIconItem.set_always_show_image(True)
        self.quitIconItem.connect("activate", gtk.main_quit)
        self.quitIconItem.show()
        self.menu.append(self.quitIconItem)

        self.yInd.set_menu(self.menu)

        self.bus = dbus.SessionBus()
        self.busName = dbus.service.BusName('edu.ydisk.Service', self.bus)
        dbus.service.Object.__init__(self, self.busName, '/edu/ydisk/YdiskObject')

        self.ReadConfig()


    def ReadConfig(self):

        self.config_file_path = '/home/' + getpass.getuser() + '/.config/yandex-disk/config.cfg'

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


    def exec_command(self, cmd):

        self.yInd.set_status(appindicator.STATUS_ATTENTION)
        p = subprocess.Popen(cmd, shell=True)
        p.wait()
        self.yInd.set_status(appindicator.STATUS_ACTIVE)

        if not p.returncode:
            print("OK")


    @dbus.service.method('edu.ydisk.Service.Methods')
    def SyncMethod(self, message):

        sync_dir = message[7:]
        print ("I receive sync msg: %s\n" % sync_dir)

        if sync_dir in self.exclude_dirs_list:
            self.exclude_dirs_list.remove(sync_dir)
            self.WriteConfig()

        self.exec_command(self.sync_cmd + sync_dir)


    @dbus.service.method('edu.ydisk.Service.Methods')
    def UnsyncMethod(self, message):

        unsync_dir = message[7:]
        print ("I receive unsync msg: %s\n" % unsync_dir)

        if not (unsync_dir in self.exclude_dirs_list):
            print(self.exclude_dirs_list, unsync_dir)
            self.exclude_dirs_list.append(unsync_dir)
            self.WriteConfig()


    @dbus.service.method('edu.ydisk.Service.Methods')
    def SetDefaultDirMethod(self, message):

        default_dir = message[7:]
        print ("I receive default_dir msg: %s\n" % default_dir)
        if self.default_dir != default_dir:
            self.default_dir = default_dir
            self.WriteConfig()


    def run(self):

        gtk.threads_enter()
        gtk.main()
        #loop = gobject.MainLoop()
        #loop.run()
        gtk.threads_leave()