ifndef PETALINUX
$(error "Error: PETALINUX environment variable not set.  Change to the root of your PetaLinux install, and source the settings.sh file")
endif

include apps.common.mk

CAMAPP = CAM_MODE
FLPAPP = FLP
HK = hk
TRANSMIT = transmitadv

# Add any other object files to this list below
CAMAPP_OBJS = CAM_MODE.o
FLPAPP_OBJS = FLP.o
HW_OBJS = HMC5883L.o GY521.o GY80.o

all: build install

build: $(CAMAPP) $(FLPAPP) $(HK) $(TRANSMIT)

$(CAMAPP): $(CAMAPP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(CAMAPP_OBJS) $(LDLIBS)

$(FLPAPP): $(FLPAPP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(FLPAPP_OBJS) $(LDLIBS) 

$(HK): hk.c $(HW_OBJS)
	$(CC) hk.c $(LDFLAGS) -o $@ $(HW_OBJS) $(LDLIBS)

$(TRANSMIT): transmitadv.c $(HW_OBJS)
	$(CC) transmitadv.c $(LDFLAGS) -o $@ $(HW_OBJS) $(LDLIBS)

clean:
	-rm -f *.elf *.gdb *.o

.PHONY: install image

install: $(CAMAPP) $(FLPAPP) $(HK) $(TRANSMIT)
	$(TARGETINST) -d $(CAMAPP) /bin/$(CAMAPP) && $(TARGETINST) -d $(FLPAPP) /bin/$(FLPAPP) && $(TARGETINST) -d $(HK) /bin/$(HK) && $(TARGETINST) -d $(TRANSMIT) /bin/$(TRANSMIT)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

help:
	@echo ""
	@echo "Quick reference for various supported build targets for $(INSTANCE)."
	@echo "----------------------------------------------------"
	@echo "  clean                  clean out build objects"
	@echo "  all                    build $(INSTANCE) and install to rootfs host copy"
	@echo "  build                  build subsystem"
	@echo "  install                install built objects to rootfs host copy"
