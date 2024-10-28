from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain

class PALRecipe(ConanFile):
    settings = "os", "build_type"

    def requirements(self):
        pass

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.generate()
        cd = CMakeDeps(self)
        cd.generate()