CCX=clang++
CCXFLAGS = -std=c++17 -pthread 
LDFLAGS :=

SRCDIR  = ./src
OBJSDIR = ./build
DEPDIR	= ./include
GPUDIR = ./include/QPULib/Lib
QPULIB :=

ifeq ($(QPU), 1)
	CCXFLAGS += -DQPU_MODE
	QPULIB := ./include/QPULib/qpulib.a
endif

ifeq ($(WIRING), 1)
	LDFLAGS += -lwiringPi
	CCXFLAGS += -DWIRING_PI=1
endif

# Find all subdirectories
INCLUDES = $(shell find $(SRCDIR) -type d | sed s/^/-I/)
INCLUDES += $(shell find $(GPUDIR) -type d | sed s/^/-I/)

# Get all headers and sources from source directory
HEADERS = $(shell find $(SRCDIR) -type f -name '*.h')
SOURCES = $(shell find $(SRCDIR) -type f -name '*.cpp')

# Generate all objects and matching objects without directory path
OBJECTS  = $(SOURCES:$(SRCDIR)%.cpp=$(OBJSDIR)%.o)
OBJECTS_NO_PATH = $(foreach obj, $(OBJECTS), $(OBJSDIR)/$(notdir $(obj)))

project: $(OBJSDIR) $(QPULIB) $(OBJECTS)
	$(CCX) $(CCXFLAGS) $(OBJECTS_NO_PATH) $(QPULIB) $(LDFLAGS) -o project.out

$(OBJECTS): $(OBJSDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(CCX) $(CCXFLAGS) $(INCLUDES) -c $< -o $(OBJSDIR)/$(@F)

$(QPULIB):
	cd ./include/QPULib && make

$(OBJSDIR):
	mkdir build

clean:
	cd ./include/QPULib && make clean
	rm -f project.out
	rm -rf $(OBJSDIR)
	find $(OBJSDIR)/ -name '*.o' -delete
	find $(DEPDIR)/ -name '*.h.gch' -delete
	rm -r project.dSYM

run:
	@echo "------------------------------ Compiling project... ------------------------------"		
	make
	@echo "------------------------------ Running project... --------------------------------"
	./project.out