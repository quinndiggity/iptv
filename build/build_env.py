#!/usr/bin/env python3
import argparse
import os
import shutil
import subprocess
import sys

from pyfastogt import system_info, utils

GLIB_SRC_ROOT = "http://ftp.acc.umu.se/pub/gnome/sources/glib"
GLIB_ARCH_COMP = "xz"
GLIB_ARCH_EXT = "tar." + GLIB_ARCH_COMP

GSTREAMER_SRC_ROOT = "https://gstreamer.freedesktop.org/src/"
GSTREAMER_ARCH_COMP = "xz"
GSTREAMER_ARCH_EXT = "tar." + GSTREAMER_ARCH_COMP

GST_PLUGINS_BASE_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_PLUGINS_BASE_ARCH_COMP = "xz"
GST_PLUGINS_BASE_ARCH_EXT = "tar." + GST_PLUGINS_BASE_ARCH_COMP

GST_PLUGINS_GOOD_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_PLUGINS_GOOD_ARCH_COMP = "xz"
GST_PLUGINS_GOOD_ARCH_EXT = "tar." + GST_PLUGINS_GOOD_ARCH_COMP

GST_PLUGINS_BAD_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_PLUGINS_BAD_ARCH_COMP = "xz"
GST_PLUGINS_BAD_ARCH_EXT = "tar." + GST_PLUGINS_BAD_ARCH_COMP

GST_PLUGINS_UGLY_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_PLUGINS_UGLY_ARCH_COMP = "xz"
GST_PLUGINS_UGLY_ARCH_EXT = "tar." + GST_PLUGINS_UGLY_ARCH_COMP

GST_LIBAV_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_LIBAV_ARCH_COMP = "xz"
GST_LIBAV_ARCH_EXT = "tar." + GST_LIBAV_ARCH_COMP

g_script_path = os.path.realpath(sys.argv[0])


