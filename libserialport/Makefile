include $(TOPDIR)/rules.mk
PKG_NAME:=libserialport
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL := git://sigrok.org/libserialport
PKG_SOURCE_VERSION:=21b3dfe5f68c205be4086469335fd2fc2ce11ed2
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/libserialport
	CATEGORY:=Libraries
	TITLE:=libserialport
endef


define Build/Configure
	cd $(PKG_BUILD_DIR); ./configure
	$(call Build/Configure/Default)
endef

define Package/libserialport/description
	libserialport (sometimes abbreviated as "sp") is a minimal, cross-platform shared library written in C that is intended to take care of the OS-specific details when writing software that uses serial ports.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/ $(1)/usr/include/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.libs/libserialport.so.0.1.1 $(1)/usr/lib/libserialport.so
	ln -rs $(1)/usr/lib/libserialport.so $(1)/usr/lib/libserialport.so.0
	ln -rs $(1)/usr/lib/libserialport.so $(1)/usr/lib/libserialport.so.0.1.1
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libserialport.h $(1)/usr/include/
endef


define Package/libserialport/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/.libs/libserialport.so.0.1.1 $(1)/usr/lib/libserialport.so
	ln -rs $(1)/usr/lib/libserialport.so $(1)/usr/lib/libserialport.so.0
	ln -rs $(1)/usr/lib/libserialport.so $(1)/usr/lib/libserialport.so.0.1.1
endef

$(eval $(call BuildPackage,libserialport))
