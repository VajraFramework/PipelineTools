################################################################################

WORKDIR = `pwd`

define all-cpp-files-under
$(patsubst ./%, %, \
  $(shell cd $(WORKDIR) ; \
          find $(1) -name "*.cpp" -and -not -name ".*") \
 )
endef

define all-c-files-under
$(patsubst ./%, %, \
  $(shell cd $(WORKDIR) ; \
          find $(1) -name "*.c" -and -not -name ".*") \
 )
endef

################################################################################

GCC_DEBUG_FLAGS += -g -D_DEBUG -std=c++11 -DPLATFORM_DESKTOP -DPLATFORM_LINUX -DDEBUG
M64 = -m64

EXECUTABLE     = executable
BUILDDIR 	= ./Build
BINDIR      = $(BUILDDIR)/bin
OBJDIR      = $(BUILDDIR)/obj
DEPSDIR      = $(BUILDDIR)/deps
RESOURCESDIR_NAME      = Resources
FBXSDK 		= ./Source/FbxSdk
LIBDIR		= $(FBXSDK)/lib/gcc4/x64/debug
INCDIRS     = ./Source/ $(FBXSDK)/include/
INC         = $(addprefix -I, $(INCDIRS))

CWD   		= .

LOCAL_SRC_DIRS = $(CWD)/Source/
OBJECTS += $(addprefix $(OBJDIR)/,\
				$(patsubst %.cpp,%.o,\
					$(call all-cpp-files-under, $(LOCAL_SRC_DIRS))\
				 )\
			)
DEPS += $(addprefix $(DEPSDIR)/,\
				$(patsubst %.cpp,%.d,\
					$(call all-cpp-files-under, $(LOCAL_SRC_DIRS))\
				 )\
			)

TARGET      = $(BINDIR)/$(EXECUTABLE)

CC          = g++
LD          = g++

CXXFLAGS    = $(M64) $(GCC_DEBUG_FLAGS)
LDFLAGS     = $(M64) $(GCC_LINK_FLAGS)

RPATH_DEF    =
STATIC_LINK  =
FBXSDK_LIB   = -lfbxsdk
CXXFLAGS += -DFBXSDK_SHARED
RPATH_DEF = -Wl,-rpath $(PWD)/$(LIBDIR)

LIBS = $(FBXSDK_LIB) -lm -lrt -luuid -lstdc++ -lpthread -ldl -L/usr/./lib/debug/lib/x86_64-linux-gnu/ -ldl-2.17


all: build_all

build_all: before_build $(TARGET) after_build

before_build:
	@echo "List of all objs: $(OBJECTS)"
	@echo "List of all deps: $(DEPS)"
	@mkdir -p $(OBJDIR)
	@mkdir -p $(DEPSDIR)
	@mkdir -p $(BINDIR)
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(dir $(DEPS))


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) -L$(LIBDIR) $(LIBS) $(RPATH_DEF)

-include $(DEPS)

$(OBJDIR)/%.o: %.c*
	$(CC) $(CXXFLAGS) $(INC) -c $< -o $@
	$(CC) -MM $(CXXFLAGS) $(INC) -c $< -o $(DEPSDIR)/$*.d
	@mv -f $(DEPSDIR)/$*.d $(DEPSDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(DEPSDIR)/$*.d.tmp > $(DEPSDIR)/$*.d
	@rm -f $*.d.tmp

after_build:
	rsync -r -v --delete $(RESOURCESDIR_NAME) $(BINDIR)/.

clean:
	rm -rf $(BUILDDIR)

################################################################################	
