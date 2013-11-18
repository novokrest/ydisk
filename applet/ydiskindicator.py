import gobject
import gtk
import appindicator
import threading


import dbus
import dbus.service

class YdiskTray(threading.Thread, dbus.service.Object):

    __path = "/home/novokrest/"
    __mainIcon = __path + "Ydisk_browser.png"
    __messageIcon = __path + "Ydisk_message3.png"
    __attentionIcon = __path + "Ydisk_UFO.png"

    __syncIcon = __path + "Ydisk_sync.png"
    __quitIcon = __path + "Ydisk_quit.png"


    __yInd = appindicator.Indicator("YdiskIndicator", "ydiskindicator", appindicator.CATEGORY_APPLICATION_STATUS)

    def __init__(self):

        threading.Thread.__init__(self)

        self.__yInd.set_icon(self.__mainIcon)
        self.__yInd.set_status(appindicator.STATUS_ACTIVE)
        self.__yInd.set_attention_icon(self.__attentionIcon)

        self.__menu = gtk.Menu()

        syncIcon = gtk.Image()
        syncIcon.set_from_file(self.__syncIcon)
        syncIcon.show()

        self.syncIconItem = gtk.ImageMenuItem("Synchronize")
        self.syncIconItem.set_image(syncIcon)
        self.syncIconItem.set_always_show_image(True)
        self.syncIconItem.connect("activate", self.__Syncronization)
        self.syncIconItem.show()
        self.__menu.append(self.syncIconItem)

        quitIcon = gtk.Image()
        quitIcon.set_from_file(self.__quitIcon)
        quitIcon.show()

        self.quitIconItem = gtk.ImageMenuItem("Quit")
        self.quitIconItem.set_image(quitIcon)
        self.quitIconItem.set_always_show_image(True)
        self.quitIconItem.connect("activate", gtk.main_quit)
        self.quitIconItem.show()
        self.__menu.append(self.quitIconItem)

        self.__yInd.set_menu(self.__menu)

        busName = dbus.service.BusName('com.ydisk.Service', bus=dbus.SessionBus())
        dbus.service.Object.__init__(self, busName, '/YdiskObject')

    @dbus.service.method('com.ydisk.Service', in_signature='s', out_signature='s')
    def RecieveMessage(self, message):
        self.__yInd.set_icon(self.__messageIcon)
        return "This message was recieved: %s" % message


    def __Syncronization(self, widget):
        self.syncIconItem.set_sensitive(False)
        self.__yInd.set_status(appindicator.STATUS_ATTENTION)
        gobject.timeout_add(3000, self.__OutSynchronization)

    def __OutSynchronization(self):
        self.__yInd.set_status(appindicator.STATUS_ACTIVE)
        self.__yInd.set_icon(self.__mainIcon)
        self.syncIconItem.set_sensitive(True)
        return False

    def run(self):
        gtk.threads_enter()
        gtk.main()
        gtk.threads_leave()