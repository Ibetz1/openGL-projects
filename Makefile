# Compiler and flags
CXX = gcc
CXXFLAGS = -Iproject/include -Ilib -Wall -c

# Libraries and paths
LDFLAGS = -Llib -lglad -lglfw3 -lopengl32 -lgdi32 -lstdc++

# Directories and files
SRCDIR = project/src
ASSETSDIR = assets
SHADERDIR = project\shaders
BUILDDIR = build
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))
TARGET = $(BUILDDIR)/main.exe

# Phony targets to always execute
.PHONY: build run clean

# Build target that cleans and then builds
build: dirs clean $(TARGET) copy_assets

# Main executable
$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

# Build object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# Create build directory if it doesn't exist
dirs:
	if not exist $(BUILDDIR) mkdir $(BUILDDIR)
	if not exist $(SHADERDIR) mkdir $(SHADERDIR)
	if not exist $(ASSETSDIR) mkdir $(ASSETSDIR)

# Copy assets folder to build directory
copy_assets: $(BUILDDIR)
	@xcopy /E /I $(ASSETSDIR) $(BUILDDIR)\assets >nul
	@xcopy /E /I $(SHADERDIR) $(BUILDDIR)\shaders >nul

# Clean up contents of build directory only
clean:
	@if exist $(BUILDDIR) del /Q $(BUILDDIR)\*.* >nul
	@if exist $(BUILDDIR) for /D %%d in ($(BUILDDIR)\*) do rmdir /S /Q "%%d"

run:
	@cd $(BUILDDIR) && main.exe