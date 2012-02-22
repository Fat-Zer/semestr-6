cmake_minimum_required(VERSION 2.8)

PROJECT( prog_protect )
find_package( PkgConfig REQUIRED )
pkg_check_modules( DBUS REQUIRED dbus-1 )
pkg_check_modules( DBUS_GLIB REQUIRED dbus-glib-1 )
pkg_check_modules( GLIB2 REQUIRED glib-2.0 )

include_directories( 
		${DBUS_INCLUDE_DIRS} 
		${DBUS_INCLUDE_DIRS} 
		${GLIB2_INCLUDE_DIRS} )
link_directories( 
		${DBUS_LIBRARY_DIRS} 
		${DBUS_GLIB_LIBRARY_DIRS} 
		${GLIB2_LIBRARY_DIRS} )

ADD_EXECUTABLE( prog prog.c )
target_link_libraries( prog 
		${DBUS_LIBRARIES} 
		${DBUS_GLIB_LIBRARIES} 
		${GLIB2_LIBRARIES})

