#!/usr/bin/env python3
import sys
import subprocess
import shutil
import os

build_dir = 'build_linux'


def print_usage():
    print("Usage:\n"
          "[required] argv[1] build type(release/debug)\n"
          "[required] argv[2] license key\n"
          "[optional] argv[3] license algo\n")


if __name__ == "__main__":
    argc = len(sys.argv)

    if argc > 1:
        build_type = sys.argv[1]
    else:
        print_usage()
        sys.exit(1)

    if argc > 2:
        license_key = sys.argv[2]
    else:
        print_usage()
        sys.exit(1)

    license_algo = 0
    if argc > 3:
        license_algo = sys.argv[3]

    pwd = os.getcwd()

    abs_dir_path = os.path.abspath(build_dir)
    if os.path.exists(abs_dir_path):
        shutil.rmtree(abs_dir_path)

    os.mkdir(abs_dir_path)
    os.chdir(build_dir)
    subprocess.call(['cmake', '../../', '-GNinja', '-DCMAKE_BUILD_TYPE={0}'.format(build_type),
                     '-DCMAKE_INSTALL_PREFIX=/usr/local', '-DLICENSE_KEY={0}'.format(license_key),
                     '-DHARDWARE_LICENSE_ALGO=0'.format(license_algo),
                     '-DJSONC_USE_STATIC=ON'])
    subprocess.call(['ninja', 'install'])
    os.chdir(pwd)
