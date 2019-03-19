import errno
import os
import subprocess

def buildRegenerate():
    newPath = os.path.join("b", "r", "libawui")
    if not os.path.isdir(newPath):
        os.makedirs(newPath)
    os.chdir(newPath)

    removeFile("CMakeCache.txt")
    path = os.path.join("..", "..", "..", "libawui")
    subprocess.run(["cmake", "-GNinja", "-DCMAKE_BUILD_TYPE=Release", path])
    subprocess.run(["ninja"])
    #($COMMAND_BUILD)
    os.chdir(os.path.join("..", "..", ".."))

    #os.chdir("samples")
    #removeFile("CMakeCache.txt")
    #subprocess.run(["cmake", "-GNinja", "-DCMAKE_BUILD_TYPE=Release", "."])
    #($COMMAND_BUILD)
    #os.chdir("..")


def removeFile(filename):
    try:
        os.remove(filename)
    except OSError as e: # this would be "except OSError, e:" before Python 2.6
        if e.errno != errno.ENOENT: # errno.ENOENT = no such file or directory
            raise