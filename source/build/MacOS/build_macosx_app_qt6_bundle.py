import os
import shutil
import sys
import subprocess
from pathlib import Path

isDebug = False
isAppStore = False
isCodeSign = True

certNameWebApplication = "Developer ID Application: Stuart MacLean (NR9FA7GR93)"
certNameWebPackage = ""
certNameStoreApplication = "3rd Party Mac Developer Application: Stuart MacLean (NR9FA7GR93)"
certNameStorePackage = "3rd Party Mac Developer Installer: Stuart MacLean (NR9FA7GR93)"

certNameApplication = ""
certNamePackage = ""

qt6VersionMajor = 6
qt6VersionMinor = 7
qt6VersionRevision = 0

def get_root_project_directory():
    return os.path.realpath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))

def get_source_directory():
    return os.path.realpath(os.path.join(get_root_project_directory(), "source"))

def get_app_bundle_directory():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Black Omega.app"))

def get_app_package():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Black Omega.pkg"))

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

def get_plugins_resources_directory():
    return os.path.realpath(os.path.join(get_app_bundle_directory(), "Contents", "Resources"))

def get_qt6_root_directory():
	qtdirName = str(qt6VersionMajor) + "." + str(qt6VersionMinor) + "." + str(qt6VersionRevision)
	return os.path.realpath(os.path.join(Path.home(), "Qt", qtdirName))

def get_qt6_lib_directory():
	return os.path.realpath(os.path.join(get_qt6_root_directory(), "macos", "lib"))

def get_qt6_plugin_directory():
	return os.path.realpath(os.path.join(get_qt6_root_directory(), "macos", "plugins"))

def get_build_lib_path():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Omega", "lib"))

def get_build_lib_path_ucase():
    return os.path.realpath(os.path.join(get_root_project_directory(), "Build", "Omega"))

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

def copy_and_link_library(libName, major):
    print("copy " + libName + ".dylib")
    lName = libName + "." + major + ".dylib"
    srcLib = get_build_lib_path() + "/" + lName
    destLib = get_plugins_directory() + "/" + lName
    if os.path.exists(srcLib) is not True:
        print(libName + " library not found '" + srcLib + "'")
        sys.exit(-1)
    shutil.copyfile(srcLib, destLib)
    os.chmod(destLib, 0o755)
    lnLib = get_plugins_directory() + "/" + libName + ".dylib"
    os.symlink(lName, lnLib)

def copy_and_link_library_xml(libName, major):
	print("copy " + libName + ".dylib")
	lName = libName + "." + major + ".dylib"
	srcLib = get_build_lib_path() + "/" + lName
	destLib = get_plugins_directory() + "/" + lName
	if os.path.exists(srcLib) is not True:
		print(libName + " library not found '" + srcLib + "'")
		sys.exit(-1)
	shutil.copyfile(srcLib, destLib)
	os.chmod(destLib, 0o755)
	lnLib = get_plugins_directory() + "/" + libName + ".dylib"
	os.symlink(lName, lnLib)
	lnLib = get_plugins_directory() + "/" + libName + ".2.dylib"
	print(lName)
	print(lnLib)
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

def copy_and_help():
    print("Copy Black Omega.help")
    if isAppStore:
        srcHelpDir = get_source_directory() + "/help/appleStore/Black Omega.help"
    else:
        srcHelpDir = get_source_directory() + "/help/apple/Black Omega.help"
    destHelpDir = get_plugins_resources_directory() + "/Black Omega.help"
    shutil.copytree(srcHelpDir, destHelpDir)

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

def copy_executable(exeName):
    print("copy " + exeName)
    srcExec = get_build_lib_path() + "/" + exeName
    destExec = get_exec_directory() + "/" + exeName
    if os.path.exists(srcExec) is not True:
        print(exeName + " executble not found '" + srcExec + "'")
        sys.exit(-1)
    shutil.copyfile(srcExec, destExec)
    os.chmod(destExec, 0o755)

def get_qt6_framework_name(libName):
	name = libName + ".framework/Versions/A/" + libName
	return name

def get_qt6_framework_path(libName):
	name = get_frameworks_directory() + "/" + libName + ".framework/Versions/A/" + libName
	return name

