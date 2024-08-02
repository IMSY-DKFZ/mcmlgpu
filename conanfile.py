from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class MCMLRecipe(ConanFile):
    name = "mcml"
    version = "0.0.3"
    package_type = "application"

    # Optional metadata
    license = ""
    author = ""
    url = ""
    description = "Library to generate simulations of light-tissue interactions"
    topics = ("Monte Carlo", "GPU", "Light-Tissue interactions")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "cuda_arch": ["60", "61", "62", "70", "72", "75", "80", "86"]
    }
    default_options = {"shared": False, "fPIC": True, "cuda_arch": "86"}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "tqdm/*", "resources/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={"CUDA_ARCH": self.options.cuda_arch})
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["MCML"]
