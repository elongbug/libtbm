bin_PROGRAMS = ut

ut_SOURCES = \
	src/main_tests.cpp \
	src/ut_tbm_bufmgr.cpp \
	src/ut_tbm_bufmgr_backend.cpp \
	src/ut_tbm_surface.cpp \
	src/ut_tbm_surface_queue.cpp \
	src/ut_tbm_surface_internal.cpp \
	stubs/stdlib_stubs.cpp

ut_CXXFLAGS = \
	${CXXFLAGS} \
	@LIBTBM_CFLAGS@ \
	-I./stubs \
	-I./src \
	-I../src \
	-fpermissive

ut_LDFLAGS = \
	${LDFLAGS} \
	@LIBTBM_LIBS@ \
	-lgtest \
	-ldl \
	-pthread

check:
	./ut
