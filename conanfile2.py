from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.files import copy
import os

class QuoFEMConan(ConanFile):
    name = "quoFEM"
    license = "BSD-3-Clause"
    author = "NHERI SimCenter"
    url = "https://github.com/NHERI-SimCenter/quoFEM"
    description = "NHERI SimCenter R2D Tool"
    settings = "os", "arch", "compiler", "build_type"

    def requirements(self):
        self.requires("jansson/2.13.1")
        self.requires("zlib/1.3.1")
        if self.settings.os != "Linux":
            self.requires("libcurl/8.12.1")

    def generate(self):
        # create the toolchain and config files for CMake        
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

        # to address "DLL not found" error during local development .. why seperate generate function rather than typical generators = "CMakeDeps", "CMakeToolchain"
        if self.settings.os == "Windows":
            # For CMake, the binary usually ends up in build/Release or build/Debug
            bindir = os.path.join(self.build_folder, str(self.settings.build_type))
            for dep in self.dependencies.values():
                copy(self, "*.dll", dep.cpp_info.bindirs[0], bindir)
                
