import os
import shutil
import sys
import subprocess

isDebug = False
isUnitTest = False
isAppStore = False
isCodeSign = True

certNameWebApplication = "Developer ID Application: Stuart MacLean (NR9FA7GR93)"
certNameWebPackage = ""
certNameStoreApplication = "3rd Party Mac Developer Application: Stuart MacLean (NR9FA7GR93)"
certNameStorePackage = "3rd Party Mac Developer Installer: Stuart MacLean (NR9FA7GR93)"

certNameApplication = ""
certNamePackage = ""

qt5VersionMajor = 5
qt5VersionMinor = 10
qt5VersionRevision = 1

def get_root_project_directory():
    return os.path.realpath(os.path.join(os.path.dirname(__file__), '..', '..'))

def get_source_directory():
    return os.path.realpath(os.path.join(get_root_project_directory(), "source"))

def get_app_bundle_directory():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Black Omega.app"))

def get_app_package():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Black Omega.pkg"))

def get_app_final_bundle_directory():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Black Omega.app"))

def get_contents_directory():
    return os.path.realpath(os.path.join(get_app_bundle_directory(), "Contents"))

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

def get_plugins_printsupport_directory():
    return os.path.realpath(os.path.join(get_plugins_directory(), "printsupport"))

def get_plugins_resources_directory():
    return os.path.realpath(os.path.join(get_app_bundle_directory(), "Contents", "Resources"))

def get_build_lib_path():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Omega", "lib"))

def get_qt5_libname(libName):
    name = "lib" + libName
    if isDebug:
        name += "_debug"
    name +=  "." + str(qt5VersionMajor) + "." + str(qt5VersionMinor) + "." + str(qt5VersionRevision) + ".dylib"
    return name

def get_qt5_lib_major_minor_name(libName):
    name = "lib" + libName
    if isDebug:
        name += "_debug"
    name +=  "." + str(qt5VersionMajor) + "." + str(qt5VersionMinor) + ".dylib"
    return name

def get_qt5_lib_major_name(libName):
    name = "lib" + libName
    if isDebug:
        name += "_debug"
    name +=  "." + str(qt5VersionMajor) + ".dylib"
    return name

def get_qt5_lib_plain_name(libName):
    name = "lib" + libName
    if isDebug:
        name += "_debug"
    name +=  ".dylib"
    return name

def get_dest_qt5_library(libName):
    dest = get_plugins_directory() + "/" + get_qt5_libname(libName)
    return dest

def get_dest_qt5_major_minor_library(libName):
    dest = get_plugins_directory() + "/" + get_qt5_lib_major_minor_name(libName)
    return dest

def get_dest_qt5_major_library(libName):
    dest = get_plugins_directory() + "/" + get_qt5_lib_major_name(libName)
    return dest

def get_dest_qt5_plain_library(libName):
    dest = get_plugins_directory() + "/" + get_qt5_lib_plain_name(libName)
    return dest

def get_source_qt5_library(libName):
    src = get_build_lib_path() + "/" + get_qt5_libname(libName)
    return src

def copy_and_link_qt5_library(libName):
    print("copy " + libName + ".dylib")
    if os.path.exists(get_source_qt5_library(libName)) is not True:
        print(libName + " library not found '" + get_source_qt5_library(libName) + "'")
        sys.exit(-1)
    shutil.copyfile(get_source_qt5_library(libName), get_dest_qt5_library(libName))
    os.chmod(get_dest_qt5_library(libName), 0o755)
    os.symlink(get_qt5_libname(libName), get_dest_qt5_major_minor_library(libName))
    os.symlink(get_qt5_libname(libName), get_dest_qt5_major_library(libName))
    os.symlink(get_qt5_libname(libName), get_dest_qt5_plain_library(libName))

def copy_qt5_plugin(pluginDir, pluginName):
    libName = "lib" + pluginName + ".dylib"
    srcPlugin = os.path.realpath(os.path.join(get_build_lib_path(), pluginDir, libName))
    print("copy plugin " + libName)
    if os.path.exists(srcPlugin) is not True:
        print(libName + " library not found '" + srcPlugin + "'")
        sys.exit(-1)
    destPlugin = os.path.realpath(os.path.join(get_plugins_directory(), pluginDir, libName))
    shutil.copyfile(srcPlugin, destPlugin)
    os.chmod(destPlugin, 0o755)

