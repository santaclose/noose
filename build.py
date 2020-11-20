import os
import sys

if 'm' in sys.argv:
	defines = ["NOOSE_PLATFORM_MACOS"]
else:
	defines = ["NOOSE_PLATFORM_LINUX"]

links = "-lsfml-graphics -lsfml-window -lsfml-system"
includes = ["src", "vendor/nativefiledialog/src"]
additionalFilesToCompile = ["vendor/nativefiledialog/src/nfd_common.c", "vendor/nativefiledialog/src/nfd_zenity.c"]
output = "noose"

# build define string
defineString = ""
first = True
for item in defines:
	if not first:
		defineString += " "
	else:
		first = False
	defineString += f"-D{item}"

# build include string
includeString = ""
first = True
for item in includes:
	if not first:
		includeString += " "
	else:
		first = False
	includeString += f"-I {item}"

#os.chdir("src")

cppFiles = os.popen('find src | grep ".*\.c\(pp\)\?$"').read().replace("\n", " ")

finalString = f"g++ -o {output} -std=c++11 {includeString} {cppFiles} {' '.join(additionalFilesToCompile)} {links} {defineString} -O3 -fpermissive"
print(finalString)
if 'n' not in sys.argv:
	os.system(finalString)

if 'r' in sys.argv:
	os.system("./noose")