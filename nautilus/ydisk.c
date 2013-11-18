
#include <gtk/gtk.h>
#include <libnautilus-extension/nautilus-extension-types.h>
#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-extension/nautilus-file-info.h>
#include <libnautilus-extension/nautilus-info-provider.h>

#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


/*Extension*/
typedef struct
{
	GObjectClass parent_slot;
} YdiskExtensionClass;

typedef struct
{
	GObject parent_slot;
} YdiskExtension;

static ydisk_extension_class_init (YdiskExtensionClass *class);
static ydisk_extension_class_init (YdiskExtensionClass *class) {}

static ydisk_extension_instance_init (YdiskExtensionClass *inst);
static ydisk_extension_instance_init (YdiskExtensionClass *inst) {}

/*Types*/
static GType ydisk_extension_type;
static GType provider_types[1];

GType ydisk_extension_get_type (void)
{
	return ydisk_extension_type;
}

/*Interface*/

void sendsignal(char* sigvalue) 
{
	DBusMessage* msg;
	DBusMessageIter* args;
	DBusConnection* conn;
	DBusError err;
	int ret;
	dbus_uint32_t serial = 0;

	printf("Sending signal with value %s\n", sigvalue);

	dbus_error_init (&err);

	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if (dbus_error_is_set(&err)) {
		fprintf(stderr, "Connection Error (%s)\n", err.message);
		dbus_error_free(&err);
	}
	if (NULL == conn) {
		exit(1);	
	}

	ret = dbus_bus_request_name(conn, "test.signal.source", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
	if (dbus_error_is_set(&err)) {
      fprintf(stderr, "Name Error (%s)\n", err.message);
      dbus_error_free(&err);
	}
	if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
      exit(1);
	}

	msg = dbus_message_new_signal("/test/signal/Object", // object name of the signal
                                 "test.signal.Type", // interface name of the signal
                                 "Test"); // name of the signal
	if (NULL == msg)
	{
		fprintf(stderr, "Message Null\n");
		exit(1);
	}
	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &sigvalue)) {
		fprintf(stderr, "Out Of Memory!\n");
		exit(1);
	}

	if (!dbus_connection_send(conn, msg, &serial)) {
		fprintf(stderr, "Out Of Memory!\n");
		exit(1);
	}
	dbus_connection_flush(conn);

	printf("Signal Sent\n");

   // free the message
	dbus_message_unref(msg);
}


static void do_sync_method (NautilusMenuItem *item, gpointer user_data)
{
	GList *files;
	GList *l;

	files = g_object_get_data (item, "foo_extension_files");
	
	for (l = files; l != NULL; l = l->next) {
		NautilusFileInfo *file = NAUTILUS_FILE_INFO (l->data);
		char *name;
		name = nautilus_file_info_get_uri (file);	
		g_print("File for synchronized: %s\n", name);
		g_free(name);
	}

	sendsignal("Sync");
} 


static GList * ydisk_nautilus_get_menu_items (NautilusMenuProvider * provider,
							GtkWidget * window, 
							GList * files)
{	

	NautilusMenuItem *root_item, *item_sync, *item_unsync;
	GList *items = NULL;
	root_item = nautilus_menu_item_new ("ydisk", "Ydisk", "root", "ydisk");
	NautilusMenu *sub_menu = nautilus_menu_new ();

	//g_warning("before %x, %x", (size_t)root_item, (size_t)sub_menu);
	nautilus_menu_item_set_submenu (root_item, sub_menu);

	//g_warning("after");

	item_sync = nautilus_menu_item_new ("sync", "_Synchronize", "sync", "ydisk");
	g_signal_connect (item_sync, "activate", G_CALLBACK(do_sync_method), provider);
	g_object_set_data_full (item_sync, "foo_extension_files", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

	item_unsync = nautilus_menu_item_new ("unsync", "_Unsynchronize", "unsync", "ydisk");
	nautilus_menu_append_item (sub_menu, item_sync);
	nautilus_menu_append_item (sub_menu, item_unsync);
	
	if (root_item != NULL)
		items = g_list_append (items, root_item);
	//items = g_list_prepend (items, item_first);
	//items = g_list_append (items, item_second);

	return items;
}

static GList *
ydisk_nautilus_get_bg_menu_items (NautilusMenuProvider * provider,
				      GtkWidget * window,
				      NautilusFileInfo * folder)
{
	GList * files = NULL;
	GList * items = NULL;

	files = g_list_prepend (files, folder);
	items = ydisk_nautilus_get_menu_items (provider, window, files);
	files = g_list_remove (files, folder);
	g_list_free (files);

	return items;
}

static void ydisk_nautilus_menu_provider_iface_init (NautilusMenuProviderIface *iface)
{
	iface->get_file_items = ydisk_nautilus_get_menu_items;
	//iface->get_background_items = ydisk_nautilus_get_bg_menu_items;
	return;
}

static void ydisk_extension_register_type (GTypeModule *module)
{
	static const GTypeInfo info = {
		sizeof(YdiskExtensionClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) ydisk_extension_class_init,
		NULL,
		NULL,
		sizeof(YdiskExtension),
		0,
		(GInstanceInitFunc) ydisk_extension_instance_init,
		};

	ydisk_extension_type = g_type_module_register_type (module, G_TYPE_OBJECT, "YdiskExtension", &info, 0);

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) ydisk_nautilus_menu_provider_iface_init,
		NULL,
		NULL
		};

	g_type_module_add_interface (module, 
		ydisk_extension_type, 
		NAUTILUS_TYPE_MENU_PROVIDER, 
		&menu_provider_iface_info);
}


/*Extension initialization*/
void nautilus_module_initialize (GTypeModule *module)
{

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	ydisk_extension_register_type (module);
	provider_types[0] = ydisk_extension_get_type ();
}

void nautilus_module_shutdown (void)
{
	/*No specific shutdown*/
}

void nautilus_module_list_types (const GType** types, int* num_types)
{
	*types = provider_types;
	*num_types = G_N_ELEMENTS (provider_types);
}
