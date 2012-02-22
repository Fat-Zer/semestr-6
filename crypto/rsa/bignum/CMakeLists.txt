add_library (bignum bignum.c bignum_print.c bignum_arithmetic.c)

# if(CMAKE_BUILD_TYPE EQUAL "Debug")
	add_executable(bignum-debug bignum-debug.c)
	target_link_libraries(bignum-debug bignum)
# endif()

