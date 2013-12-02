#define YDISK_EXTENSION_TYPE  (ydisk_extension_get_type ())
#define YDISK_EXTENSION(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), YDISK_EXTENSION_TYPE, YdiskExtension))

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


typedef struct _YdiskExtension
{
	GObject parent_slot;

	DBusGConnection * connection;
	DBusGProxy  *proxy;
	GError *error;
	
} YdiskExtension;


/*Declarations*/

static ydisk_extension_class_init (YdiskExtensionClass *class);
static ydisk_extension_instance_init (YdiskExtension *inst);

static GType provider_types[1];
static GType ydisk_extension_type;

static void ydisk_extension_finalize (GObject *object);

static bool ChooseDirectory (GList * files);
static void Do_synchronize (NautilusMenuItem *item, gpointer user_data);
static void Do_unsynchronize (NautilusMenuItem *item, gpointer user_data);
static void Sync_Unsync (NautilusMenuItem *item, YdiskExtension * yde, char item_id);
static void Send_signal (YdiskExtension * yde, char * dir_name, char * service_method);

/*Definitions*/

static ydisk_extension_class_init (YdiskExtensionClass *class) {

	G_OBJECT_CLASS(class)->finalize = ydisk_extension_finalize;
}


static ydisk_extension_instance_init (YdiskExtension *inst) {
	
	inst->error = NULL;
	inst->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &inst->error);
	inst->proxy = dbus_g_proxy_new_for_name (inst->connection,
					"edu.ydisk.Service",
					"/edu/ydisk/YdiskObject",
					"edu.ydisk.Service.Methods");
}


/*Types*/

GType ydisk_extension_get_type (void)
{
	return ydisk_extension_type;
}


/*Interface*/

static void Send_signal (YdiskExtension * yde, char * dir_name, char * service_method) 
{
	g_warning("Send_signal");	

	if (yde->connection == NULL)
	{
		
		g_printerr ("Failed to open connection to bus: %s\n", yde->error->message);
		g_error_free (yde->error);
		exit (1);
	}
	
	if (yde->proxy != NULL)
	{
		g_warning("NOT NULL PROXY");
	}
	
	yde->error = NULL;
	
	dbus_g_proxy_call_no_reply (yde->proxy, service_method, G_TYPE_STRING, dir_name, G_TYPE_INVALID);

	/*if (!dbus_g_proxy_call (yde->proxy, service_method, &yde->error, G_TYPE_STRING, dir_name, G_TYPE_INVALID, G_TYPE_INVALID))
	{
		g_warning("ERROR_DBUS_PROXY_CALL");
		if (yde->error->domain == DBUS_GERROR && yde->error->code == DBUS_GERROR_REMOTE_EXCEPTION)
			g_printerr ("Caught remote method exception %s: %s",
					dbus_g_error_get_name (yde->error),
					yde->error->message);
		else
			g_printerr ("Error %s:\n", yde->error->message);

		g_error_free (yde->error);
		exit (1);
	}*/
	
	g_warning("Out_of_Send_signal");
}


static void Sync_Unsync (NautilusMenuItem *item, YdiskExtension * yde, char item_id)
{
	g_warning("Sync_Unsync");

	char * service_method;
	char * data_files;

	switch (item_id)
	{
		case 's':
			g_warning("Synchronize");
			service_method = "SyncMethod";
			data_files = "sync_dirs";
			break;

		case 'u':	
			g_warning("Unsynchronize");
			service_method = "UnsyncMethod";
			data_files = "unsync_dirs";
			break;

		case 'd':
			g_warning("Set_default_dir");
			service_method = "SetDefaultDirMethod";
			data_files = "default_dir";
			break;
	}
	

	GList * files = g_object_get_data ((GObject*)item, data_files);
	GList * l = files;
	
	while (l != NULL) {

		NautilusFileInfo *file = NAUTILUS_FILE_INFO (l->data);
		char * dir_name;
		dir_name = nautilus_file_info_get_uri (file);
	
		g_print("Files for %s: %s\n",service_method, dir_name);
		Send_signal (yde, dir_name, service_method);
		g_free(dir_name);

		l = l->next;
	}
	

	//g_free (data_files);
	//g_free (service_method);

}  


static void Do_synchronize (NautilusMenuItem *item, gpointer user_data)
{
	g_warning("Do_synchronize");

	YdiskExtension * yde = YDISK_EXTENSION (user_data);
	Sync_Unsync (item, yde, 's');
}


static void Do_unsynchronize (NautilusMenuItem *item, gpointer user_data)
{
	g_warning("Do_unsynchronize");

	YdiskExtension * yde = YDISK_EXTENSION (user_data);
	Sync_Unsync (item, yde, 'u');
}


static void Do_set_default_dir (NautilusMenuItem *item, gpointer user_data)
{
	g_warning("Do_set_default_dir");

	YdiskExtension * yde = YDISK_EXTENSION (user_data);
	Sync_Unsync (item, yde, 'd');
}


static bool ChooseDirectory (GList * files)
{
	g_warning("ChooseDirectory");

	if (files != NULL)
	{
		GList *l = nautilus_file_info_list_copy (files);
		for (; l != NULL; l = l->next)
		{
			NautilusFileInfo * file = NAUTILUS_FILE_INFO (l->data);
			gboolean is_dir = nautilus_file_info_is_directory (file);

			if (! is_dir) {return false;}
		}
		return true;
	}
	else
	{
		return false;
	}
}


static GList * ydisk_nautilus_get_menu_items (NautilusMenuProvider * provider,
							GtkWidget * window, 
							GList * files)
{
	g_warning("ydisk_nautilus_get_menu_items");

	GList *items = NULL;
	
	if (ChooseDirectory(files))
	{	
		g_warning("after ChooseDirectory");		

		NautilusMenuItem *root_item;
		NautilusMenuItem *item_sync, *item_unsync, *item_default;
		root_item = nautilus_menu_item_new ("ydisk", "Ydisk", "root", "ydisk");

		NautilusMenu *sub_menu = nautilus_menu_new ();
		nautilus_menu_item_set_submenu (root_item, sub_menu);

		item_sync = nautilus_menu_item_new ("sync_item", "_Synchronize", "sync", "ydisk");
		g_signal_connect (item_sync, "activate", G_CALLBACK(Do_synchronize), provider);
		g_object_set_data_full ((GObject*)item_sync, "sync_dirs", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

		item_unsync = nautilus_menu_item_new ("unsync_item", "_Unsynchronize", "unsync", "ydisk");
		g_signal_connect (item_unsync, "activate", G_CALLBACK(Do_unsynchronize), provider);
		g_object_set_data_full ((GObject*)item_unsync, "unsync_dirs", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

		item_default = nautilus_menu_item_new ("default_item", "_Set as local disk", "default", "ydisk");
		g_signal_connect (item_default, "activate", G_CALLBACK(Do_set_default_dir), provider);
		g_object_set_data_full ((GObject*)item_default, "default_dir", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

		nautilus_menu_append_item (sub_menu, item_sync);
		nautilus_menu_append_item (sub_menu, item_unsync);
		nautilus_menu_append_item (sub_menu, item_default);
	
		if (root_item != NULL) {
			items = g_list_append (items, root_item);
		}
	}

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


static void ydisk_extension_finalize (GObject *object)
{
	YdiskExtension * yde = YDISK_EXTENSION(object);

	if (yde->proxy != NULL)
		g_object_unref (yde->proxy);

	if (yde->connection)
		g_object_unref (yde->connection);

	if (yde->error)
		g_error_free (yde->error);
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