def relink_id_for_qt6_framework(libName):
    print("relink " + libName)
    idPath = "@executable_path/../PlugIns/" + get_qt6_framework_name(libName)
    libPath = get_frameworks_directory() + "/" + get_qt6_framework_name(libName)
    subprocess.check_call(["install_name_tool", "-id", idPath, libPath])

def relink_change_for_qt6_framework(imprint, libName, targetLib):
    oldPath = imprint + "/" + get_qt6_framework_name(libName)
    execLibName = "@executable_path/../Frameworks/" + get_qt6_framework_name(libName)
    targetLibName = get_frameworks_directory() + "/" + get_qt6_framework_name(targetLib)
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_change_for_qt6_framework_dylib(imprint, libName, targetLib):
	oldPath = imprint + "/" + get_qt6_framework_name(libName)
	execLibName = "@executable_path/../Frameworks/" + get_qt6_framework_name(libName)
	targetLibName = get_plugins_directory() + "/" + targetLib + ".dylib"
	subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_id_for_qt6_plugin_library(pluginDir, libName):
    lName = "lib" + libName + ".dylib"
    print("relink plugin " + pluginDir + "/" + lName)
    idPath = "@executable_path/../PlugIns/" + pluginDir + "/" + lName
    libPath = get_plugins_directory() + "/" + pluginDir + "/" + lName
    subprocess.check_call(["install_name_tool", "-id", idPath, libPath])

def relink_change_for_qt6_platform_library(pluginDir, pluginName, libName):
    lName = "lib" + pluginName + ".dylib"
    oldPath = "@rpath/" + get_qt6_framework_name(libName)
    execLibName = "@executable_path/../Frameworks/" + get_qt6_framework_name(libName)
    targetLibName = get_plugins_directory() + "/" + pluginDir + "/" + lName
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_id_for_library_major(libName, major):
    print("relink " + libName)
    lName = libName + "." + major + ".dylib"
    idPath = "@executable_path/../PlugIns/" + lName
    libPath = get_plugins_directory() + "/" + lName
    subprocess.check_call(["install_name_tool", "-id", idPath, libPath])

def relink_id_for_library(libName):
    print("relink " + libName)
    lName = libName + ".dylib"
    idPath = "@executable_path/../PlugIns/" + lName
    libPath = get_plugins_directory() + "/" + lName
    subprocess.check_call(["install_name_tool", "-id", idPath, libPath])

def relink_change_library(imprint, libName, targetLib):
    oldPath = imprint + "/" + libName + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetLibName = get_plugins_directory() + "/" + targetLib + ".dylib"
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_change_library_version(imprint, libName, version, targetLib):
    oldPath = imprint + "/" + libName + "." + version + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetLibName = get_plugins_directory() + "/" + targetLib + ".dylib"
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_change_omega_library(libName, targetLib):
    oldPath = get_build_lib_path_ucase() + "/" + libName + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetLibName = get_plugins_directory() + "/" + targetLib + ".dylib"
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetLibName])

def relink_omega_library(libName, qtModules, libsArray, omegaArray):
    relink_id_for_library(libName)
    for mod in qtModules:
        relink_change_for_qt6_framework_dylib("@rpath", mod, libName)
    for lib in libsArray:
        relink_change_library("@rpath", lib, libName)
    for lib in omegaArray:
        relink_change_omega_library(lib, libName)

def relink_change_qt6_path_exec(imprint, libName, targetExec):
    oldPath = imprint + "/" + get_qt6_framework_name(libName)
    execLibName = "@executable_path/../Frameworks/" + get_qt6_framework_name(libName)
    targetExecName = get_exec_directory() + "/" + targetExec
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetExecName])

def relink_change_library_exec(imprint, libName, targetExec):
    oldPath = imprint + "/" + libName + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetExecName = get_exec_directory() + "/" + targetExec
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetExecName])

def relink_change_omega_library_exec(libName, targetExec):
    oldPath = get_build_lib_path_ucase() + "/" + libName + ".dylib"
    execLibName = "@executable_path/../PlugIns/" + libName + ".dylib"
    targetExecName = get_exec_directory() + "/" + targetExec
    #print(["install_name_tool", "-change", oldPath, execLibName, targetExecName])
    subprocess.check_call(["install_name_tool", "-change", oldPath, execLibName, targetExecName])