def copy_and_link_library(libName, major):
    print("copy " + libName + ".dylib")
    lName = libName + "." + str(major) + ".dylib"
    srcLib = get_build_lib_path() + "/" + lName
    destLib = get_plugins_directory() + "/" + lName
    if os.path.exists(srcLib) is not True:
        print(libName + " library not found '" + srcLib + "'")
        sys.exit(-1)
    shutil.copyfile(srcLib, destLib)
    os.chmod(destLib, 0o755)
    lnLib = get_plugins_directory() + "/" + libName + ".dylib"
    os.symlink(lName, lnLib)

def copy_plain_library(libName):
    print("copy " + libName + ".dylib")
    srcLib = get_build_lib_path() + "/" + libName + ".dylib"
    destLib = get_plugins_directory() + "/" + libName + ".dylib"
    if os.path.exists(srcLib) is not True:
        print(libName + " library not found '" + srcLib + "'")
        sys.exit(-1)
    shutil.copyfile(srcLib, destLib)
    os.chmod(destLib, 0o755)

def copy_executable(exeName):
    print("copy " + exeName)
    srcExec = get_build_lib_path() + "/" + exeName
    destExec = get_exec_directory() + "/" + exeName
    if os.path.exists(srcExec) is not True:
        print(exeName + " executble not found '" + srcExec + "'")
        sys.exit(-1)
    shutil.copyfile(srcExec, destExec)
    os.chmod(destExec, 0o755)

def copy_resource(resourceName, srcDir, destDir):
    print("copy resource " + resourceName)
    srcFile = srcDir + "/" + resourceName
    destFile = destDir + "/" + resourceName
    if os.path.exists(srcFile) is not True:
        print("resource not found '" + srcFile + "'")
        sys.exit(-1)
    shutil.copyfile(srcFile, destFile)
    os.chmod(destFile, 0o644)

def copy_plist():
    if isAppStore:
        srcDir = get_source_directory() + "/player/appstore"
    else:
        srcDir = get_source_directory() + "/player"
    copy_resource("Info.plist", srcDir, get_contents_directory())

def copy_icon():
    srcDir = get_source_directory() + "/player"
    copy_resource("omega.icns", srcDir, get_plugins_resources_directory())

def copy_and_help():
    print("Copy Black Omega.help")
    if isAppStore:
        srcHelpDir = get_source_directory() + "/help/appleStore/Black Omega.help"
    else:
        srcHelpDir = get_source_directory() + "/help/apple/Black Omega.help"
    destHelpDir = get_plugins_resources_directory() + "/Black Omega.help"
    shutil.copytree(srcHelpDir, destHelpDir)

def relink_id_for_library_major(libName, major):
    print("relink " + libName)
    lName = libName + "." + str(major) + ".dylib"
    idPath = "@executable_path/../PlugIns/" + lName
    libPath = get_plugins_directory() + "/" + lName
    subprocess.check_call(["install_name_tool", "-id", idPath, libPath])

def relink_change_library(imprint, libName, targetLib):
    oldPath = imprint + "/" + libName + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetLibName = get_plugins_directory() + "/" + targetLib + ".dylib"
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_id_for_qt5_library(libName):
    print("relink " + libName)
    idPath = "@executable_path/../PlugIns/" + get_qt5_libname(libName)
    libPath = get_plugins_directory() + "/" + get_qt5_libname(libName)
    subprocess.check_call(["install_name_tool", "-id", idPath, libPath])

def relink_change_for_qt5_library(imprint, libName, targetLib):
    oldPath = imprint + "/" + get_qt5_lib_major_name(libName)
    execLibName = "@executable_path/../PlugIns/" + get_qt5_lib_major_name(libName)
    targetLibName = get_plugins_directory() + "/" + get_qt5_libname(targetLib)
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_change_qt5_path(imprint, libName, targetLib):
    oldPath = imprint + "/" + get_qt5_lib_major_name(libName)
    execLibName = "@executable_path/../PlugIns/" + get_qt5_lib_major_name(libName)
    targetLibName = get_plugins_directory() + "/" + targetLib + ".dylib"
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_id_for_qt5_plugin_library(pluginDir, libName):
    lName = "lib" + libName + ".dylib"
    print("relink plugin " + pluginDir + "/" + lName)
    idPath = "@executable_path/../PlugIns/" + pluginDir + "/" + lName
    libPath = get_plugins_directory() + "/" + pluginDir + "/" + lName
    subprocess.check_call(["install_name_tool", "-id", idPath, libPath])

