cmake_minimum_required(VERSION 2.8)

PROJECT( prog_protect )
find_package( PkgConfig REQUIRED )
pkg_check_modules( GLIB2 REQUIRED glib-2.0>=2.26 )
pkg_check_modules( GIO REQUIRED gio-2.0 )

include_directories( 
		${GIO_INCLUDE_DIRS}
		${GLIB2_INCLUDE_DIRS} )
link_directories( 
		${GIO_LIBRARY_DIRS} 
		${GLIB2_LIBRARY_DIRS} )

ADD_EXECUTABLE( prog prog.c udisks-interface.c )
target_link_libraries( prog 
		${GIO_LIBRARIES} 
		${GLIB2_LIBRARIES})

ADD_EXECUTABLE( installer installer.c udisks-interface.c )
target_link_libraries( installer
		${GIO_LIBRARIES} 
		${GLIB2_LIBRARIES})