def relink_omega_executable(execName, qtModules, libsArray, omegaArray):
    print("relink " + execName)
    for mod in qtModules:
        relink_change_qt6_path_exec("@rpath", mod, execName)
    for lib in libsArray:
        relink_change_library_exec("@rpath", lib, execName)
    for lib in omegaArray:
        relink_change_omega_library_exec(lib, execName)

def codesign_qt6_library(libName):
    print("Signing " + libName)
    subprocess.check_call(["codesign", "-s", certNameApplication, get_qt6_framework_path(libName)])

def codesign_qt6_plugin(pluginDir, pluginName):
    print("Signing plugin " + pluginDir +"/" + pluginName)
    libName = "lib" + pluginName + ".dylib"
    destPlugin = os.path.realpath(os.path.join(get_plugins_directory(), pluginDir, libName))
    subprocess.check_call(["codesign", "-s", certNameApplication, destPlugin])

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

def package_and_sign():
    print("Build and sign package")
    subprocess.check_call(["productbuild", "--component", get_app_bundle_directory(), "/Applications", "--sign", certNamePackage, get_app_package()])

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

copy_qt6_framework("QtCore")
copy_qt6_framework("QtDBus")
copy_qt6_framework("QtGui")
copy_qt6_framework("QtXml")
copy_qt6_framework("QtWidgets")
copy_qt6_framework("QtCore5Compat")
copy_qt6_framework("QtNetwork")

copy_qt6_plugin("platforms", "qcocoa")
copy_qt6_plugin("imageformats", "qgif")
copy_qt6_plugin("imageformats", "qjpeg")

copy_and_link_library("libwavpack", "1")
copy_and_link_library_xml("libxml2", "2.13.5")

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
copy_plain_library("libremote")
copy_plain_library("libwidget")

copy_executable("Omega")

relink_id_for_qt6_framework("QtCore")

relink_id_for_qt6_framework("QtDBus")
relink_change_for_qt6_framework("@rpath", "QtCore", "QtDBus")

relink_id_for_qt6_framework("QtGui")
relink_change_for_qt6_framework("@rpath", "QtCore", "QtGui")
relink_change_for_qt6_framework("@rpath", "QtDBus", "QtGui")

relink_id_for_qt6_framework("QtCore5Compat")
relink_change_for_qt6_framework("@rpath", "QtCore", "QtCore5Compat")

relink_id_for_qt6_framework("QtNetwork")
relink_change_for_qt6_framework("@rpath", "QtCore", "QtNetwork")

relink_id_for_qt6_framework("QtWidgets")
relink_change_for_qt6_framework("@rpath", "QtCore", "QtWidgets")
relink_change_for_qt6_framework("@rpath", "QtGui", "QtWidgets")

relink_id_for_qt6_framework("QtXml")
relink_change_for_qt6_framework("@rpath", "QtCore", "QtXml")

relink_id_for_qt6_plugin_library("platforms", "qcocoa")
relink_change_for_qt6_platform_library("platforms", "qcocoa", "QtCore")
relink_change_for_qt6_platform_library("platforms", "qcocoa", "QtGui")

relink_id_for_qt6_plugin_library("imageformats", "qgif")
relink_change_for_qt6_platform_library("imageformats", "qgif", "QtCore")
relink_change_for_qt6_platform_library("imageformats", "qgif", "QtGui")

relink_id_for_qt6_plugin_library("imageformats", "qjpeg")
relink_change_for_qt6_platform_library("imageformats", "qjpeg", "QtCore")
relink_change_for_qt6_platform_library("imageformats", "qjpeg", "QtGui")

relink_id_for_library_major("libxml2", "2")
relink_id_for_library_major("libwavpack", "1")

