__author__ = 'novokrest'

import ydiskindicator


from dbus.mainloop.glib import DBusGMainLoop
DBusGMainLoop(set_as_default=True)

Yindicator = ydiskindicator.YdiskTray()
Yindicator.start()


