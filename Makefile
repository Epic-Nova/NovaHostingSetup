include Utilities/Configurations/Custom.mk
include Utilities/Configurations/Defaults.mk
include Utilities/Configurations/${BUILD_CONFIGURATION}.mk

.SUFFIXES:
.DEFAULT_GOAL := all

.PHONY: all

SOURCE_FILES := $(shell find $(SOURCE_DIRECTORY) -name "*.cpp")
OBJECT_FILES := $(patsubst $(SOURCE_DIRECTORY)/%.cpp, $(BUILD_OBJECT_DIRECTORY)/%.o, $(SOURCE_FILES))
DEPENDENCY_FILES := $(patsubst $(SOURCE_DIRECTORY)/%.cpp, $(BUILD_OBJECT_DIRECTORY)/%.d, $(SOURCE_FILES))

all: $(OBJECT_FILES)
	@echo "Linking object files..."
	@mkdir -p $(BUILD_DIRECTORY)
	@$(BUILD_TOOLCHAIN) $(BUILD_FLAGS) $(BUILD_OPTIMSATIONS) -o $(BUILD_DIRECTORY)/DocumentationSetup $(OBJECT_FILES) $(THIRD_PARTY_LIBRARIES)

$(BUILD_OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp
	@echo "Compiling file: $<"
	@mkdir -p $(dir $@)
	@$(BUILD_TOOLCHAIN) $(BUILD_FLAGS) $(BUILD_OPTIMSATIONS) -c $< -o $@

-include $(DEPENDENCY_FILES)

$(BUILD_OBJECT_DIRECTORY)/%.d: $(SOURCE_DIRECTORY)/%.cpp
	@mkdir -p $(dir $@)
	@$(BUILD_TOOLCHAIN) $(BUILD_FLAGS) $(BUILD_OPTIMSATIONS) -MM $< -MT $(patsubst $(SOURCE_DIRECTORY)/%.cpp, $(BUILD_OBJECT_DIRECTORY)/%.o, $<) -MF $@

clean:
	@echo "Cleaning object files..."
	@rm -rf $(BUILD_OBJECT_DIRECTORY)
	@echo "Cleaning binaries..."
	@rm -rf $(BUILD_DIRECTORY)
