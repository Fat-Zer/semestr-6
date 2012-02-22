#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus-glib.h>

#define DBUS_INTERFACE_UDISKS "org.freedesktop.UDisks"
#define DBUS_INTERFACE_UDISKS_DEVICE "org.freedesktop.UDisks.Device"
#define DBUS_INTERFACE_DBUS_PROPERTIES "org.freedesktop.DBus.Properties"
#define DBUS_INTERFACE_CK "org.freedesktop.ConsoleKit"
#define DBUS_INTERFACE_CK_MANAGER "org.freedesktop.ConsoleKit.Manager"
#define DBUS_INTERFACE_CK_SEAT "org.freedesktop.ConsoleKit.Seat"
#define DBUS_INTERFACE_CK_SESSION "org.freedesktop.ConsoleKit.Session"

#define DBUS_COMMON_NAME_UDISKS "org.freedesktop.UDisks"
#define DBUS_COMMON_NAME_CK "org.freedesktop.ConsoleKit"

#define DBUS_OBJECT_PATH_UDISKS_ROOT "/org/freedesktop/UDisks"
#define DBUS_OBJECT_PATH_CK_ROOT "/org/freedesktop/ConsoleKit"

gboolean
org_freedesktop_UDisks_enumerate_devices (DBusGProxy *proxy, GPtrArray** OUT_devices, GError **error)

{
  return dbus_g_proxy_call (proxy, "EnumerateDevices", error, G_TYPE_INVALID, dbus_g_type_get_collection ("GPtrArray", DBUS_TYPE_G_OBJECT_PATH), OUT_devices, G_TYPE_INVALID);
}



int main (int argc, char **argv)
{
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	char **name_list;
	char **name_list_ptr;
	GPtrArray *device_paths; ;
	int i;

	g_type_init ();

	error = NULL;
	connection = dbus_g_bus_get (DBUS_BUS_SYSTEM, &error);
	if(connection == NULL) {
		g_printerr ("Failed to open dBus connection: %s\n",
				error->message);
		g_error_free (error);
		exit (1);
	}

	/* Create a proxy object for the "bus driver" (name "org.freedesktop.DBus") */

	proxy = dbus_g_proxy_new_for_name (connection,
			"org.freedesktop.UDisks", 
			"/org/freedesktop/UDisks",
			"org.freedesktop.UDisks");

	/* Call ListNames method, wait for reply */
	error = NULL;
	if (! org_freedesktop_UDisks_enumerate_devices(proxy, &device_paths, &error))
	{
		/* Just do demonstrate remote exceptions versus regular GError */
		if (error->domain == DBUS_GERROR && error->code == DBUS_GERROR_REMOTE_EXCEPTION)
			g_printerr ("Caught remote method exception %s: %s",
					dbus_g_error_get_name (error),
					error->message);
		else
			g_printerr ("Error: %s\n", error->message);
		g_error_free (error);
		exit (1);
	}

	for (i = 0; i < device_paths->len; i++)
        {
          char *object_path = device_paths->pdata[n];
          do_show_info (object_path);
          g_print ("\n"
                   "========================================================================\n");
        }

	/* Print the results */
/*
	g_print ("Names on the message bus:\n");

	for (name_list_ptr = name_list; *name_list_ptr; name_list_ptr++)
	{
		g_print ("  %s\n", *name_list_ptr);
	}
	g_strfreev (name_list);

	g_object_unref (proxy);
*/
	return 0;
}
