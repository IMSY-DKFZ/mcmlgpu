.PHONY: wheel copy_files build_wheel clean

# Define directories
SRC_DIR := cmake-build-release
DEST_DIR := mcmlgpu/bin

# Define files to be copied
FILES := MCML safeprimes_base32.txt

# Main goal
wheel: copy_files build_wheel

# Target to copy necessary files to the Python package
copy_files:
	@mkdir -p $(DEST_DIR)
	@for file in $(FILES); do \
	    cp $(SRC_DIR)/$$file $(DEST_DIR)/; \
	done
	@echo "Files copied to $(DEST_DIR)"

# Target to build the Python wheel
build_wheel:
	@pip install build
	@python -m build
	@echo "Python wheel built successfully"

# Target to clean up build artifacts
clean:
	@rm -rf dist
	@rm -rf mcmlgpu.egg-info
	@echo "Cleaned build artifacts"