class BuildRequest(object):
    def __init__(self, platform, arch_name, dir_path, prefix_path):
        platform_or_none = system_info.get_supported_platform_by_name(platform)

        if not platform_or_none:
            raise utils.BuildError('invalid platform')

        build_arch = platform_or_none.architecture_by_arch_name(arch_name)
        if not build_arch:
            raise utils.BuildError('invalid arch')

        if not prefix_path:
            prefix_path = build_arch.default_install_prefix_path()

        packages_types = platform_or_none.package_types()
        build_platform = platform_or_none.make_platform_by_arch(build_arch, packages_types)

        self.platform_ = build_platform
        build_dir_path = os.path.abspath(dir_path)
        if os.path.exists(build_dir_path):
            shutil.rmtree(build_dir_path)

        os.mkdir(build_dir_path)
        os.chdir(build_dir_path)

        self.build_dir_path_ = build_dir_path
        self.prefix_path_ = prefix_path
        print("Build request platform: {0}({1}) created".format(build_platform.name(), build_arch.name()))

    def build(self, url, compiler_flags: utils.CompileInfo, executable='./configure'):
        utils.build_from_sources(url, compiler_flags, g_script_path, self.prefix_path_, executable)

    def get_system_libs(self):
        platform = self.platform_
        platform_name = platform.name()
        dep_libs = []

        if platform_name == 'linux':
            distribution = system_info.linux_get_dist()
            if distribution == 'DEBIAN':
                dep_libs = ['gcc', 'g++', 'git', 'make', 'cmake', 'autoconf', 'libtool', 'pkg-config', 'gettext',
                            'libssl-dev',
                            'libcairo2-dev',
                            'libmount-dev', 'libdrm-dev', 'libudev-dev', 'libjpeg-dev', 'freeglut3-dev',
                            'libegl1-mesa-dev',
                            'zlib1g-dev', 'libffi-dev', 'yasm', 'bison', 'flex', 'libxrandr-dev', 'libfaac-dev',
                            'libfaad-dev',
                            'ninja-build', 'libsqlite3-dev', 'intltool', 'liborc-0.4-dev', 'libxml2-dev', 'libx264-dev',
                            'glib-networking',
                            'libmp3lame-dev']
            elif distribution == 'RHEL':
                dep_libs = ['gcc', 'gcc-c++', 'git', 'make', 'cmake', 'autoconf', 'libtool', 'cairo-gobject-devel',
                            'libmount-devel',
                            'libpciaccess-devel', 'libxcb-devel', 'libdrm-devel', 'libsoup-devel', 'libx264-devel',
                            'libudev-devel', 'libjpeg-turbo-devel', 'zlib-devel', 'libffi-devel', 'pcre-devel', 'yasm',
                            'bison', 'flex', 'alsa-lib-devel', 'ninja-build', 'patch', 'lame-devel']
        else:
            raise NotImplemented("Unknown platform '%s'" % platform_name)

        return dep_libs

    def install_system(self):
        platform = self.platform_
        dep_libs = self.get_system_libs()
        for lib in dep_libs:
            platform.install_package(lib)

        # post install step
        platform_name = platform.name()
        if platform_name == 'linux':
            distribution = system_info.linux_get_dist()
            if distribution == 'RHEL':
                subprocess.call(['ln', '-sf', '/usr/bin/ninja-build', '/usr/bin/ninja'])

    def build_jsonc(self):
        jsonc_compiler_flags = utils.CompileInfo([], ['--with-pic', '--disable-shared', '--enable-static'])

        pwd = os.getcwd()
        cloned_dir = utils.git_clone('https://github.com/fastogt/json-c.git', pwd)
        os.chdir(cloned_dir)

        autogen_jsonc = ['sh', 'autogen.sh']
        subprocess.call(autogen_jsonc)

        utils.build_command_configure(jsonc_compiler_flags, g_script_path, self.prefix_path_)
        os.chdir(pwd)
        #shutil.rmtree(cloned_dir)

    def build_hiredis(self):
        pwd = os.getcwd()
        cloned_dir = utils.git_clone('https://github.com/fastogt/hiredis.git', pwd)
        os.chdir(cloned_dir)

        make_hiredis = ['make', 'PREFIX={0}'.format(self.prefix_path_), 'install']
        subprocess.call(make_hiredis)
        os.chdir(pwd)
        #shutil.rmtree(cloned_dir)

    def build_libev(self):
        libev_compiler_flags = utils.CompileInfo([], ['--with-pic', '--disable-shared', '--enable-static'])

        pwd = os.getcwd()
        cloned_dir = utils.git_clone('https://github.com/fastogt/libev.git', pwd)
        os.chdir(cloned_dir)

        autogen_libev = ['sh', 'autogen.sh']
        subprocess.call(autogen_libev)

        utils.build_command_configure(libev_compiler_flags, g_script_path, self.prefix_path_)
        os.chdir(pwd)
        #shutil.rmtree(cloned_dir)

    def build_common(self):
        pwd = os.getcwd()
        cmake_project_root_abs_path = '..'
        if not os.path.exists(cmake_project_root_abs_path):
            raise utils.BuildError('invalid cmake_project_root_path: %s' % cmake_project_root_abs_path)

        # project static options
        prefix_args = '-DCMAKE_INSTALL_PREFIX={0}'.format(self.prefix_path_)

        cmake_line = ['cmake', cmake_project_root_abs_path, '-GUnix Makefiles', '-DCMAKE_BUILD_TYPE=RELEASE',
                      prefix_args]
        try:
            cloned_dir = utils.git_clone('https://github.com/fastogt/common.git', pwd)
            os.chdir(cloned_dir)

            os.mkdir('build_cmake_release')
            os.chdir('build_cmake_release')
            common_cmake_line = list(cmake_line)
            common_cmake_line.append('-DQT_ENABLED=OFF')
            common_cmake_line.append('-DJSON_ENABLED=ON')
            common_cmake_line.append('-DBUILD_WITH_FPIC=ON')
            subprocess.call(common_cmake_line)
            subprocess.call(['make', 'install'])
            os.chdir(self.build_dir_path_)
            #shutil.rmtree(cloned_dir)
        except Exception as ex:
            os.chdir(self.build_dir_path_)
            raise ex

    def build_glib(self, version):
        glib_version_short = version[:version.rfind('.')]
        compiler_flags = utils.CompileInfo([], [])
        self.build('{0}/{1}/glib-{2}.{3}'.format(GLIB_SRC_ROOT, glib_version_short, version, GLIB_ARCH_EXT),
                   compiler_flags)

    def build_gstreamer(self, version):
        compiler_flags = utils.CompileInfo([], ['--disable-debug'])
        self.build('{0}gstreamer/gstreamer-{1}.{2}'.format(GSTREAMER_SRC_ROOT, version, GSTREAMER_ARCH_EXT),
                   compiler_flags)

    def build_gst_plugins_base(self, version):
        compiler_flags = utils.CompileInfo([], ['--disable-debug'])
        self.build('{0}gst-plugins-base/gst-plugins-base-{1}.{2}'.format(GST_PLUGINS_BASE_SRC_ROOT, version,
                                                                         GST_PLUGINS_BASE_ARCH_EXT),
                   compiler_flags)

    def build_gst_plugins_good(self, version):
        compiler_flags = utils.CompileInfo([], ['--disable-debug'])
        self.build('{0}gst-plugins-good/gst-plugins-good-{1}.{2}'.format(GST_PLUGINS_GOOD_SRC_ROOT, version,
                                                                         GST_PLUGINS_GOOD_ARCH_EXT),
                   compiler_flags)

    def build_gst_plugins_bad(self, version):
        compiler_flags = utils.CompileInfo([], ['--disable-debug'])
        self.build('{0}gst-plugins-bad/gst-plugins-bad-{1}.{2}'.format(GST_PLUGINS_BAD_SRC_ROOT, version,
                                                                       GST_PLUGINS_BAD_ARCH_EXT),
                   compiler_flags)

    def build_gst_plugins_ugly(self, version):
        compiler_flags = utils.CompileInfo([], ['--disable-debug'])
        self.build('{0}gst-plugins-ugly/gst-plugins-ugly-{1}.{2}'.format(GST_PLUGINS_UGLY_SRC_ROOT, version,
                                                                         GST_PLUGINS_UGLY_ARCH_EXT),
                   compiler_flags)

    def build_gst_libav(self, version):
        compiler_flags = utils.CompileInfo([], ['--disable-debug'])
        self.build('{0}gst-libav/gst-libav-{1}.{2}'.format(GST_LIBAV_SRC_ROOT, version, GST_LIBAV_ARCH_EXT),
                   compiler_flags)


