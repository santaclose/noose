import os
import sys

if 'm' in sys.argv:
	defines = ["MACOS"]
else:
	defines = ["LINUX"]

links = "-lsfml-graphics -lsfml-window -lsfml-system"
output = "noose"

defineString = ""
first = True
for item in defines:
	if not first:
		defineString += " "
	else:
		first = False
	defineString += f"-D{item}"

os.chdir("noose")

cppFiles = os.popen('find . | grep ".*\.c\(pp\)\?$"').read().replace("\n", " ")

finalString = f"g++ -o {output} -std=c++11 {cppFiles} {links} {defineString} -O3"
print(finalString)
if 'n' not in sys.argv:
	os.system(finalString)

if 'r' in sys.argv:
	os.system("./noose")