def relink_change_for_qt5_platform_library(pluginDir, pluginName, libName):
    lName = "lib" + pluginName + ".dylib"
    oldPath = "@rpath/" + get_qt5_lib_major_name(libName)
    execLibName = "@executable_path/../PlugIns/" + get_qt5_lib_major_name(libName)
    targetLibName = get_plugins_directory() + "/" + pluginDir + "/" + lName
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_id_for_library(libName):
    print("relink " + libName)
    lName = libName + ".dylib"
    idPath = "@executable_path/../PlugIns/" + lName
    libPath = get_plugins_directory() + "/" + lName
    subprocess.check_call(["install_name_tool", "-id", idPath, libPath])

def relink_change_omega_library(libName, targetLib):
    oldPath = get_build_lib_path() + "/" + libName + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetLibName = get_plugins_directory() + "/" + targetLib + ".dylib"
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_omega_library(libName, qtModules, libsArray, omegaArray):
    relink_id_for_library(libName)
    for mod in qtModules:
        relink_change_qt5_path("@rpath", mod, libName)
    for lib in libsArray:
        relink_change_library("@rpath", lib, libName)
    for lib in omegaArray:
        relink_change_omega_library(lib, libName)

def relink_change_qt5_path_exec(imprint, libName, targetExec):
    oldPath = imprint + "/" + get_qt5_lib_major_name(libName)
    execLibName = "@executable_path/../PlugIns/" + get_qt5_lib_major_name(libName)
    targetExecName = get_exec_directory() + "/" + targetExec
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetExecName])

def relink_change_library_exec(imprint, libName, targetExec):
    oldPath = imprint + "/" + libName + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetExecName = get_exec_directory() + "/" + targetExec
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetExecName])

def relink_change_omega_library_exec(libName, targetExec):
    oldPath = get_build_lib_path() + "/" + libName + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetExecName = get_exec_directory() + "/" + targetExec
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetExecName])

def relink_omega_executable(execName, qtModules, libsArray, omegaArray):
    print("relink " + execName)
    for mod in qtModules:
        relink_change_qt5_path_exec("@rpath", mod, execName)
    for lib in libsArray:
        relink_change_library_exec("@rpath", lib, execName)
    for lib in omegaArray:
        relink_change_omega_library_exec(lib, execName)

def codesign_qt5_library(libName):
    print("Signing " + libName)
    subprocess.check_call(["codesign", "-s", certNameApplication, get_dest_qt5_library(libName)])

def codesign_qt5_plugin(pluginDir, pluginName):
    print("Signing plugin " + pluginDir +"/" + pluginName)
    libName = "lib" + pluginName + ".dylib"
    destPlugin = os.path.realpath(os.path.join(get_plugins_directory(), pluginDir, libName))
    subprocess.check_call(["codesign", "-s", certNameApplication, destPlugin])

def codesign_library_version(libName, majorVer):
    print("Signing " + libName + ".dylib")
    lName = libName + "." + str(majorVer) + ".dylib"
    destLib = get_plugins_directory() + "/" + lName
    subprocess.check_call(["codesign", "-s", certNameApplication, destLib])

def codesign_library_plain(libName):
    print("Signing " + libName + ".dylib")
    destLib = get_plugins_directory() + "/" + libName + ".dylib"
    subprocess.check_call(["codesign", "-s", certNameApplication, destLib])

def codesign_exec(execName):
    print("Signing " + execName)
    destExec = get_exec_directory() + "/" + execName
    subprocess.check_call(["codesign", "-s", certNameApplication, destExec])

def codesign_app_bundle():
    print("Signing APP bundle")
    if isAppStore:
        entitlePath = get_source_directory() + "/player/appstore/BlackOmega.entitlements"
        subprocess.check_call(["codesign", "-s", certNameApplication, "-v", "--entitlements", entitlePath, get_app_bundle_directory()])
    else:
        subprocess.check_call(["codesign", "-s", certNameApplication, get_app_bundle_directory()])

def codesign_help():
    print("Signing Help")
    destHelpDir = get_plugins_resources_directory() + "/Black Omega.help"
    subprocess.check_call(["codesign", "-s", certNameApplication, destHelpDir])

# Beginning of main build script
print("Build Black Omega application bundle for MacOSX")
if os.path.exists(get_app_bundle_directory()):
    print("Removing existing app bundle '" + get_app_bundle_directory() + "'")
    shutil.rmtree(get_app_bundle_directory())
