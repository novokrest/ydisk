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
	DBusGProxy  *ydisk_proxy;
	GError *error;
	DBusGProxyCall *check_proxy;
	char * default_dir;
	
} YdiskExtension;


/*Declarations*/

static ydisk_extension_class_init (YdiskExtensionClass *class);
static ydisk_extension_instance_init (YdiskExtension *inst);

static GType provider_types[1];
static GType ydisk_extension_type;

static void ydisk_extension_finalize (GObject *object);

static bool check_directory (GList * files);
static bool check_service (gpointer provider, char * service_name);
static void synchronize (NautilusMenuItem *item, gpointer user_data);
static void synchronize_readonly (NautilusMenuItem *item, gpointer user_data);
static void synchronize_overwrite (NautilusMenuItem *item, gpointer user_data);
static GList *  create_yandexdisk_menu (NautilusMenuProvider * provider, GtkWidget * window, GList * files);
static GList *  create_regular_menu (NautilusMenuProvider * provider, GtkWidget * window, GList * files);
static void create_sygnal (NautilusMenuItem *item, YdiskExtension * yde, char * service_method, char * data_files, char * mode);
static void send_signal (YdiskExtension * yde, char * dir_name, char * service_method, char * mode);

/*Definitions*/

static ydisk_extension_class_init (YdiskExtensionClass *class) {

	G_OBJECT_CLASS(class)->finalize = ydisk_extension_finalize;
}


static void default_dir_callback (DBusGProxy *proxy,
                                  DBusGProxyCall *call_id,
                                  void *user_data)
{
	YdiskExtension * yde = (YdiskExtension*)user_data;

    if (yde->default_dir != NULL)
	{
		g_free (yde->default_dir);
	}

	if (yde->error != NULL)
	{
		g_error_free (yde->error);
	}

	yde->default_dir = NULL;
	yde->error = NULL;
	
	if (!dbus_g_proxy_end_call (proxy, call_id, &yde->error, G_TYPE_STRING, &yde->default_dir, G_TYPE_INVALID)) {}
}


static ydisk_extension_instance_init (YdiskExtension *inst) {
	
	inst->error = NULL;
	inst->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &inst->error);
	inst->ydisk_proxy = dbus_g_proxy_new_for_name (inst->connection,
					"edu.ydisk.Service",
					"/edu/ydisk/YdiskObject",
					"edu.ydisk.Service.Methods");

	inst->default_dir = NULL;	
	inst->check_proxy = dbus_g_proxy_begin_call (inst->ydisk_proxy, "get_default_dir", &default_dir_callback, inst, NULL, G_TYPE_INVALID );
	
}


/*Types*/

GType ydisk_extension_get_type (void)
{
	return ydisk_extension_type;
}


/*Interface*/

static void send_signal (YdiskExtension * yde, char * dir_name, char * service_method, char * mode) 
{
	if (yde->connection == NULL)
	{
		g_printerr ("Failed to open connection to bus: %s\n", yde->error->message);
		g_error_free (yde->error);
		exit (1);
	}
	
	yde->error = NULL;
	
	dbus_g_proxy_call_no_reply (yde->ydisk_proxy, service_method, G_TYPE_STRING, dir_name, G_TYPE_STRING, mode, G_TYPE_INVALID);
}


static void create_sygnal (NautilusMenuItem *item, YdiskExtension * yde, char * service_method, char * data_files, char * mode)
{
	GList * files = g_object_get_data ((GObject*)item, data_files);
	GList * l = files;
	
	while (l != NULL) {

		NautilusFileInfo *file = NAUTILUS_FILE_INFO (l->data);
		char * dir_name;
		dir_name = nautilus_file_info_get_uri (file);
	
		send_signal (yde, dir_name, service_method, mode);
		g_free(dir_name);

		l = l->next;
	}

}  


static void synchronize (NautilusMenuItem *item, gpointer user_data)
{
	YdiskExtension * yde = YDISK_EXTENSION (user_data);
	create_sygnal (item, yde, "sync", "dirs", "default");
}


static void synchronize_readonly (NautilusMenuItem *item, gpointer user_data)
{
	YdiskExtension * yde = YDISK_EXTENSION (user_data);
	create_sygnal (item, yde, "sync", "dirs", "readonly");
}


static void synchronize_overwrite (NautilusMenuItem *item, gpointer user_data)
{
	YdiskExtension * yde = YDISK_EXTENSION (user_data);
	create_sygnal (item, yde, "sync", "dirs", "overwrite");
}


static void store (NautilusMenuItem *item, gpointer user_data)
{
	YdiskExtension * yde = YDISK_EXTENSION (user_data);
	create_sygnal (item, yde, "sync", "dirs", "store");
}


static void unsynchronize (NautilusMenuItem *item, gpointer user_data)
{
	YdiskExtension * yde = YDISK_EXTENSION (user_data);
	create_sygnal (item, yde, "unsync", "dirs", "unsync");
}


