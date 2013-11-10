__author__ = 'novokrest'

import dbus

bus = dbus.SessionBus()
ydiskservice = bus.get_object('com.ydisk.Service', '/YdiskObject')
ydiskmessage = ydiskservice.get_dbus_method('RecieveMessage', 'com.ydisk.Service')
print ydiskmessage('Hello')

#print ydiskservice.repeatmessage("Hello, Yandex Disk!", dbus_interface = 'com.ydisk.Service')
