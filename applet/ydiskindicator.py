import gobject
import gtk
import appindicator
import threading

import dbus
import dbus.service
import subprocess
import getpass
from re import split

import config
import log

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
    path = "/usr/share/ydisk/rc/"
    mainIcon = path + "Ydisk_UFO_main.png"
    attentionIcon = path + "Ydisk_sync.png"

    messageIcon = path + "Ydisk_message3.png"

    syncIcon = path + "Ydisk_sync.png"
    quitIcon = path + "Ydisk_quit.png"

    yInd = appindicator.Indicator("YdiskIndicator", "ydiskindicator", appindicator.CATEGORY_APPLICATION_STATUS)

    #Commands

    sync_cmd = 'yandex-disk sync -d '
    sync_ro_cmd = 'yandex-disk sync --read-only -d '
    sync_ow_cmd = 'yandex-disk sync --read-only --overwrite -d '

    start_cmd = 'yandex-disk start'
    stop_cmd = 'yandex-disk stop'
    status_cmd = 'yandex-disk status'


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
        self.quitIconItem.connect("activate", self.quit)
        self.quitIconItem.show()
        self.menu.append(self.quitIconItem)

        self.yInd.set_menu(self.menu)

        self.bus = dbus.SessionBus()
        self.busName = dbus.service.BusName('edu.ydisk.Service', self.bus)
        dbus.service.Object.__init__(self, self.busName, '/edu/ydisk/YdiskObject')

        self.logfile = log.Log()
        self.configfile = config.Config()
        self.configfile.ReadConfig()


    def exec_command(self, cmd):

        self.yInd.set_status(appindicator.STATUS_ATTENTION)
        log = open(self.logfile.log_path, 'a')
        p = subprocess.call(cmd, stdout=log, stderr=log, shell=True)
        log.close()
        self.yInd.set_status(appindicator.STATUS_ACTIVE)

        if not p:
            self.logfile.PrintLog('OK\n')


    @dbus.service.method('edu.ydisk.Service.Methods')
    def SyncMethod(self, message):

        sync_dir = message[7:]
        self.logfile.PrintLog("Directory for synchronization: %s\n" % sync_dir)

        if sync_dir in self.configfile.exclude_dirs_list:
            self.configfile.exclude_dirs_list.remove(sync_dir)
            self.configfile.WriteConfig()

        self.exec_command(self.sync_cmd + sync_dir)


    @dbus.service.method('edu.ydisk.Service.Methods')
    def SyncROMethod(self, message):

        sync_dir = message[7:]
        self.logfile.PrintLog("Directory for synchronization read-only: %s\n" % sync_dir)

        if sync_dir in self.configfile.exclude_dirs_list:
            self.configfile.exclude_dirs_list.remove(sync_dir)
            self.configfile.WriteConfig()

        self.exec_command(self.sync_ro_cmd + sync_dir)


    @dbus.service.method('edu.ydisk.Service.Methods')
    def SyncOWMethod(self, message):

        sync_dir = message[7:]
        self.logfile.PrintLog("Directory for synchronization read-only overwrite: %s\n" % sync_dir)

        if sync_dir in self.configfile.exclude_dirs_list:
            self.configfile.exclude_dirs_list.remove(sync_dir)
            self.configfile.WriteConfig()

        self.exec_command(self.sync_ow_cmd + sync_dir)


    @dbus.service.method('edu.ydisk.Service.Methods')
    def UnsyncMethod(self, message):

        unsync_dir = message[7:]
        self.logfile.PrintLog("Directory for unsynchronization: %s\n" % unsync_dir)

        if not (unsync_dir in self.configfile.exclude_dirs_list):
            self.configfile.exclude_dirs_list.append(unsync_dir)
            self.configfile.WriteConfig()


    @dbus.service.method('edu.ydisk.Service.Methods')
    def SetDefaultDirMethod(self, message):

        default_dir = message[7:]
        print ("I receive default_dir msg: %s\n" % default_dir)
        if self.configfile.default_dir != default_dir:
            self.configfile.default_dir = default_dir
            self.configfile.WriteConfig()


    def run(self):

        gtk.threads_enter()
        gtk.main()
        #loop = gobject.MainLoop()
        #loop.run()
        gtk.threads_leave()

    def quit(self, widget):
        self.bus.close()
        gtk.main_quit()