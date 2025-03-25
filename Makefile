# This little makefile compiles a kernel32.dll replacement that DiscordMessenger can link against.

LIBNAME = kernec32

XL = -Xlinker
MJSSV = $(XL) --major-subsystem-version $(XL)
MNSSV = $(XL) --minor-subsystem-version $(XL)
MJOSV = $(XL) --major-os-version $(XL)
MNOSV = $(XL) --minor-os-version $(XL)

# Give it a subsystem version of 3.10 and an OS version of 1.0
SSYSVER = $(MJSSV) 3 $(MNSSV) 10 $(MJOSV) 1 $(MNOSV) 0

SOURCES = src/main.c src/forward.c src/hacks.c src/main.def
OBJECTS = $(patsubst src/%.c,bld/%.o,$(SOURCES))

.PHONY: clean

$(LIBNAME).dll:	$(OBJECTS)
	gcc -shared $^ -o $@ -O3 -ffreestanding -nodefaultlibs -nostartfiles -lkernel32 -luser32 -Wl,--enable-stdcall-fixup $(SSYSVER) -Wl,--out-implib,lib$(LIBNAME).a

bld/%.o: src/%.c
	mkdir -p $(dir $@)
	gcc -c $^ -o $@

bld/%.o: src/%.S
	mkdir -p $@
	as $^ -o $@ 

clean:
	rm -rf bld

