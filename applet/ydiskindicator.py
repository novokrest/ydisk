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
                "default": "yandex-disk sync -d ",
                "store": 'cp -R %s %s',
                "stop": "yandex-disk stop",
                "start_default": "yandex-disk start",
                "start_readonly": "yandex-disk start  --read-only",
                "start_overwrite": "yandex-disk start --read-only --overwrite",
                "open_folder": "nautilus "}

    notify_messages = {'readonly': 'Start read-only synchronization:\n%s',
                      'overwrite': 'Start overwrite synchronization:\n%s',
                      'default': 'Start folder synchronization:\n%s',
                      'store': 'Folder add to YandexDisk:\n%s',
                      'unsync': 'Folder add in exclude list:\n%s',
                      'off': 'Folder delete from exclude list:\n%s',
                      'success': 'Successfully synchronized:\n%s',
                      'error': '',
                      'restart': 'Successfully restarted'}

    def __init__(self, path, log_path):
        threading.Thread.__init__(self)
        pynotify.init('ydisk-notify')
        self.bus = dbus.SessionBus()
        busName = dbus.service.BusName('edu.ydisk.Service', self.bus)
        dbus.service.Object.__init__(self, busName, '/edu/ydisk/YdiskObject')
        self.path = path
        self.log_path = log_path
        self.create_log()
        self.exec_command(self.cmdlines["start_default"])

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
        menu.append(self.create_open_item())
        menu.append(self.create_restart_item())
        menu.append(self.create_quit_item())
        return menu

    def create_open_item(self):
        return self.create_icon_item("Open YandexDisk", self.create_icon(self.path + "Ydisk_folder.png"), self.open_folder)

    def create_restart_item(self):
        return self.create_icon_item("Restart", self.create_icon(self.path + "Ydisk_refresh.png"), self.restart)

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
        logging.info(cmd)
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        output, error = p.communicate()
        if not p.returncode:
            logging.info(output)
        else:
            logging.info(output + error)
            self.notify_messages['error'] = output + error + '%s'
        self.yInd.set_status(appindicator.STATUS_ACTIVE)
        return p.returncode

    @dbus.service.method('edu.ydisk.Service.Methods')
    def sync(self, sync_dir, mode):
        path = sync_dir[7:]
        self.config.include(path)
        if mode == 'store':
            logging.info("Directory for synchronization: %s\n" % path)
            self.exec_command(self.cmdlines[mode] % (path, self.config.default_dir))
            self.notify(self.notify_messages[mode] % path)
        else:
            self.notify(self.notify_messages[mode] % path)
            self.do_restart(mode)
            logging.info("Synchronization in %s mode: %s\n" % (mode, path))
            result = self.exec_command(self.cmdlines[mode] + self.config.default_dir)
            if not result:
                self.notify(self.notify_messages['success'] % path)
            else:
                self.notify(self.notify_messages['error'] % '')


    @dbus.service.method('edu.ydisk.Service.Methods')
    def unsync(self, unsync_dir, mode):
        path = unsync_dir[7:]
        self.config.exclude(path)
        logging.info(self.notify_messages[mode] % path + '\n')
        self.notify(self.notify_messages[mode] % path)

    @dbus.service.method('edu.ydisk.Service.Methods')
    def get_default_dir(self):
        return self.config.default_dir

    def notify(self, message):
        pynotify.Notification("Yandex-disk",
                              message,
                              self.path + "Ydisk_UFO.png").show()

    def run(self):
        gtk.threads_enter()
        gtk.main()
        gtk.threads_leave()

    def open_folder(self, widget):
        self.exec_command(self.cmdlines["open_folder"] + self.config.default_dir)

    def restart(self, widget):
        self.yInd.set_status(appindicator.STATUS_ATTENTION)
        self.do_restart("default")
        self.notify(self.notify_messages['restart'])
        self.yInd.set_status(appindicator.STATUS_ACTIVE)

    def do_restart(self, mode):
        self.exec_command(self.cmdlines["stop"])
        self.exec_command(self.cmdlines["start_" + mode])
        self.config.read_config()

    def quit(self, widget):
        self.bus.close()
        self.delete_log()
        self.exec_command(self.cmdlines["stop"])
        gtk.main_quit()