space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.os.include

SIMULATOR_SDK_TARGET=simulator-sdk
ifeq ($(OSTYPE),darwin)
 SIMULATOR_SDK_VERSION=2.1.2.17
else
 SIMULATOR_SDK_VERSION=2.1.4.1
endif

ifeq ($(OSTYPE),linux)
 HOSTTYPE ?= $(shell uname -m)
 ifeq ($(HOSTTYPE),x86_64)
  OS_TAG=linux64
  SIMULATOR_SDK_PACKAGE=simulator-sdk-$(SIMULATOR_SDK_VERSION)-linux64.tar.gz
  PREVIOUS_SDK_PACKAGE=$(filter-out $(SIMULATOR_SDK_PACKAGE),$(wildcard simulator-sdk-*-linux64.tar.gz))
 else
  OS_TAG=linux32
  SIMULATOR_SDK_PACKAGE=simulator-sdk-$(SIMULATOR_SDK_VERSION)-linux32.tar.gz
  PREVIOUS_SDK_PACKAGE=$(filter-out $(SIMULATOR_SDK_PACKAGE),$(wildcard simulator-sdk-*-linux32.tar.gz))
 endif
 EXTRACT=tar xfz
 LIBFILES=lib*.so*
 WGET=wget
endif

ifeq ($(OSTYPE),darwin)
 OS_TAG=mac
 SIMULATOR_SDK_PACKAGE=simulator-sdk-$(SIMULATOR_SDK_VERSION)-mac64.tar.gz
 PREVIOUS_SDK_PACKAGE=$(filter-out $(SIMULATOR_SDK_PACKAGE),$(wildcard simulator-sdk-*-mac64.tar.gz))
 EXTRACT=tar xfz
 LIBFILES=lib*.dylib
 WGET=curl -O
endif

ifeq ($(OSTYPE),windows)
 OS_TAG=windows
 SIMULATOR_SDK_PACKAGE=simulator-sdk-$(SIMULATOR_SDK_VERSION)-win32-vs2010.zip
 PREVIOUS_SDK_PACKAGE=$(filter-out $(SIMULATOR_SDK_PACKAGE),$(wildcard simulator-sdk-*-win32-vs2010.zip))
 EXTRACT=unzip -q
 LIBFILES=*.lib
 WGET=wget
endif

ifeq ($(OSTYPE),windows)
 # remove .zip
 SIMULATOR_SDK_SOURCE=$(basename $(SIMULATOR_SDK_PACKAGE))
else
 # remove .tar.gz
 SIMULATOR_SDK_SOURCE=$(basename $(basename $(SIMULATOR_SDK_PACKAGE)))
endif

release debug profile: $(SIMULATOR_SDK_TARGET)

$(SIMULATOR_SDK_PACKAGE):
	@if ! $(WGET) -h > /dev/null ; then echo "You need to download the" $(SIMULATOR_SDK_PACKAGE) "package and copy it in this directory"; fi
	$(WGET) https://cyberbotics.com/files/repository/dependencies/$(OS_TAG)/release/$(SIMULATOR_SDK_PACKAGE)

$(SIMULATOR_SDK_TARGET): $(SIMULATOR_SDK_PACKAGE)
	@echo "#" extracting $(SIMULATOR_SDK_PACKAGE)
	@$(EXTRACT) $(SIMULATOR_SDK_PACKAGE)
	@echo "#" removing old $(SIMULATOR_SDK_TARGET)
	@rm -rf $(SIMULATOR_SDK_TARGET)
ifneq ($(PREVIOUS_SDK_PACKAGE),)
	@rm -f $(PREVIOUS_SDK_PACKAGE)
endif
	@echo "#" installing $(SIMULATOR_SDK_PACKAGE)
	@mv $(SIMULATOR_SDK_SOURCE) $(SIMULATOR_SDK_TARGET)
	@touch $(SIMULATOR_SDK_TARGET) # update modification date on Linux and Mac
ifeq ($(OSTYPE),windows)
	@sed -e 's/\/\/\ \#define\ BOOST_ALL_DYN_LINK/\#define\ BOOST_ALL_DYN_LINK/g' simulator-sdk/include/boost/config/user.hpp > user.hpp
	@mv user.hpp simulator-sdk/include/boost/config/user.hpp
	@rm simulator-sdk/bin/*d4.dll # Qt4 debug DLLs
	@rm simulator-sdk/bin/*243d.dll # OpenCV 2.4.3 debug DLLs
	@rm simulator-sdk/bin/boost_*-gd-1_55.dll # boost debug DLLs
	@rm simulator-sdk/bin/*_d.dll # other debug DLLs
	@rm simulator-sdk/bin/*_d.exe # debug binaries
	@rm simulator-sdk/lib/*_d.lib
	@rm simulator-sdk/lib/*/*_d.dll
	@rm simulator-sdk/lib/*243d.lib # OpenCV 2.4.3 debug LIBs
	@rm simulator-sdk/lib/boost_*-gd-1_55.lib # boost LIBs
	@rm simulator-sdk/lib/qtmaind.lib # Qt lib
else ifeq ($(OSTYPE),darwin)
	@rm -f simulator-sdk/lib/*Qt*_debug* # Qt4 debug libraries
	# fixed bad sym link
	@rm simulator-sdk/Python.framework/Python
	@ln -s Versions/2.7/Python simulator-sdk/Python.framework/Python
endif

# extract the full aldebaran packages (for testing only)
full:
	@echo "#" extracting $(SIMULATOR_SDK_PACKAGE)
	$(EXTRACT) $(SIMULATOR_SDK_PACKAGE)
	mv $(SIMULATOR_SDK_SOURCE) $(SIMULATOR_SDK_TARGET)
	touch $(SIMULATOR_SDK_TARGET)

clean:
	rm -rf $(SIMULATOR_SDK_TARGET)
	rm -rf $(SIMULATOR_SDK_SOURCE)
