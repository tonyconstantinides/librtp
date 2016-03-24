export SHELL := /bin/bash

SDKS_ROOT := /usr/local/eva/sdks

HOST_SDKROOT := $(SDKS_ROOT)/darwin
HOST_SDK_BIN := $(HOST_SDKROOT)/bin
ARM_LINUX_SDKROOT := $(SDKS_ROOT)/arm-linux
ARM_LINUX_SDK_BIN := $(ARM_LINUX_SDKROOT)/bin
export PATH := $(ARM_LINUX_SDK_BIN):$(HOST_SDK_BIN):$(PATH)

PLATFORM ?= darwin
SDKROOT := $(SDKS_ROOT)/$(PLATFORM)
ifeq ($(PLATFORM),arm-linux)
	SYSROOT := $(SDKROOT)/arm-linux-gnueabihf/sysroot
	export PKG_CONFIG_LIBDIR := $(SYSROOT)/usr/lib/pkgconfig:$(SYSROOT)/usr/share/pkgconfig
endif

BUILD_DIR_BASE = build
BUILD_DIR = $(BUILD_DIR_BASE)/$(1)-$(PLATFORM)
SRCROOT := $(CURDIR)

XCODEPROJ_DIR = $(BUILD_DIR_BASE)/xcode
XCODEPROJ := $(XCODEPROJ_DIR)/$(shell basename $(SRCROOT)).xcodeproj

# FIXME: Move these into the SDK.
CMAKE_TOOLCHAIN_FILE = $(SRCROOT)/toolchains/$(PLATFORM).cmake

NINJA_FLAGS = $(if $(V),-v,)

BUILD_TYPE ?= debug
ifeq ($(BUILD_TYPE),debug)
	CMAKE_BUILD_TYPE := Debug
else
	ifeq ($(BUILD_TYPE),release)
		CMAKE_BUILD_TYPE := Release
	else
		ifeq ($(BUILD_TYPE),production)
			CMAKE_BUILD_TYPE := Production
		endif
	endif
endif

CMAKE = cmake
NINJA = ninja

NINJA_CMD = $(NINJA) $(NINJA_FLAGS)
CMAKE_CMD = $(CMAKE) -DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) $(1)
BUILD_CMD = [ ! -f "$(call BUILD_DIR,$(1))/build.ninja" ] && \
	( mkdir -p $(call BUILD_DIR,$(1)) && cd $(call BUILD_DIR,$(1)) && \
		$(call CMAKE_CMD,-GNinja $(2) $(SRCROOT)) && $(NINJA_CMD) ) || \
	( cd $(call BUILD_DIR,$(1)) && $(NINJA_CMD) )

.PHONY: all
all:
	$(call BUILD_CMD,$(BUILD_TYPE),-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE))

.PHONY: debug
debug:
	$(MAKE) BUILD_TYPE=debug

.PHONY: release
release:
	$(MAKE) BUILD_TYPE=release

.PHONY: production
production:
	$(MAKE) BUILD_TYPE=production

.PHONY: xcodeproj
xcodeproj:
	mkdir -p $(XCODEPROJ_DIR) && cd $(XCODEPROJ_DIR) && $(call CMAKE_CMD,-GXcode $(SRCROOT))
	@echo "Xcode project generated: $(XCODEPROJ)"

.PHONY: open-xcodeproj
open-xcodeproj: xcodeproj
	open $(XCODEPROJ)

.PHONY: install
install: all
	[[ "$(DESTDIR)" == "" ]] && ( echo "FATAL: Must specify a DESTDIR for the install target." && exit 1 ) \
		|| ( cd $(call BUILD_DIR,$(BUILD_TYPE)) && $(NINJA_CMD) install )

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR_BASE)
