#!/usr/bin/python3

import os
import sys

def main():
    option = getOption()
    options = sys.argv
    options.pop(0)
    options[0] = option
    launchOption(option, options)

def getOption():
    options = ["build",
            "clean",
            "countlines",
            "help",
            "regenerate",
            "stationTV",
            "ubuntu"]

    command = os.path.basename(sys.argv[0])

    option = "build" if (len(sys.argv) < 2) else sys.argv[1]

    matches = [i for i in options if i.startswith(option)]

    if len(matches) == 0:
        print(command + ": unknown command '" + sys.argv[1] +  "'")
        option = "help"
    else:
        if len(matches) == 1:
            option = matches[0]
        else:
            print(command + ": command '" + sys.argv[1] +  "' is ambiguous:")
            print("   " + (' '.join(matches)))
            exit(1)

    return option

def launchOption(option, args):
    {
        'build': notImplemented,
        'clean': notImplemented,
        'countlines': notImplemented,
        'help': notImplemented,
        'regenerate': regenerate,
        'stationTV': notImplemented,
        'ubuntu': notImplemented,
        'help': showHelp,
    }[option](args)

def notImplemented(args):
    print(args[0] + ": not implemented")
    pass

def regenerate(args):
    from scripts.buildRegenerate import buildRegenerate
    buildRegenerate()

def showHelp(args):
    print("Build")
    print()
    print("basic commands:")
    print()
    print(" build         build projects (default)")
    print(" clean         clean projects")
    print(" countlines    count all project lines")
    print(" help          display this help and exit")
    print(" regenerate    regenerate cmake projects and build")
    print(" stationTV     launch stationTV")
    print(" ubuntu        Check Ubuntu development packages")
    print()
    exit(1)

if __name__ == "__main__":
    main()