if os.path.exists(get_app_final_bundle_directory()):
    print("Removing existing app bundle '" + get_app_final_bundle_directory() + "'")
    shutil.rmtree(get_app_final_bundle_directory())
print("Creating application bundle directory '" + get_app_bundle_directory() + "'")
os.mkdir(get_app_bundle_directory())
os.makedirs(get_frameworks_directory())
os.makedirs(get_exec_directory())
os.makedirs(get_plugins_imageformats_directory())
os.makedirs(get_plugins_platforms_directory())
os.makedirs(get_plugins_printsupport_directory())
os.makedirs(get_plugins_resources_directory())

qt5CoreLibName = "Qt5Core"
copy_and_link_qt5_library(qt5CoreLibName)
qt5GuiLibName = "Qt5Gui"
copy_and_link_qt5_library(qt5GuiLibName)
qt5XmlLibName = "Qt5Xml"
copy_and_link_qt5_library(qt5XmlLibName)
qt5WidgetsLibName = "Qt5Widgets"
copy_and_link_qt5_library(qt5WidgetsLibName)
qt5TestLibName = "Qt5Test"
copy_and_link_qt5_library(qt5TestLibName)
qt5PrintSupportLibName = "Qt5PrintSupport"
copy_and_link_qt5_library(qt5PrintSupportLibName)

copy_qt5_plugin("platforms", "qcocoa")
copy_qt5_plugin("printsupport", "cocoaprintersupport")
copy_qt5_plugin("imageformats", "qgif")
copy_qt5_plugin("imageformats", "qjpeg")

copy_and_link_library("libixml", 2)
copy_and_link_library("libthreadutil", 6)
copy_and_link_library("libupnp", 6)

copy_plain_library("libmpcdec")
copy_and_link_library("libwavpack", 1)
copy_and_link_library("libxml2", 2)

if isUnitTest:
    copy_plain_library("libgtest")
    copy_plain_library("libgtest_main")
    copy_plain_library("libgmock")
    copy_plain_library("libgmock_main")

copy_and_help()
copy_plist()
copy_icon()

copy_plain_library("libcommon")
copy_plain_library("libengine")
copy_plain_library("libblueomega")
copy_plain_library("libsilveromega")
copy_plain_library("libblackomega")
copy_plain_library("libredomega")
copy_plain_library("libwhiteomega")
copy_plain_library("libgreenomega")
copy_plain_library("libvioletomega")
copy_plain_library("libcyanomega")
copy_plain_library("libtoneomega")
copy_plain_library("libwavpackomega")
copy_plain_library("libnetwork_omega")
copy_plain_library("librtp")
copy_plain_library("librtp_silveromega")
copy_plain_library("libhttp")
copy_plain_library("libmime")
copy_plain_library("libaudioio")
copy_plain_library("libtrackinfo")
copy_plain_library("libtrackdb")
copy_plain_library("libtrackmodel")
copy_plain_library("libdlna")
copy_plain_library("libremote")
copy_plain_library("libwidget")

copy_executable("Omega")
if isUnitTest:
    copy_executable("Test")

relink_id_for_library_major("libixml",2)
relink_id_for_library_major("libthreadutil",6)

relink_id_for_library_major("libupnp",6)
relink_change_library("@rpath", "libixml.2", "libupnp.6")
relink_change_library("@rpath", "libthreadutil.6", "libupnp.6")

relink_id_for_qt5_library("Qt5Core")

relink_id_for_qt5_library("Qt5Gui")
relink_change_for_qt5_library("@rpath", "Qt5Core", "Qt5Gui")

relink_id_for_qt5_library("Qt5Xml")
relink_change_for_qt5_library("@rpath", "Qt5Core", "Qt5Xml")

relink_id_for_qt5_library("Qt5Widgets")
relink_change_for_qt5_library("@rpath", "Qt5Core", "Qt5Widgets")
relink_change_for_qt5_library("@rpath", "Qt5Gui", "Qt5Widgets")

if isUnitTest:
    relink_id_for_qt5_library("Qt5Test")
    relink_change_for_qt5_library("@rpath", "Qt5Core", "Qt5Test")

relink_id_for_qt5_library("Qt5PrintSupport")
relink_change_for_qt5_library("@rpath", "Qt5Core", "Qt5PrintSupport")
relink_change_for_qt5_library("@rpath", "Qt5Gui", "Qt5PrintSupport")
relink_change_for_qt5_library("@rpath", "Qt5Widgets", "Qt5PrintSupport")

