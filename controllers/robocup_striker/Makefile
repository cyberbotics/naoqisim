# Webots Makefile system
#
# You may add some variable definitions hereafter to customize the build process
# See documentation in $(WEBOTS_HOME_PATH)/resources/Makefile.include

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
	@echo "! Skipping compilation of robocup_striker.exe"
else
	@echo "# compiling robocup_striker.exe"
	@$(MSBUILD) robocup_striker.sln /verbosity:minimal /property:Configuration=Release > /dev/null
endif

clean:
	@rm -rf build *.opensdf *.pdb *.sdf *.suo *.vcxproj.user *.exe

else

ifndef WEBOTS_NAOSIM_PATH
WEBOTS_NAOSIM_PATH = ../../aldebaran/simulator-sdk
endif

SOCCER_RESOURCES_PATH = ../..
NS_LIB_PATH = ../../libraries/nao_soccer
SV_LIB_PATH = ../../libraries/soccer_vision
INCLUDE = -I"$(NS_LIB_PATH)" -I"$(SV_LIB_PATH)" -I"../nao_soccer_supervisor" -I"$(WEBOTS_NAOSIM_PATH)/include"
LIBRARIES = -L"$(NS_LIB_PATH)" -lnao_soccer -L"$(WEBOTS_NAOSIM_PATH)/lib" -Wl,-rpath,"$(WEBOTS_NAOSIM_PATH)/lib" -L"$(SV_LIB_PATH)" -lsoccer_vision
CFLAGS = -std=c++0x -Wno-deprecated-declarations -DBOOST_SIGNALS_NO_DEPRECATION_WARNING -Wno-unused-local-typedefs -D_GLIBCXX_USE_CXX11_ABI=0

### Do not modify: this includes Webots global Makefile.include
include $(WEBOTS_HOME_PATH)/resources/Makefile.include

endif
