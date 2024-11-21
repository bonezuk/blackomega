import os
import shutil
import sys
import subprocess
from pathlib import Path

isDebug = True
isUnitTest = False
isAppStore = False
isCodeSign = True

certNameWebApplication = "Developer ID Application: Stuart MacLean (NR9FA7GR93)"
certNameWebPackage = ""
certNameStoreApplication = "3rd Party Mac Developer Application: Stuart MacLean (NR9FA7GR93)"
certNameStorePackage = "Apple Distribution: Stuart MacLean (NR9FA7GR93)"

certNameApplication = ""
certNamePackage = ""

qt6VersionMajor = 6
qt6VersionMinor = 7
qt6VersionRevision = 0

def get_root_project_directory():
    return os.path.realpath(os.path.join(os.path.dirname(__file__), '..', '..'))

def get_source_directory():
    return os.path.realpath(os.path.join(get_root_project_directory(), "source"))

def get_app_bundle_directory():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Black Omega.app"))

def get_frameworks_directory():
    return os.path.realpath(os.path.join(get_app_bundle_directory(), "Contents", "Frameworks"))

def get_plugins_directory():
    return os.path.realpath(os.path.join(get_app_bundle_directory(), "Contents", "Plugins"))

def get_exec_directory():
    return os.path.realpath(os.path.join(get_app_bundle_directory(), "Contents", "MacOS"))

def get_plugins_imageformats_directory():
    return os.path.realpath(os.path.join(get_plugins_directory(), "imageformats"))

def get_plugins_platforms_directory():
    return os.path.realpath(os.path.join(get_plugins_directory(), "platforms"))

def get_plugins_resources_directory():
    return os.path.realpath(os.path.join(get_app_bundle_directory(), "Contents", "Resources"))

def get_qt6_root_directory():
	qtdirName = str(qt6VersionMajor) + "." + str(qt6VersionMinor) + "." + str(qt6VersionRevision)
	return os.path.realpath(os.path.join(Path.home(), "Qt", qtdirName))

def get_qt6_lib_directory():
	return os.path.realpath(os.path.join(get_qt6_root_directory(), "macos", "lib"))

def get_qt6_plugin_directory():
	return os.path.realpath(os.path.join(get_qt6_root_directory(), "macos", "plugins"))

def copy_qt6_framework(libName):
	print("Copy " + libName + ".framework")
	subprocess.check_call(["cp", "-R", os.path.join(get_qt6_lib_directory(), libName + ".framework"), get_frameworks_directory()])

def copy_qt6_plugin(pluginDir, pluginName):
    libName = "lib" + pluginName + ".dylib"
    srcPlugin = os.path.realpath(os.path.join(get_qt6_plugin_directory(), pluginDir, libName))
    print("copy plugin " + libName)
    if os.path.exists(srcPlugin) is not True:
        print(libName + " library not found '" + srcPlugin + "'")
        sys.exit(-1)
    destPlugin = os.path.realpath(os.path.join(get_plugins_directory(), pluginDir, libName))
    shutil.copyfile(srcPlugin, destPlugin)
    os.chmod(destPlugin, 0o755)


# Beginning of main build script
print("Build Black Omega application bundle for MacOSX using Qt6")
if os.path.exists(get_app_bundle_directory()):
    print("Removing existing app bundle '" + get_app_bundle_directory() + "'")
    shutil.rmtree(get_app_bundle_directory())

print("Creating application bundle directory '" + get_app_bundle_directory() + "'")
os.mkdir(get_app_bundle_directory())
os.makedirs(get_frameworks_directory())
os.makedirs(get_exec_directory())
os.makedirs(get_plugins_imageformats_directory())
os.makedirs(get_plugins_platforms_directory())
os.makedirs(get_plugins_resources_directory())

qt6CoreName = "QtCore"
copy_qt6_framework(qt6CoreName)
qt6GuiName = "QtGui"
copy_qt6_framework(qt6GuiName)
qt6XmlName = "QtXml"
copy_qt6_framework(qt6XmlName)
qt6WidgetsName = "QtWidgets"
copy_qt6_framework(qt6WidgetsName)
qt6Core5CompatName = "QtCore5Compat"
copy_qt6_framework(qt6Core5CompatName)
qt6NetworkName = "QtNetwork"
copy_qt6_framework(qt6NetworkName)
qt6TestName = "QtTest"
copy_qt6_framework(qt6TestName)

copy_qt6_plugin("platforms", "qcocoa")
copy_qt6_plugin("imageformats", "qgif")
copy_qt6_plugin("imageformats", "qjpeg")
