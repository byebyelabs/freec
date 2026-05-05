import argparse
import glob
import os
import subprocess

CC = os.getenv("CC")
CFLAGS = os.getenv("CFLAGS")
BUILD_DIR = os.getenv("BUILD_DIR")
LIB = os.getenv("LIB")


def main():
    p = argparse.ArgumentParser()
    p.add_argument("pattern", help="e.g. 'tests/use_after_free_*'")
    args = p.parse_args()

    files = glob.glob(args.pattern)
    for src in files:
        sli = src.rfind("/")  # last slash index
        dst = (f"{src[:sli]}/exe_{src[sli + 1 :]}").replace(".c", "")
        command = f"{CC} {CFLAGS} -o {dst} {src}"

        # compile
        print(command)
        subprocess.run(command, shell=True)

        # run the executable
        command = f"LD_PRELOAD=./{BUILD_DIR}/{LIB}.so ./{dst}"
        print(command)
        subprocess.run(command, shell=True)

        # print padding line
        print()


if __name__ == "__main__":
    main()
