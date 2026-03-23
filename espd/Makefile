include $(TOPDIR)/rules.mk

PKG_NAME:=espd
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/espd
	CATEGORY:=Espd
	TITLE:=Espd
	DEPENDS := cJSON libubus libubox libblobmsg-json libserialport 
endef

define Package/espd/description
	Daemon that communicates with esp controller
endef

define Package/espd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/espd $(1)/usr/bin
	$(INSTALL_BIN) ./files/espd.init $(1)/etc/init.d/espd
endef

$(eval $(call BuildPackage,espd))
