#include <libnautilus-extension/nautilus-menu-provider.h>

//вся суть в foo_extension_get_file_items

typedef struct
{
    GObject parent_slot;
} FooExtension;

typedef struct
{
    GObjectClass parent_slot;
} FooExtensionClass;

static void foo_extension_class_init    (FooExtensionClass *class);
static void foo_extension_instance_init (FooExtension      *img);

static GList *
foo_extension_get_file_items (NautilusMenuProvider *provider,
			      GtkWidget *window,
			      GList *files);

/* Interfaces */

static void
foo_extension_menu_provider_iface_init (NautilusMenuProviderIface *iface) {
  iface->get_file_items = foo_extension_get_file_items;
  return;
}

/* Extension */
static void foo_extension_class_init(FooExtensionClass *class)
{
}

static void foo_extension_instance_init(FooExtension *img)
{
}

static GType provider_types[1];

static GType foo_extension_type;

static void foo_extension_register_type(GTypeModule *module)
{
    static const GTypeInfo info = {
                sizeof(FooExtensionClass),
                (GBaseInitFunc) NULL,
                (GBaseFinalizeFunc) NULL,
                (GClassInitFunc) foo_extension_class_init,
                NULL,
                NULL,
                sizeof (FooExtension),
                0,
                (GInstanceInitFunc) foo_extension_instance_init,
                };

    static const GInterfaceInfo menu_provider_iface_info = {
        (GInterfaceInitFunc) foo_extension_menu_provider_iface_init,
        NULL,
        NULL
    };

    foo_extension_type = g_type_module_register_type(module,
                              G_TYPE_OBJECT,
                              "FooExtension",
                              &info, 0);


    g_type_module_add_interface (module,
                                 foo_extension_type,
                                 NAUTILUS_TYPE_MENU_PROVIDER,
                                 &menu_provider_iface_info);
}

GType foo_extension_get_type(void)
{
    return foo_extension_type;
}

/* Extension initialization */
void nautilus_module_initialize (GTypeModule  *module)
{
    foo_extension_register_type(module);
    provider_types[0] = foo_extension_get_type();
}

void nautilus_module_shutdown(void)
{
    /* Any module-specific shutdown */
}

void nautilus_module_list_types (const GType **types, int *num_types)
{
    *types = provider_types;
    *num_types = G_N_ELEMENTS (provider_types);
}

static GList *
foo_extension_get_file_items (NautilusMenuProvider *provider,
			      GtkWidget *window,
			      GList *files)
{
	NautilusMenuItem *item;
	GList *ret;

	item = nautilus_menu_item_new ("FooExtension::do_stuff",
				       "Do IMPORTANT FOO STUFF",
				       "Do important foo-related stuff to the selected files",
				       NULL);
	ret = g_list_append (0, item);
	return ret;
}
