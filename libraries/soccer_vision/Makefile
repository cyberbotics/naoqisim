space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.os.include

# Webots Makefile system
#
# You may add some variable definitions hereafter to customize the build process
# See documentation in $(WEBOTS_HOME_PATH)/resources/Makefile.include

CXX_SOURCES = $(wildcard *.cpp)

ifeq ($(OSTYPE),windows)

 #no dependency is computed outside Visual C++, so any change in any CPP or HPP file should fire a call to msbuild
HPP_SOURCES = $(wildcard *.hpp)
MSBUILD?=/C/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe
release debug profile: soccer_vision_VS.lib
soccer_vision_VS.lib: $(CXX_SOURCES) $(HPP_SOURCES) soccer_vision.dll
ifeq ($(wildcard $(MSBUILD)),)
	@echo "! Skipping compilation of soccer_vision_VS.lib"
else
	@echo "# building soccer_vision_VS.lib using VC++"
	@$(MSBUILD) soccer_vision_VS.sln /verbosity:minimal /property:Configuration=Release > /dev/null
endif
FILES_TO_REMOVE=soccer_vision_VS.lib soccer_vision_VS.suo soccer_vision_VS.sdf soccer_vision_VS.vcxproj.user
endif # Window_NT

INCLUDE = -I"$(WEBOTS_HOME)/include"
ifeq ($(OSTYPE),windows)
  OPENCV_VERSION := $(subst .,,$(shell opencv_version))
  LIBRARIES = -L"$(WEBOTS_HOME)/msys64/mingw64/bin" -lopencv_core$(OPENCV_VERSION) -lopencv_imgproc$(OPENCV_VERSION)
else
  LIBRARIES = -L"$(WEBOTS_HOME)/lib" -lopencv_core -lopencv_imgproc
endif

### Do not modify: this includes Webots global Makefile.include
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
