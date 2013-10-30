

#include <libnautilus-extension/nautilus-extension-types.h>
#include <gtk/gtk.h>
#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-extension/nautilus-file-info.h>
#include <libnautilus-extension/nautilus-info-provider.h>

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



static GList * ydisk_nautilus_get_menu_items (NautilusMenuProvider * provider,
	GtkWidget * window, 
	GList * files)
{
	NautilusMenuItem *root_item, *item_first, *item_second;
	GList *items = NULL;
	root_item = nautilus_menu_item_new ("ydisk", "Ydisk", "root", "ydisk");
	NautilusMenu *sub_menu = nautilus_menu_new ();

	//g_warning("before %x, %x", (size_t)root_item, (size_t)sub_menu);
	nautilus_menu_item_set_submenu (root_item, sub_menu);

	//g_warning("after");

	item_first = nautilus_menu_item_new ("ydisk1", "_Synchronize", "sync", "ydisk");
	item_second = nautilus_menu_item_new ("ydisk2", "_Unsynchronize", "unsync", "ydisk");
	nautilus_menu_append_item (sub_menu, item_first);
	nautilus_menu_append_item (sub_menu, item_second);
	
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