relink_omega_library("libcommon", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], [])
relink_omega_library("libengine", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon"])

relink_omega_library("libblackomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libblueomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libsilveromega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libredomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libwhiteomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine", "libredomega"])
relink_omega_library("libgreenomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libtoneomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libvioletomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libcyanomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("libwavpackomega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2", "libwavpack.1"], ["libcommon", "libengine"])

relink_omega_library("libnetwork_omega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine"])
relink_omega_library("librtp", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega"])
relink_omega_library("librtp_silveromega", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega", "librtp", "libsilveromega"])
relink_omega_library("libhttp", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega"])
relink_omega_library("libmime", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega", "libhttp"])

relink_omega_library("libaudioio", ["QtCore", "QtGui", "QtXml"], ["libxml2.2"], ["libcommon", "libengine", "libnetwork_omega", "libhttp", "libmime"])

relink_omega_library("libtrackinfo", ["QtCore", "QtGui", "QtXml", "QtCore5Compat"], ["libxml2.2"],
                     ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega", "libvioletomega", "libcyanomega"])

relink_omega_library("libtrackdb", ["QtCore", "QtGui", "QtXml", "QtCore5Compat"], ["libxml2.2"],
                     ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega", "libvioletomega", 
                     "libcyanomega", "libtrackinfo", "libnetwork_omega", "libaudioio", "libmime", "libhttp"])

relink_omega_library("libremote", ["QtCore", "QtGui", "QtXml", "QtWidgets"], ["libxml2.2"],
                     ["libcommon", "libengine", "libnetwork_omega"])

relink_omega_library("libtrackmodel", ["QtCore", "QtGui", "QtXml", "QtCore5Compat"], ["libxml2.2"],
                     ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega", "libvioletomega", "libcyanomega",
                      "libtrackinfo", "libnetwork_omega", "libaudioio", "libblackomega", "libblueomega", "librtp", "librtp_silveromega",\
                      "libhttp", "libmime", "libtrackdb", "libdlna"])

relink_omega_library("libwidget", ["QtCore", "QtGui", "QtXml", "QtWidgets", "QtCore5Compat"], ["libxml2.2"],
                     ["libcommon", "libengine", "libnetwork_omega", "libhttp", "libmime", "libtrackdb", "libdlna",
                      "libtrackinfo", "libgreenomega", "libsilveromega", "libvioletomega", "libwhiteomega",
                      "libredomega", "libcyanomega"])

relink_omega_executable("Omega", ["QtCore", "QtGui", "QtXml", "QtWidgets", "QtCore5Compat"], ["libxml2.2", "libwavpack.1"],
                        ["libcommon", "libengine", "libsilveromega", "libredomega", "libwhiteomega", "libgreenomega",
                         "libvioletomega", "libcyanomega", "libtoneomega", "libwavpackomega", "libtrackinfo", "libnetwork_omega",
                         "libaudioio", "libblackomega", "libblueomega", "librtp", "librtp_silveromega", "libhttp", "libmime",
                         "libtrackdb", "libdlna", "libremote", "libwidget"])

# Codesigning
if isCodeSign:
    if isAppStore:
        certNameApplication = certNameStoreApplication
        certNamePackage = certNameStorePackage
    else:
        certNameApplication = certNameWebApplication
        certNamePackage = certNameWebPackage

codesign_qt6_library("QtCore")
codesign_qt6_library("QtDBus")
codesign_qt6_library("QtGui")
codesign_qt6_library("QtXml")
codesign_qt6_library("QtWidgets")
codesign_qt6_library("QtCore5Compat")
codesign_qt6_library("QtNetwork")

codesign_qt6_plugin("platforms", "qcocoa")
codesign_qt6_plugin("imageformats", "qgif")
codesign_qt6_plugin("imageformats", "qjpeg")

codesign_library_plain("libxml2.2")
codesign_library_plain("libwavpack.1")

codesign_library_plain("libaudioio")
codesign_library_plain("libblackomega")
codesign_library_plain("libblueomega")
codesign_library_plain("libcommon")
codesign_library_plain("libcyanomega")
codesign_library_plain("libengine")
codesign_library_plain("libgreenomega")
codesign_library_plain("libhttp")
codesign_library_plain("libmime")
codesign_library_plain("libnetwork_omega")
codesign_library_plain("libredomega")
codesign_library_plain("libremote")
codesign_library_plain("librtp")
codesign_library_plain("librtp_silveromega")
codesign_library_plain("libsilveromega")
codesign_library_plain("libtoneomega")
codesign_library_plain("libtrackdb")
codesign_library_plain("libtrackinfo")
codesign_library_plain("libtrackmodel")
codesign_library_plain("libvioletomega")
codesign_library_plain("libwavpackomega")
codesign_library_plain("libwhiteomega")
codesign_library_plain("libwidget")

codesign_help()
codesign_app_bundle()

if isAppStore:
    package_and_sign()