if __name__ == "__main__":
    glib_default_version = '2.58.1'
    gstreamer_default_version = '1.14.4'
    gst_plugins_base_default_version = gstreamer_default_version
    gst_plugins_good_default_version = gstreamer_default_version
    gst_plugins_bad_default_version = gstreamer_default_version
    gst_plugins_ugly_default_version = gstreamer_default_version
    gst_libav_default_version = gstreamer_default_version

    host_os = system_info.get_os()
    arch_host_os = system_info.get_arch_name()

    parser = argparse.ArgumentParser(prog='build_env', usage='%(prog)s [options]')
    # system
    system_grp = parser.add_mutually_exclusive_group()
    system_grp.add_argument('--with-system', help='build with system dependencies (default)', dest='with_system',
                            action='store_true', default=True)
    system_grp.add_argument('--without-system', help='build without system dependencies', dest='with_system',
                            action='store_false', default=False)

    # hiredis
    hiredis_grp = parser.add_mutually_exclusive_group()
    hiredis_grp.add_argument('--with-hiredis', help='build hiredis (default, version: git master)', dest='with_hiredis',
                             action='store_true', default=True)
    hiredis_grp.add_argument('--without-hiredis', help='build without hiredis', dest='with_hiredis',
                             action='store_false',
                             default=False)

    # json-c
    jsonc_grp = parser.add_mutually_exclusive_group()
    jsonc_grp.add_argument('--with-json-c', help='build json-c (default, version: git master)', dest='with_jsonc',
                           action='store_true', default=True)
    jsonc_grp.add_argument('--without-json-c', help='build without json-c', dest='with_jsonc', action='store_false',
                           default=False)

    # libev
    libev_grp = parser.add_mutually_exclusive_group()
    libev_grp.add_argument('--with-libev-c', help='build libev (default, version: git master)', dest='with_libev',
                           action='store_true', default=True)
    libev_grp.add_argument('--without-libev-c', help='build without libev', dest='with_libev', action='store_false',
                           default=False)

    # common
    common_grp = parser.add_mutually_exclusive_group()
    common_grp.add_argument('--with-common', help='build common (default, version: git master)', dest='with_common',
                            action='store_true', default=True)
    common_grp.add_argument('--without-common', help='build without common', dest='with_common',
                            action='store_false',
                            default=False)

    # glib
    glib_grp = parser.add_mutually_exclusive_group()
    glib_grp.add_argument('--with-glib', help='build glib (default, version:{0})'.format(glib_default_version),
                          dest='with_glib', action='store_true', default=True)
    glib_grp.add_argument('--without-glib', help='build without glib', dest='with_glib', action='store_false',
                          default=False)
    parser.add_argument('--glib-version', help='glib version (default: {0})'.format(glib_default_version),
                        default=glib_default_version)

    # gstreamer
    gstreamer_grp = parser.add_mutually_exclusive_group()
    gstreamer_grp.add_argument('--with-gstreamer',
                               help='build gstreamer (default, version:{0})'.format(gstreamer_default_version),
                               dest='with_gstreamer', action='store_true', default=True)
    gstreamer_grp.add_argument('--without-gstreamer', help='build without gstreamer', dest='with_gstreamer',
                               action='store_false',
                               default=False)
    parser.add_argument('--gstreamer-version',
                        help='gstreamer version (default: {0})'.format(gstreamer_default_version),
                        default=gstreamer_default_version)

    # gst-plugins-base
    gst_plugins_base_grp = parser.add_mutually_exclusive_group()
    gst_plugins_base_grp.add_argument('--with-gst-plugins-base',
                                      help='build gst-plugins-base (default, version:{0})'.format(
                                          gst_plugins_base_default_version),
                                      dest='with_gst_plugins_base', action='store_true', default=True)
    gst_plugins_base_grp.add_argument('--without-gst-plugins-base', help='build without gst-plugins-base',
                                      dest='with_gst_plugins_base',
                                      action='store_false',
                                      default=False)
    parser.add_argument('--gst-plugins-base-version',
                        help='gst-plugins-base version (default: {0})'.format(gst_plugins_base_default_version),
                        default=gst_plugins_base_default_version)

    # gst-plugins-good
    gst_plugins_good_grp = parser.add_mutually_exclusive_group()
    gst_plugins_good_grp.add_argument('--with-gst-plugins-good',
                                      help='build gst-plugins-good (default, version:{0})'.format(
                                          gst_plugins_good_default_version),
                                      dest='with_gst_plugins_good', action='store_true', default=True)
    gst_plugins_good_grp.add_argument('--without-gst-plugins-good', help='build without gst-plugins-good',
                                      dest='with_gst_plugins_good',
                                      action='store_false',
                                      default=False)
    parser.add_argument('--gst-plugins-good-version',
                        help='gst-plugins-good version (default: {0})'.format(gst_plugins_good_default_version),
                        default=gst_plugins_good_default_version)

    # gst-plugins-bad
    gst_plugins_bad_grp = parser.add_mutually_exclusive_group()
    gst_plugins_bad_grp.add_argument('--with-gst-plugins-bad',
                                     help='build gst-plugins-bad (default, version:{0})'.format(
                                         gst_plugins_bad_default_version),
                                     dest='with_gst_plugins_bad', action='store_true', default=True)
    gst_plugins_bad_grp.add_argument('--without-gst-plugins-bad', help='build without gst-plugins-bad',
                                     dest='with_gst_plugins_bad',
                                     action='store_false',
                                     default=False)
    parser.add_argument('--gst-plugins-bad-version',
                        help='gst-plugins-bad version (default: {0})'.format(gst_plugins_bad_default_version),
                        default=gst_plugins_bad_default_version)

    # gst-plugins-ugly
    gst_plugins_ugly_grp = parser.add_mutually_exclusive_group()
    gst_plugins_ugly_grp.add_argument('--with-gst-plugins-ugly',
                                      help='build gst-plugins-ugly (default, version:{0})'.format(
                                          gst_plugins_ugly_default_version),
                                      dest='with_gst_plugins_ugly', action='store_true', default=True)
    gst_plugins_ugly_grp.add_argument('--without-gst-plugins-ugly', help='build without gst-plugins-ugly',
                                      dest='with_gst_plugins_ugly',
                                      action='store_false',
                                      default=False)
    parser.add_argument('--gst-plugins-ugly-version',
                        help='gst-plugins-ugly version (default: {0})'.format(gst_plugins_ugly_default_version),
                        default=gst_plugins_ugly_default_version)

    # gst-libav
    gst_libav_grp = parser.add_mutually_exclusive_group()
    gst_libav_grp.add_argument('--with-gst-libav',
                               help='build gst-libav (default, version:{0})'.format(
                                   gst_libav_default_version),
                               dest='with_gst_libav', action='store_true', default=True)
    gst_libav_grp.add_argument('--without-gst-libav', help='build without gst-libav',
                               dest='with_gst_libav',
                               action='store_false',
                               default=False)
    parser.add_argument('--gst-libav-version',
                        help='gst-libav version (default: {0})'.format(gst_libav_default_version),
                        default=gst_libav_default_version)

    # other
    parser.add_argument('--platform', help='build for platform (default: {0})'.format(host_os), default=host_os)
    parser.add_argument('--architecture', help='architecture (default: {0})'.format(arch_host_os),
                        default=arch_host_os)
    parser.add_argument('--prefix_path', help='prefix_path (default: None)', default=None)

    argv = parser.parse_args()

    arg_platform = argv.platform
    arg_prefix_path = argv.prefix_path
    arg_architecture = argv.architecture

    request = BuildRequest(arg_platform, arg_architecture, 'build_' + arg_platform + '_env',
                           arg_prefix_path)
    if argv.with_system:
        request.install_system()

    if argv.with_hiredis:
        request.build_hiredis()
    if argv.with_jsonc:
        request.build_jsonc()
    if argv.with_libev:
        request.build_libev()
    if argv.with_common:
        request.build_common()

    if argv.with_glib:
        request.build_glib(argv.glib_version)

    if argv.with_gstreamer:
        request.build_gstreamer(argv.gstreamer_version)

    if argv.with_gst_plugins_base:
        request.build_gst_plugins_base(argv.gst_plugins_base_version)

    if argv.with_gst_plugins_good:
        request.build_gst_plugins_good(argv.gst_plugins_good_version)

    if argv.with_gst_plugins_bad:
        request.build_gst_plugins_bad(argv.gst_plugins_bad_version)

    if argv.with_gst_plugins_ugly:
        request.build_gst_plugins_ugly(argv.gst_plugins_ugly_version)

    if argv.with_gst_libav:
        request.build_gst_libav(argv.gst_libav_version)
