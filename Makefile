CC 	 := gcc
CFLAGS := -Wall -Werror -g

.PHONY := directories all clean

SRCDIR 	   := src
BUILDDIR   := build
OBJSDIR    := $(BUILDDIR)/objs
DEPSDIR    := $(BUILDDIR)/deps
TARGETDIR  := lib
TARGETNAME := librdsm.a
TARGET 	   := $(TARGETDIR)/$(TARGETNAME)
DIRS 	   := $(TARGETDIR) $(BUILDDIR) $(DEPSDIR) $(OBJSDIR)

SRCS := $(shell ls $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJSDIR)/%.o, $(SRCS))
DEPS := $(patsubst $(SRCDIR)/%.c, $(DEPSDIR)/%.d, $(SRCS))


all: directories $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	@echo "Building the Rocket library..."
	ar rcs $@ $^


$(OBJSDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -c $< -o $@ -MMD -MF $(patsubst $(OBJSDIR)/%.o, $(DEPSDIR)/%.d, $@)


directories:
	@echo "Creating necessary build and output directories for rocket..."
	@mkdir -p $(DIRS)


clean:
	@echo "Cleaning files and directories of build for rocket..."
	@$(RM) -r $(DIRS)

