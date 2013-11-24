
#include <gtk/gtk.h>
#include <libnautilus-extension/nautilus-extension-types.h>
#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-extension/nautilus-file-info.h>
#include <libnautilus-extension/nautilus-info-provider.h>

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
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
//dbus
} YdiskExtension;

static ydisk_extension_class_init (YdiskExtensionClass *class);
static ydisk_extension_class_init (YdiskExtensionClass *class) {
//finalize
}

static ydisk_extension_instance_init (YdiskExtensionClass *inst);
static ydisk_extension_instance_init (YdiskExtensionClass *inst) {
//dbus
}

/*Types*/
static GType ydisk_extension_type;
static GType provider_types[1];

GType ydisk_extension_get_type (void)
{
	return ydisk_extension_type;
}

/*Interface*/

static void dbus_test () {
	
	DBusGConnection * connection;
	GError *error;
	DBusGProxy  *proxy;
	char **name_list;
	char **name_list_ptr;

	g_type_init ();

	error = NULL;
	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	if (connection == NULL)
	{
		g_printerr ("Failed to open connection to bus: %s\n", error->message);
		g_error_free (error);
		exit (1);
	}

	proxy = dbus_g_proxy_new_for_name (connection,
					DBUS_SERVICE_DBUS,
					DBUS_PATH_DBUS,
					DBUS_INTERFACE_DBUS);
	error = NULL;
	if (!dbus_g_proxy_call (proxy, "ListNames", &error, G_TYPE_INVALID, G_TYPE_STRV, &name_list,
											 G_TYPE_INVALID))
	{
		if (error->domain == DBUS_GERROR && error->code == DBUS_GERROR_REMOTE_EXCEPTION)
			g_printerr ("Caught remote method exception %s: %s",
					dbus_g_error_get_name (error),
					error->message);
		else
			g_printerr ("Error %s:\n", error->message);
		g_error_free (error);
		exit (1);
	}
	g_print ("Names on the message bus:\n");

	for (name_list_ptr = name_list; *name_list_ptr; name_list_ptr++) {
		g_print ("  %s\n", *name_list_ptr); }
	g_strfreev (name_list);
	g_object_unref (proxy);
}

static void do_sync_method (NautilusMenuItem *item, gpointer user_data)
{
	GList *files;
	GList *l;

	files = g_object_get_data ((GObject*)item, "foo_extension_files");
	
	for (l = files; l != NULL; l = l->next) {
		NautilusFileInfo *file = NAUTILUS_FILE_INFO (l->data);
		char *name;
		name = nautilus_file_info_get_uri (file);	
		g_print("File for synchronized: %s\n", name);
		dbus_test ();
		g_free(name);
	}

	
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
	g_object_set_data_full ((GObject*)item_sync, "foo_extension_files", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

	item_unsync = nautilus_menu_item_new ("unsync", "_Unsynchronize", "unsync", "ydisk");

	nautilus_menu_append_item (sub_menu, item_sync);
	nautilus_menu_append_item (sub_menu, item_unsync);
	
	if (root_item != NULL)
		items = g_list_append (items, root_item);

	return items;
}


static void ydisk_nautilus_menu_provider_iface_init (NautilusMenuProviderIface *iface)
{
	iface->get_file_items = ydisk_nautilus_get_menu_items;
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
