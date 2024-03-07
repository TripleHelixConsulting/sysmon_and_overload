#
# Cross Platform Makefile - Supports x86_64 and aarch64
#
# You will need GLFW (http://www.glfw.org):
# Install instructions for different platforms remain the same.

#CXX = g++
#CXX = clang++

EXE_x86_64 = loadTester_x86_64
EXE_aarch64 = loadTester_aarch64

IMGUI_DIR = imgui
TP_DIR = thread_pool
ENDLESS_TH_M_DIR = endlessThMngr

SOURCES = main.cpp K3Buffer.cpp K3Proc.cpp K3Key.cpp imgui_thread.cpp
SOURCES += $(TP_DIR)/thread_pool.cpp
SOURCES += $(ENDLESS_TH_M_DIR)/endless_th_manager.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp

## Object files per architecture
OBJS_x86_64 = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
OBJS_aarch64 = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

UNAME_S := $(shell uname -s)

## C++ flags (common for both architectures)
CXXFLAGS = -std=c++14 -pthread -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(IMGUI_DIR)/include -I$(TP_DIR) -I$(ENDLESS_TH_M_DIR)
CXXFLAGS += -g -Wall -Wformat

## Architecture specific flags
ARCH_FLAGS =

## Build rules for each architecture
all: $(EXE_x86_64) $(EXE_aarch64)

$(EXE_x86_64): $(OBJS_x86_64)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(ARCH_FLAGS_x86_64) $(LIBS)

$(EXE_aarch64): $(OBJS_aarch64)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(ARCH_FLAGS_aarch64) $(LIBS)

## Object file rules per architecture
%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(ARCH_FLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(ARCH_FLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) $(ARCH_FLAGS) -c -o $@ $<

%.o:$(TP_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(ARCH_FLAGS) -c -o $@ $<

%.o:$(ENDLESS_TH_M_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(ARCH_FLAGS) -c -o $@ $<

## Target specific architecture flags (needs to be defined)
ARCH_FLAGS_x86_64 = # Define flags for x86_64 build
ARCH_FLAGS_aarch64 = -march=armv8-a -mfpu=neon-fp-armv8 -Llibs_aarch64

clean:
	rm -f $(OBJS_x86_64) $(OBJS_aarch64)

clobber:
	rm -f $(EXE_x86_64) $(EXE_aarch64)
