#!/usr/bin/env python

import argparse
import sys
import os
import platform
import errno
import shutil
import subprocess


def is_win():
    return "Windows" in platform.system()


def is_linux():
    return "Linux" in platform.system()


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Genarate project files based platform etc.")

    if is_win():
        parser.add_argument("target", choices=["win",  "win64"], help="Special the target that build for.")
    elif is_linux():
        parser.add_argument("target", choices=["linux"], help="Special the target that build for.")
    else:
        parser.add_argument("target", choices=["win",  "win64", "linux"], help="Special the target that build for.")
    parser.add_argument("--folder", default="build", help="The build folder, relative path of project directory.Default is\"./build\".")
    parser.add_argument("--build", action="store_true", default=False, help="Build after project files genarated.")
    parser.add_argument("--release", action="store_true", default=False, help="Enable release build.")

    args = parser.parse_args()

    if "--help" in sys.argv or "-h" in sys.argv:
        parser.print_help()
        sys.exit()

    try:
        media_node_type = ""
        options = ""
        build_path = os.path.join(os.getcwd(), args.folder)

        # config generater
        if args.target == "win":
            options += " -G \"Visual Studio 15 2017\""
        elif args.target == "win64":
            options += " -G \"Visual Studio 15 2017 Win64\""
        elif args.target == "linux":
            options += " -G \"Unix Makefiles\""
        else:
            pass

        # config other options
        build_type = "Release" if args.release else "Debug"
        options += " -DCMAKE_BUILD_TYPE=%s" % build_type

        mkdir_p(build_path)

        cmds = ["cmake %s %s" % (os.getcwd(), options)]
        if args.build:
            if args.target == "linux":
                cmds.append("make -j `nproc`")
            else:
                cmds.append("cmake --build %s --config %s" % (build_path, build_type))

        for cmd in cmds:
            print("run command:", cmd)
            rlt = subprocess.call(cmd, shell=True, cwd=build_path)
            if rlt != 0:
                print("Error, stop")
                sys.exit(rlt)

        if args.build:
            shutil.copy("resource/video_1280x720.yuv", "%s/bin" % args.folder)
            shutil.copy("resource/weiruanyahei.ttf", "%s/bin" % args.folder)

        sys.exit(0)

    except OSError as e:
        print("Error: %s" % str(e))
        sys.exit(1)
