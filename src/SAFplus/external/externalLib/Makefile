
SAFPLUS_SDK_DIR ?=/opt/clovis/6.1
SAFPLUS_DIR ?=/code/git/SAFplus/src/SAFplus

all: libmw.so

prebuildExternal:
	mkdir -p prebuildExternal


libmw.so: prebuildExternal
	(cd prebuildExternal; $(SAFPLUS_DIR)/configure --with-sdk-dir=$(SAFPLUS_SDK_DIR))
	(cd prebuildExternal/asp/build/local; make V=1 O=0 CL_DEBUG=1 EXTRA_CFLAGS=-DNO_SAF)
	(cp -f prebuildExternal/target/x86_64/linux-3.5.7/lib/libmw.so libmw.so)
	(cp -f prebuildExternal/target/x86_64/linux-3.5.7/lib/libezxml.so .)
	(cp -f prebuildExternal/target/x86_64/linux-3.5.7/lib/libClTIPC.so .)
	(cp -f prebuildExternal/target/x86_64/linux-3.5.7/lib/libClUDP.so .)

clean:
	rm -rf libmw.so libezxml.so 
	rm -rf prebuildExternal/target
	rm -rf prebuildExternal/asp/target


# libClTIPC.so

