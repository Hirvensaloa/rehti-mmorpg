from conan import ConanFile

class ClientConan(ConanFile):
    name = "Client"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    tool_requires = "cmake/3.27.0"
    def requirements(self):
        self.requires("boost/1.83.0")
        self.requires("vulkan-loader/1.3.239.0")
        self.requires("vulkan-validationlayers/1.3.239.0")
        self.requires("spirv-tools/1.3.239.0", override=True)
        self.requires("glfw/3.3.8")
        self.requires("vulkan-memory-allocator/3.0.1")
        self.requires("stb/cci.20220909")
        self.requires("glm/cci.20230113")
        self.requires("vulkan-headers/1.3.239.0", override=True)
	self.requires("imgui/cci.20230105+1.89.2.docking")

