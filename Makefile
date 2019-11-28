CCX=clang++
CCXFLAGS = -std=c++17 -pthread
LDFLAGS :=

SRCDIR  = ./src
OBJSDIR = ./build
DEPDIR	= ./include

ifeq ($(RELEASE), 1)
	CCXFLAGS += -Ofast
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

# Get all headers and sources from source directory
HEADERS = $(shell find $(SRCDIR) -type f -name '*.hpp')
SOURCES = $(shell find $(SRCDIR) -type f -name '*.cpp')

# Generate all objects
OBJECTS  = $(SOURCES:$(SRCDIR)%.cpp=$(OBJSDIR)%.o)
#OBJECTS_NO_PATH = $(foreach obj, $(OBJECTS), $(OBJSDIR)/$(notdir $(obj)))

# Add qpulib to rule if you want to make with it
project: $(OBJSDIR) $(OBJECTS)
	$(CCX) $(CCXFLAGS) $(OBJECTS) $(LDFLAGS) -o project.out

$(OBJECTS): $(OBJSDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CCX) $(CCXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJSDIR):
	# Recusively copy directory structure of src/ to build/ without files
	rsync -av -f"+ */" -f"- *" $(SRCDIR)/ $(OBJSDIR)

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