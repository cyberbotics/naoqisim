
space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.os.include

ifndef WEBOTS_NAOSIM_PATH
WEBOTS_NAOSIM_PATH = $(WEBOTS_HOME)/projects/robots/aldebaran/aldebaran/simulator-sdk
endif
CXX_SOURCES = $(wildcard *.cpp)

ifeq ($(OSTYPE),linux)
  # std=c++11 is not used here because it causes a lot of errors in macros from the simulator-sdk
  # -Wno-unused-variable is used to hide warnings about unused variables in boost includes used by the simulator-sdk
  CFLAGS += -std=gnu++11 -Wno-unused-variable -Wno-deprecated-declarations
  # ensure ABI compliance
  CFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
endif

ifeq ($(OSTYPE),windows)

# no dependency is computed outside Visual C++, so any change in any CPP or HPP file should fire a call to msbuild
HPP_SOURCES = $(wildcard *.hpp)

MSBUILD?=/C/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe

release debug profile: $(CXX_SOURCES) $(HPP_SOURCES)
ifeq ($(wildcard $(MSBUILD)),)
	@echo "! Skipping compilation of nao_soccer.lib"
else
	@echo "# compiling nao_soccer.lib"
	@$(MSBUILD) nao_soccer.sln /verbosity:minimal /property:Configuration=Release > /dev/null
endif

clean:
	@rm -rf build *.opensdf *.pdb *.sdf *.suo *.vcxproj.user *.lib

else

SV_LIB_PATH = ../soccer_vision
INCLUDE = -I"$(WEBOTS_NAOSIM_PATH)/include" -I"../../controllers/nao_soccer_supervisor" -I"$(SV_LIB_PATH)"
LIBRARIES = -L"$(WEBOTS_NAOSIM_PATH)/lib" -Wl,-rpath,"$(WEBOTS_NAOSIM_PATH)/lib" -lalvalue -lalerror -lalproxies -lalcommon -lqi -lqitype -lboost_system -L"$(SV_LIB_PATH)" -lsoccer_vision

# We need to use this until Aldebaran changes how they compile their libraries on Mac
USE_STD_CXX = true

# Hide Boost warnings from Aldebaran libraries
CFLAGS += -DBOOST_SIGNALS_NO_DEPRECATION_WARNING
CFLAGS += -Wno-unused-local-typedefs

### Do not modify: this includes Webots global Makefile.include
include $(WEBOTS_HOME_PATH)/resources/Makefile.include

endif
