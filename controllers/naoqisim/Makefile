space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))

CXX_SOURCES = $(wildcard *.cpp)

ifeq ($(OS),Windows_NT)

# no dependency is computed outside Visual C++, so any change in any CPP or HPP file should fire a call to msbuild
HPP_SOURCES = $(wildcard *.hpp)

MSBUILD?=/C/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe

release debug profile: $(CXX_SOURCES) $(HPP_SOURCES)
ifeq ($(wildcard $(MSBUILD)),)
	@echo "! Skipping compilation of naoqisim.exe controller"
else
	@echo "# compiling naoqisim.exe"
	@$(MSBUILD) naoqisim.sln /verbosity:minimal /property:Configuration=Release > /dev/null
endif

clean:
	@rm -rf build *.opensdf *.pdb *.sdf *.suo *.vcxproj.user *.exe

else

ifndef WEBOTS_NAOSIM_PATH
WEBOTS_NAOSIM_PATH = $(WEBOTS_HOME)/projects/robots/aldebaran/aldebaran/simulator-sdk
endif

### -----C/C++ Options-----
###
### add include path from Aldebaran's simulation-sdk:
INCLUDE = -I"$(WEBOTS_NAOSIM_PATH)/include"

###
### add libraries from Aldebaran's simulation-sdk:
### it is simpler to configure LD_LIBRARY_PATH rather than enumerating all libraries
LIBRARIES = -L"$(WEBOTS_NAOSIM_PATH)/lib" -Wl,-rpath,"$(WEBOTS_NAOSIM_PATH)/lib" -lalnaosim -lalrobotmodel -lqi -lalsimutils

include $(WEBOTS_HOME_PATH)/resources/Makefile.os.include
ifeq ($(OSTYPE),linux)
  # ensure ABI compliance using GCC 5
  CFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
endif

###
### use Webots C API:
USE_C_API = true

###
### this will can be dropped when Aldebaran will compile its libraries using libc++ (default in Mavericks 10.9)
USE_STD_CXX = true

### Do not modify: this includes Webots global Makefile.include
include $(WEBOTS_HOME_PATH)/resources/Makefile.include

endif
