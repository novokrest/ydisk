import gtk
import appindicator
import pynotify
import threading
import dbus
import dbus.service
import subprocess
import os
import logging
import config



class YdiskTray(threading.Thread, dbus.service.Object):
    yInd = appindicator.Indicator("YdiskIndicator", "ydiskindicator", appindicator.CATEGORY_APPLICATION_STATUS)
    #Commands
    cmdlines = {"readonly": "yandex-disk sync --read-only -d ",
                "overwrite": "yandex-disk sync --read-only --overwrite -d ",
                "default": "yandex-disk sync -d "}

    notify_messages = {'readonly': 'Start read-only synchronization:\n%s',
                      'overwrite': 'Start overwrite synchronization:\n%s',
                      'default': 'Start folder synchronization:\n%s',
                      'on': 'Folder add in exclude list:\n%s',
                      'off': 'Folder delete from exclude list:\n%s',
                      'success': 'Successfully synchronized:\n%s',
                      'error': ''}

    def __init__(self, path, log_path):
        threading.Thread.__init__(self)
        pynotify.init('ydisk-notify')
        self.bus = dbus.SessionBus()
        busName = dbus.service.BusName('edu.ydisk.Service', self.bus)
        dbus.service.Object.__init__(self, busName, '/edu/ydisk/YdiskObject')
        self.path = path
        self.log_path = log_path
        self.create_log()

        self.yInd.set_icon(self.path + "Ydisk_UFO_main.png")
        self.yInd.set_attention_icon(self.path + "Ydisk_sync.png")
        self.yInd.set_status(appindicator.STATUS_ACTIVE)
        self.yInd.set_menu(self.create_menu())

        self.config = config.Config()

    def create_log(self):
        if not os.path.exists(self.log_path):
            os.mkdir(self.log_path)
        log = self.log_path + 'yd.log'
        open(log, 'a').close()
        logging.basicConfig(filename=log,
                            level=logging.INFO,
                            format='%(asctime)s%(levelname)s:%(message)s',
                            datefmt='%d/%m/%Y %I:%M:%S %p\n')

    def delete_log(self):
        os.remove(self.log_path + 'yd.log')

    def create_menu(self):
        menu = gtk.Menu()
        menu.append(self.create_quit_item())
        return menu

    def create_quit_item(self):
        return self.create_icon_item("Quit", self.create_icon(self.path + "Ydisk_quit.png"), self.quit)

    def create_icon(self, path):
        icon = gtk.Image()
        icon.set_from_file(path)
        icon.show()
        return icon

    def create_icon_item(self, name, icon, callback):
        icon_item = gtk.ImageMenuItem(name)
        icon_item.set_image(icon)
        icon_item.set_always_show_image(True)
        icon_item.connect("activate", callback)
        icon_item.show()
        return icon_item

    def exec_command(self, cmd):
        self.yInd.set_status(appindicator.STATUS_ATTENTION)
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        output, error = p.communicate()
        if not p.returncode:
            logging.info(output)
        else:
            logging.info(error)
            self.notify_messages['error'] = error
        self.yInd.set_status(appindicator.STATUS_ACTIVE)
        return p.returncode

    @dbus.service.method('edu.ydisk.Service.Methods')
    def sync(self, sync_dir, mode):
        path = sync_dir[7:]
        self.config.include(path)
        logging.info("Directory for %s synchronization: %s\n" % (mode, path))
        self.notify(path, mode)
        result = self.exec_command(self.cmdlines[mode] + path)
        if not result:
            self.notify(path, 'success')
        else:
            self.notify(path, 'error')

    @dbus.service.method('edu.ydisk.Service.Methods')
    def unsync(self, unsync_dir, mode):
        path = unsync_dir[7:]
        if mode == 'on':
            self.config.exclude(path)
        if mode == 'off':
            self.config.include(path)
        logging.info('Directory for unsynchronization %s: %s\n' % (mode, path))
        self.notify(path, mode)

    def notify(self, path, mode):
        pynotify.Notification("Yandex-disk",
                              self.notify_messages[mode] % path,
                              self.path + "Ydisk_UFO.png").show()

    def run(self):
        gtk.threads_enter()
        gtk.main()
        gtk.threads_leave()

    def quit(self, widget):
        self.bus.close()
        self.delete_log()
        gtk.main_quit()