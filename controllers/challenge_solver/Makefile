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

# no dependency is computed outside Visual C++, so any change in any CPP or HPP file should fire a call to msbuild
HPP_SOURCES = $(wildcard *.hpp)

MSBUILD?=/C/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe

release debug: $(CXX_SOURCES) $(HPP_SOURCES)
ifeq ($(wildcard $(MSBUILD)),)
	@echo "! Skipping compilation of challenge_solver.exe"
else
	@echo "# compiling challenge_solver.exe"
	@$(MSBUILD) challenge_solver.sln /verbosity:minimal /property:Configuration=Release > /dev/null
endif

clean:
	@rm -rf build *.opensdf *.pdb *.sdf *.suo *.vcxproj.user *.exe

else

RESOURCES_PATH = ../../aldebaran/simulator-sdk
INCLUDE = -I"$(RESOURCES_PATH)/include"
LIBRARIES = -L"$(RESOURCES_PATH)/lib" -Wl,-rpath,"$(RESOURCES_PATH)/lib" -lalcommon -lalerror -lalproxies -lalvalue -lboost_system -lopencv_core -lopencv_imgproc -lqi -lqitype
USE_C_API = true
CFLAGS = -Wno-deprecated-declarations -DBOOST_SIGNALS_NO_DEPRECATION_WARNING -Wno-unused-local-typedefs -D_GLIBCXX_USE_CXX11_ABI=0

# We need to use this until Aldebaran changes how they compile their libraries on Mac
USE_STD_CXX = true

### Do not modify: this includes Webots global Makefile.include
include $(WEBOTS_HOME_PATH)/resources/Makefile.include

endif