relink_id_for_qt5_plugin_library("platforms", "qcocoa")
relink_change_for_qt5_platform_library("platforms", "qcocoa", "Qt5Core")
relink_change_for_qt5_platform_library("platforms", "qcocoa", "Qt5Gui")
relink_change_for_qt5_platform_library("platforms", "qcocoa", "Qt5PrintSupport")
relink_change_for_qt5_platform_library("platforms", "qcocoa", "Qt5Widgets")

relink_id_for_qt5_plugin_library("printsupport", "cocoaprintersupport")
relink_change_for_qt5_platform_library("printsupport", "cocoaprintersupport", "Qt5Core")
relink_change_for_qt5_platform_library("printsupport", "cocoaprintersupport", "Qt5Gui")
relink_change_for_qt5_platform_library("printsupport", "cocoaprintersupport", "Qt5PrintSupprt")
relink_change_for_qt5_platform_library("printsupport", "cocoaprintersupport", "Qt5Widgets")

relink_id_for_qt5_plugin_library("imageformats", "qgif")
relink_change_for_qt5_platform_library("imageformats", "qgif", "Qt5Core")
relink_change_for_qt5_platform_library("imageformats", "qgif", "Qt5Gui")

relink_id_for_qt5_plugin_library("imageformats", "qjpeg")
relink_change_for_qt5_platform_library("imageformats", "qjpeg", "Qt5Core")
relink_change_for_qt5_platform_library("imageformats", "qjpeg", "Qt5Gui")

relink_id_for_library_major("libxml2", 2)

relink_id_for_library("libmpcdec")

relink_id_for_library_major("libwavpack", 1)

if isUnitTest:
    relink_id_for_library("libgtest")
    relink_id_for_library("libgtest_main")
    relink_change_library("@rpath", "libgtest", "libgtest_main")
    relink_id_for_library("libgmock")
    relink_id_for_library("libgmock_main")
    relink_change_library("@rpath", "libgmock", "libgmock_main")

