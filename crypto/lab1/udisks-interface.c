#include <stdio.h>
#include <stdlib.h>
#include <gio/gio.h>
#include <glib/gprintf.h>

#define DBUS_INTERFACE_UDISKS "org.freedesktop.UDisks"
#define DBUS_INTERFACE_UDISKS_DEVICE "org.freedesktop.UDisks.Device"

#define DBUS_COMMON_NAME_UDISKS "org.freedesktop.UDisks"

#define DBUS_OBJECT_PATH_UDISKS_ROOT "/org/freedesktop/UDisks"

gboolean dbus_get_bool_property(GDBusProxy *proxy, gchar * property) {
	GVariant* var;
	gboolean rv;
	var = g_dbus_proxy_get_cached_property(proxy, property);
	g_variant_get(var, "b", &rv);
	g_variant_unref(var);
	return rv;
}

gchar* dbus_get_string_property(GDBusProxy *proxy, gchar * property) {
	GVariant* var;
	gchar* rv;
	var = g_dbus_proxy_get_cached_property(proxy, property);
	rv = g_variant_dup_string(var, 0);
	g_variant_unref(var);
	return rv;
}

GPtrArray* get_serial_array(GError **error) {
	
	GDBusProxy *udisks_proxy=0;
	GVariant *device_list;
	GPtrArray* rv=0;

	udisks_proxy = g_dbus_proxy_new_for_bus_sync (
				G_BUS_TYPE_SYSTEM, 
				G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS,
				NULL /* info */ , 
				DBUS_COMMON_NAME_UDISKS,
				DBUS_OBJECT_PATH_UDISKS_ROOT,  
				DBUS_INTERFACE_UDISKS,
				NULL /* cancellable */ , 
				error );

	if(udisks_proxy == NULL) {
		g_prefix_error (error, "Failed to create dBus proxy:");
		goto udisks_proxy_create_error;
	}

	// Call ListNames method, wait for reply 
	device_list = g_dbus_proxy_call_sync (
			udisks_proxy, 
			"EnumerateDevices", 
			NULL /* paramatrs */,
			G_DBUS_CALL_FLAGS_NONE, 
			-1 /* timeout_msec */,
        	NULL /* cancellable */,
			error);
	if(device_list == NULL) {
		g_prefix_error (error, "Failed to call EnumerateDevices udisks method:");
		goto enumerate_device_error;
	}
    
	//	g_print("%s: \n", );
	{
		GVariantIter *iter;
		gchar *device;

		g_variant_get (device_list, "(ao)", &iter);
		rv = g_ptr_array_new();
		g_ptr_array_set_free_func(rv, g_free);
		while (g_variant_iter_loop (iter, "o", &device)) {
			GDBusProxy *device_proxy = 0;
			
			device_proxy = g_dbus_proxy_new_for_bus_sync (
					G_BUS_TYPE_SYSTEM, 
					G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS,
					NULL /* info */ , 
					DBUS_COMMON_NAME_UDISKS,
					device,  
					DBUS_INTERFACE_UDISKS_DEVICE,
					NULL /* cancellable */ , 
					error );

			if(device_proxy == NULL) {
				g_prefix_error (error, "Failed to create dBus devices proxy for %s device:", device);
				g_ptr_array_unref(rv); rv=0;
				goto devices_proxy_create_error;
			}
			
			if( dbus_get_bool_property(device_proxy, "DeviceIsDrive") &&
				! dbus_get_bool_property(device_proxy, "DeviceIsRemovable") ) {
				gchar* full_serial=0;
				gchar *vendor   = dbus_get_string_property(device_proxy, "DriveVendor");
                gchar *model    = dbus_get_string_property(device_proxy, "DriveModel");
                gchar *revision = dbus_get_string_property(device_proxy, "DriveRevision");
                gchar *serial   = dbus_get_string_property(device_proxy, "DriveSerial");
                gchar *wwn      = dbus_get_string_property(device_proxy, "DriveWwn");

				full_serial = g_strdup_printf("%s-%s-%s-%s", model, revision, serial, wwn);
				g_ptr_array_add(rv, full_serial);

				g_free(vendor);
				g_free(model);
				g_free(revision);
				g_free(serial);
				g_free(wwn);
			}
			g_object_unref (device_proxy);
		}
devices_proxy_create_error:
		g_variant_iter_free(iter);
	}

	
	g_variant_unref(device_list);
enumerate_device_error:
	g_object_unref (udisks_proxy);
udisks_proxy_create_error:
	return rv;
}