static bool check_yandexdisk (GList * files, YdiskExtension * yde)
{
	char * dirname;
	bool is_dir;
	if (files != NULL)
	{
		GList * l = nautilus_file_info_list_copy (files);
		NautilusFileInfo * file = NAUTILUS_FILE_INFO (l->data);
		dirname = nautilus_file_info_get_uri (file);
		is_dir = nautilus_file_info_is_directory (file);
		if (!strcmp(dirname + 7, yde->default_dir) && (is_dir) && (l->next == NULL)) 
		{ 
			return true; 
		}
		if (dirname) 
		{ 
			g_free (dirname); 
		}
	}
	return false;
}



static GList * ydisk_nautilus_get_menu_items (NautilusMenuProvider * provider,
							GtkWidget * window, 
							GList * files)
{
	GList *items = NULL;
	YdiskExtension * yde = YDISK_EXTENSION (provider);
	
	if (yde->default_dir)
	{	
		if (check_yandexdisk(files, yde))
		{
			items = create_yandexdisk_menu (provider, window, files);
		}
		else
		{
			items = create_regular_menu (provider, window, files);
		}
	}
	
	yde->check_proxy = dbus_g_proxy_begin_call (yde->ydisk_proxy, 
												"get_default_dir", 
												&default_dir_callback, yde, NULL, G_TYPE_INVALID );
	return items;
}


static GList *  create_yandexdisk_menu (NautilusMenuProvider * provider,
							        GtkWidget * window, 
							        GList * files)
{
		GList *items = NULL;
		NautilusMenuItem *root_item;
		NautilusMenuItem *item_sync, *item_sync_ro, *item_sync_ow;

		root_item = nautilus_menu_item_new ("item_root", "_Yandex Disk", "root", "None");
		NautilusMenu *sub_menu = nautilus_menu_new ();
		nautilus_menu_item_set_submenu (root_item, sub_menu);

		item_sync = nautilus_menu_item_new ("item_sync", "_Synchronize", "sync", "None");
		g_signal_connect (item_sync, "activate", G_CALLBACK(synchronize), provider);
		g_object_set_data_full ((GObject*)item_sync, "dirs", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

		item_sync_ro = nautilus_menu_item_new ("item_sync_ro", "_Synchronize read-only", "sync_ro", "None");
		g_signal_connect (item_sync_ro, "activate", G_CALLBACK(synchronize_readonly), provider);
		g_object_set_data_full ((GObject*)item_sync_ro, "dirs", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

		item_sync_ow = nautilus_menu_item_new ("item_sync_ow", "_Synchronize overwrite", "sync_ow", "None");
		g_signal_connect (item_sync_ow, "activate", G_CALLBACK(synchronize_overwrite), provider);
		g_object_set_data_full ((GObject*)item_sync_ow, "dirs", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);	

		nautilus_menu_append_item (sub_menu, item_sync);
		nautilus_menu_append_item (sub_menu, item_sync_ro);
		nautilus_menu_append_item (sub_menu, item_sync_ow);
	
		if (root_item != NULL) {
			items = g_list_append (items, root_item);
		}
		
		return items;
}


static GList *  create_regular_menu (NautilusMenuProvider * provider,
							     GtkWidget * window, 
							     GList * files)
{
		GList *items = NULL;
		NautilusMenuItem *root_item;
		NautilusMenuItem *item_store, *item_unsync;

		root_item = nautilus_menu_item_new ("item_root", "_Yandex Disk", "root", "None");

		NautilusMenu *sub_menu = nautilus_menu_new ();
		nautilus_menu_item_set_submenu (root_item, sub_menu);

		item_store = nautilus_menu_item_new ("item_add", "_Store", "sync", "None");
		g_signal_connect (item_store, "activate", G_CALLBACK(store), provider);
		g_object_set_data_full ((GObject*)item_store, "dirs", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

		item_unsync = nautilus_menu_item_new ("item_unsync", "_Unsynchronize", "sync_ro", "None");
		g_signal_connect (item_unsync, "activate", G_CALLBACK(unsynchronize), provider);
		g_object_set_data_full ((GObject*)item_unsync, "dirs", 
							nautilus_file_info_list_copy (files),
							(GDestroyNotify)nautilus_file_info_list_free);

		nautilus_menu_append_item (sub_menu, item_store);
		nautilus_menu_append_item (sub_menu, item_unsync);
	
		if (root_item != NULL) {
			items = g_list_append (items, root_item);
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
	ydisk_extension_register_type (module);
	provider_types[0] = ydisk_extension_get_type ();
}


static void ydisk_extension_finalize (GObject *object)
{
	YdiskExtension * yde = YDISK_EXTENSION(object);

	if (yde->ydisk_proxy != NULL)
		g_object_unref (yde->ydisk_proxy);

	if (yde->check_proxy != NULL)
		g_object_unref (yde->check_proxy);

	if (yde->connection)
		g_object_unref (yde->connection);

	if (yde->error)
		g_error_free (yde->error);

	if (yde->default_dir)
		g_free(yde->default_dir);
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