relink_omega_library("libcommon", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], [])
relink_omega_library("libengine", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon"])
relink_omega_library("libblueomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libsilveromega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libblackomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libredomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libwhiteomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine", "libredomega"])
relink_omega_library("libgreenomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libtoneomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libvioletomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libcyanomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2", "libmpcdec"], ["libcommon", "libengine"])
relink_omega_library("libwavpackomega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2", "libwavpack.1"], ["libcommon", "libengine"])
relink_omega_library("libnetwork_omega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("librtp", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega"])
relink_omega_library("librtp_silveromega", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega", "librtp", "libsilveromega"])
relink_omega_library("libhttp", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega"])
relink_omega_library("libmime", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega", "libhttp"])
relink_omega_library("libaudioio", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega", "libhttp", "libmime"])
relink_omega_library("libdlna", ["Qt5Core", "Qt5Gui", "Qt5Xml"], ["libxml2.2", "libupnp.6", "libixml.2", "libthreadutil.6"], ["libcommon"])
relink_omega_library("libtrackinfo", ["Qt5Core", "Qt5Gui", "Qt5Xml"],
                     ["libxml2.2", "libupnp.6", "libmpcdec"],
                     ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega", "libvioletomega", "libcyanomega", "libdlna"])
relink_omega_library("libtrackdb", ["Qt5Core", "Qt5Gui", "Qt5Xml"],
                     ["libxml2.2", "libupnp.6", "libixml.2", "libthreadutil.6", "libmpcdec"],
                     ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega", "libvioletomega", "libcyanomega", "libtrackinfo", "libnetwork_omega", "libdlna"])
relink_omega_library("libremote", ["Qt5Core", "Qt5Gui", "Qt5Xml", "Qt5Widgets"],
                     ["libxml2.2", "libupnp.6", "libixml.2", "libthreadutil.6"],
                     ["libcommon", "libengine", "libnetwork_omega", "libdlna"])
relink_omega_library("libtrackmodel", ["Qt5Core", "Qt5Gui", "Qt5Xml"],
                     ["libxml2.2", "libupnp.6", "libixml.2", "libthreadutil.6", "libmpcdec"],
                     ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega", "libvioletomega", "libcyanomega",
                      "libtrackinfo", "libnetwork_omega", "libaudioio", "libblackomega", "libblueomega", "librtp", "librtp_silveromega",\
                      "libhttp", "libmime", "libtrackdb", "libdlna"])
relink_omega_library("libwidget", ["Qt5Core", "Qt5Gui", "Qt5Xml", "Qt5Widgets"],
                     ["libxml2.2", "libupnp.6", "libixml.2", "libthreadutil.6", "libmpcdec"],
                     ["libcommon", "libengine", "libnetwork_omega", "libhttp", "libmime", "libtrackdb", "libdlna",
                      "libtrackinfo", "libgreenomega", "libsilveromega", "libvioletomega", "libwhiteomega",
                      "libredomega", "libcyanomega"])

relink_omega_executable("Omega", ["Qt5Core", "Qt5Gui", "Qt5Xml", "Qt5Widgets"],
                        ["libxml2.2", "libupnp.6", "libwavpack.1", "libmpcdec", "libixml.2", "libthreadutil.6"],
                        ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega",
                         "libvioletomega", "libcyanomega", "libtoneomega", "libwavpackomega", "libtrackinfo", "libnetwork_omega",
                         "libaudioio", "libblackomega", "libblueomega", "librtp", "librtp_silveromega", "libhttp", "libmime",
                         "libtrackdb", "libdlna", "libremote", "libwidget"])
if isUnitTest:
    relink_omega_executable("Test", ["Qt5Core", "Qt5Gui", "Qt5Xml", "Qt5Widgets", "Qt5Test"],
                        ["libxml2.2", "libupnp.6", "libwavpack.1", "libmpcdec", "libixml.2", "libthreadutil.6",
                         "libgtest", "libgtest_main", "libgmock", "libgmock_main"],
                        ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega",
                         "libvioletomega", "libcyanomega", "libtoneomega", "libwavpackomega", "libtrackinfo", "libnetwork_omega",
                         "libaudioio", "libblackomega", "libblueomega", "librtp", "librtp_silveromega", "libhttp", "libmime",
                         "libtrackdb", "libdlna", "libremote", "libwidget", "libtrackmodel"])

# Codesigning
if isCodeSign:
    if isAppStore:
        certNameApplication = certNameStoreApplication
        certNamePackage = certNameStorePackage
    else:
        certNameApplication = certNameWebApplication
        certNamePackage = certNameWebPackage



codesign_qt5_library("Qt5Core")
codesign_qt5_library("Qt5Gui")
codesign_qt5_library("Qt5Xml")
codesign_qt5_library("Qt5Widgets")
codesign_qt5_library("Qt5Test")
codesign_qt5_library("Qt5PrintSupport")

codesign_qt5_plugin("platforms", "qcocoa")
codesign_qt5_plugin("printsupport", "cocoaprintersupport")
codesign_qt5_plugin("imageformats", "qgif")
codesign_qt5_plugin("imageformats", "qjpeg")

codesign_library_version("libixml", 2)
codesign_library_version("libthreadutil", 6)
codesign_library_version("libupnp", 6)

codesign_library_plain("libmpcdec")
codesign_library_version("libwavpack", 1)
codesign_library_version("libxml2", 2)

if isUnitTest:
    codesign_library_plain("libgtest")
    codesign_library_plain("libgtest_main")
    codesign_library_plain("libgmock")
    codesign_library_plain("libgmock_main")

codesign_library_plain("libcommon")
codesign_library_plain("libengine")
codesign_library_plain("libblueomega")
codesign_library_plain("libsilveromega")
codesign_library_plain("libblackomega")
codesign_library_plain("libredomega")
codesign_library_plain("libwhiteomega")
codesign_library_plain("libgreenomega")
codesign_library_plain("libvioletomega")
codesign_library_plain("libcyanomega")
codesign_library_plain("libtoneomega")
codesign_library_plain("libwavpackomega")
codesign_library_plain("libnetwork_omega")
codesign_library_plain("librtp")
codesign_library_plain("librtp_silveromega")
codesign_library_plain("libhttp")
codesign_library_plain("libmime")
codesign_library_plain("libaudioio")
codesign_library_plain("libtrackinfo")
codesign_library_plain("libtrackdb")
codesign_library_plain("libtrackmodel")
codesign_library_plain("libdlna")
codesign_library_plain("libremote")
codesign_library_plain("libwidget")

#codesign_exec("Omega")
#if isUnitTest:
#    codesign_exec("Test")

codesign_help()
codesign_app_bundle()

def package_and_sign():
    print("Build and sign package")
    subprocess.check_call(["productbuild", "--component", get_app_bundle_directory(), "/Applications", "--sign", certNamePackage, get_app_package()])

if isAppStore:
    package_and_sign()
