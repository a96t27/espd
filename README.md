# espd

OpenWrt feed for ESP controller integration.

This feed provides packages for communicating with ESP-based controllers over serial interface.

## Packages

### libserialport

Cross-platform library for serial port communication.

Provides OS-independent access to serial devices for applications.

### espd

Daemon that communicates with ESP controller over serial port.

## Dependencies

### libserialport

- Built from external sigrok repository

### espd

- cJSON
- libubus
- libubox
- libblobmsg-json
- libserialport

## Installation

Add feed:

```text
src-git espd https://github.com/a96t27/espd.git
```

Update and install:

```bash
./scripts/feeds update espd
./scripts/feeds install -a -p espd
```

## Configuration (menuconfig)

Run configuration menu:

```bash
make menuconfig
```

Then locate packages:

### libserialport

```
Libraries  --->
    <*> libserialport
```

### espd

```
Utilities  --->
    <*> espd
```

After selecting packages:

- Save configuration
- Exit menuconfig

## Build

Build individual packages:

```bash
make package/libserialport/compile V=s
make package/espd/compile V=s
```

Or build full firmware:

```bash
make -j$(nproc)
```

## Installing produced .ipk packages

After building, the generated `.ipk` packages will be located in:

```text
bin/packages/<arch>/espd/
bin/packages/<arch>/base/
```

Example:

```text
bin/packages/aarch64_cortex-a53/espd/
```

### Find generated packages

```bash
find bin/packages -name "*.ipk"
```

## Installing on a running OpenWrt device

Copy packages to the router:

```bash
# Replace <DEVICE_IP> with your OpenWrt device IP
scp bin/packages/*/espd/*.ipk root@<DEVICE_IP>:/tmp/
```

Install with opkg:

```bash
opkg install /tmp/libserialport_*.ipk
opkg install /tmp/espd_*.ipk
```

If dependencies are missing:

```bash
opkg install /tmp/*.ipk --force-depends
```

## Verify installation

```bash
opkg list-installed | grep espd
```

## Service

```bash
/etc/init.d/espd enable
/etc/init.d/espd start
/etc/init.d/espd stop
/etc/init.d/espd restart
```
