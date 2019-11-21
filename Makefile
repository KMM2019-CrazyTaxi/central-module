CCX=clang++
CCXFLAGS = -std=c++17 -pthread 
LDFLAGS :=
QPUFLAGS :=

SRCDIR  = ./src
OBJSDIR = ./build
DEPDIR	= ./include
GPUDIR = ./include/QPULib/Lib

QPULIB :=

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
INCLUDES += $(shell find $(GPUDIR) -type d | sed s/^/-I/)

# Get all headers and sources from source directory
HEADERS = $(shell find $(SRCDIR) -type f -name '*.hpp')
SOURCES = $(shell find $(SRCDIR) -type f -name '*.cpp')

# Generate all objects and matching objects without directory path
OBJECTS  = $(SOURCES:$(SRCDIR)%.cpp=$(OBJSDIR)%.o)
OBJECTS_NO_PATH = $(foreach obj, $(OBJECTS), $(OBJSDIR)/$(notdir $(obj)))

project: $(OBJSDIR) $(QPULIB) $(OBJECTS)
	$(CCX) $(CCXFLAGS) $(OBJECTS_NO_PATH) $(QPULIB) $(LDFLAGS) -o project.out

$(OBJECTS): $(OBJSDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CCX) $(CCXFLAGS) $(INCLUDES) -c $< -o $(OBJSDIR)/$(@F)

$(QPULIB):
	cd ./include/QPULib && make $(QPUFLAGS)

$(OBJSDIR):
	mkdir build

all:
	make WIRING=1 QPU=1 CAM=1 OPENCV=1

emulation:
	make WIRING=1 QPUE=1 CAM=1 OPENCV=1

clean:
	cd ./include/QPULib && make clean
	rm -f project.out
	rm -rf $(OBJSDIR)

run:
	@echo "------------------------------ Compiling project... ------------------------------"		
	make
	@echo "------------------------------ Running project... --------------------------------"
	./project.out