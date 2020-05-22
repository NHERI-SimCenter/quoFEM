from conans import ConanFile, CMake, tools
import os

class simCenterBackendApps(ConanFile):
    name = "SimCenterBackendApplications"
    version = "1.1.0"
    description = "Backend applications for SimCenter software"
    license = "BSD 3-Clause"
    author = "Michael Gardner mhgardner@berkeley.edu"
    url = "https://github.com/NHERI-SimCenter/SimCenterBackendApplications"
    settings = {"os": None, "build_type": None, "compiler": None, "arch": ["x86_64"]}
    options = {"shared": [True, False]}
    # default_options = {"shared": False}    
    generators = "cmake"
    build_policy = "missing"
    requires = "jansson/2.11@bincrafters/stable"
               

    # Custom attributes for Bincrafters recipe conventions
    _source_subfolder = "source_subfolder"
    _build_subfolder = "build_subfolder"

    def source(self):
       git = tools.Git(folder=self._source_subfolder)
       git.clone("https://github.com/shellshocked2003/SimCenterBackendApplications", "stable/1.1.0")

    def configure(self):
        self.options.shared = False
       
    def configure_cmake(self):
        cmake = CMake(self, msbuild_verbosity='detailed')
        cmake.configure(source_folder=self._source_subfolder)
        return cmake
    
    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="LICENSE", dst="licenses", src=self._source_subfolder)
        cmake = self.configure_cmake()
        cmake.install()
        self.copy("*", dst="bin", src=self._source_subfolder + "/applications")

    def package_info(self):
        self.env_info.PATH.append(os.path.join(self.package_folder, "bin"))
