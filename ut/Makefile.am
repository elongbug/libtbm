bin_PROGRAMS = tbm_utests

tbm_utests_SOURCES = \
	src/main_tests.cpp \
	src/ut_tbm_bufmgr.cpp \
	src/ut_tbm_bufmgr_backend.cpp \
	src/ut_tbm_surface.cpp \
	src/ut_tbm_surface_queue.cpp \
	src/ut_tbm_surface_internal.cpp \
	stubs/stdlib_stubs.cpp

tbm_utests_CXXFLAGS = \
	${CXXFLAGS} \
	@LIBTBM_CFLAGS@ \
	-I./stubs \
	-I./src \
	-I../src \
	-I./gtest/googletest/include \
	-fpermissive \
	-w

tbm_utests_LDADD = \
	gtest/googletest/lib/.libs/libgtest.a

tbm_utests_LDFLAGS = \
	${LDFLAGS} \
	@LIBTBM_LIBS@ \
	-ldl \
	-pthread

check:
	./tbm_utests
