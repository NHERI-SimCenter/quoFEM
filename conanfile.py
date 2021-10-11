from conans import ConanFile

class quoFEM(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    version = "0.0.9"
    license = "BSD"
    author = "NHERI SimCenter"
    url = "https://github.com/NHERI-SimCenter/quoFEM"
    description = "NHERI SimCenter R2D Tool"
    settings = "os", "compiler", "build_type", "arch"
    generators = "qmake"
    requires = "jansson/2.13.1", \
               "zlib/1.2.11", \
               "libcurl/7.72.0"
    build_policy = "missing"

    def configure(self):
        if self.settings.os == "Windows":
            self.options["libcurl"].with_ssl = True
            self.options["lapack"].visual_studio = True
            self.options["lapack"].shared = True

    def imports(self):
        if self.settings.os == "Windows":
            output = './%s' % self.settings.build_type
            self.copy("lib*.dll", dst=output, src="bin")
            self.copy("*", dst='{}/resources'.format(output), src="resources")
