bin_PROGRAMS = zik2ctl

zik2ctl_SOURCES = zik2ctl.c \
		  bluetooth-client.c \
		  zikprofile.c \
		  zikmessage.c \
		  zik.c \
		  zikconnection.c \
		  zikinfo.c \
		  zik2/zik2.c \
		  zik2/zik2profile.c \
		  zik3/zik3.c \
		  zik3/zik3profile.c

WARNING_FLAGS := -Wall -Wextra -Wno-unused-parameter

zik2ctl_CFLAGS = $(GLIB_CFLAGS) $(GIO_CFLAGS) $(GIO_UNIX_CFLAGS) $(WARNING_FLAGS) $(CFLAGS)
zik2ctl_LDADD = $(GLIB_LIBS) $(GIO_LIBS) $(GIO_UNIX_LIBS) $(LIBS)

BUILT_SOURCES = \
	bluetooth-client.h \
	bluetooth-client.c

CLEANFILES = $(BUILT_SOURCES)

bluetooth-client.c: bluetooth-client.h

bluetooth-client.c bluetooth-client.h: bluetooth-client.xml
	$(AM_V_GEN) $(GDBUS_CODEGEN) --interface-prefix=org.bluez --c-namespace=Bluetooth --generate-c-code=bluetooth-client --c-generate-object-manager $<
