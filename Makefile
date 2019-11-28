CCX=clang++
CCXFLAGS = -std=c++17 -pthread
LDFLAGS :=
QPUFLAGS :=

SRCDIR  = ./src
OBJSDIR = ./build
DEPDIR	= ./include
GPUDIR = ./include/QPULib/Lib

QPULIB :=

ifeq ($(RELEASE), 1)
	CCXFLAGS += -Ofast
endif

ifeq ($(QPU), 1)
	CCXFLAGS += -DQPU_MODE -DQPU
	QPULIB := ./include/QPULib/qpulib.a
endif

ifeq ($(QPUE), 1)
	CCXFLAGS += -DEMULATION_MODE -DQPU
	QPULIB := ./include/QPULib/qpulib.a
	QPUFLAGS += EMU=1
endif

ifeq ($(WIRING), 1)
	CCXFLAGS += -DWIRING_PI=1
	LDFLAGS  += -lwiringPi
endif

ifeq ($(CAM), 1) 
	CCXFLAGS += -DRASPICAM=1
	LDFLAGS  += -lraspicam
endif

ifeq ($(OPENCV), 1)
	CCXFLAGS += -DOPENCV=1
	LDFLAGS  += -lopencv_core -lopencv_imgcodecs 
endif

# Find all subdirectories
INCLUDES = $(shell find $(SRCDIR) -type d | sed s/^/-I/)
# INCLUDES += $(shell find $(GPUDIR) -type d | sed s/^/-I/)

# Get all headers and sources from source directory
HEADERS = $(shell find $(SRCDIR) -type f -name '*.hpp')
SOURCES = $(shell find $(SRCDIR) -type f -name '*.cpp')

# Generate all objects and matching objects without directory path
OBJECTS  = $(SOURCES:$(SRCDIR)%.cpp=$(OBJSDIR)%.o)
OBJECTS_NO_PATH = $(foreach obj, $(OBJECTS), $(OBJSDIR)/$(notdir $(obj)))

# Add qpulib to rule if you want to make with it
project: $(OBJSDIR) $(OBJECTS)
	$(CCX) $(CCXFLAGS) $(OBJECTS_NO_PATH) $(LDFLAGS) -o project.out

$(OBJECTS): $(OBJSDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CCX) $(CCXFLAGS) $(INCLUDES) -c $< -o $(OBJSDIR)/$(@F)

$(QPULIB):
	cd ./include/QPULib && make $(QPUFLAGS)

$(OBJSDIR):
	mkdir build

all:
	@echo "------------------------------ Compiling project... ------------------------------"		
	make WIRING=1 CAM=1
	@echo "------------------------------- Compiled project! --------------------------------"

release:
	@echo "------------------------------ Compiling project... ------------------------------"		
	make WIRING=1 CAM=1 RELEASE=1
	@echo "------------------------------- Compiled project! --------------------------------"

clean:
	# cd ./include/QPULib && make clean
	rm -f project.out
	rm -rf $(OBJSDIR)