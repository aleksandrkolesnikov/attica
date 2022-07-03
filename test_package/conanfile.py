from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
import os


class AtticaTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("Attica/5.96.0@kde/testing")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.16.0]")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.so*", dst = "lib", src = "bin")

    def test(self):
        os.chdir(self.build_folder)
        self.run(".%sAtticaTestPackage" % os.sep)
