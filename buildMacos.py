import os

defines = ["MACOS"]
links = "-lsfml-graphics -lsfml-window -lsfml-system"
output = "noose"
cppFiles = os.popen("find . | grep cpp").read().replace("\n", " ")

defineString = ""
first = True
for item in defines:
	if not first:
		defineString += " "
	else:
		first = False
	defineString += f"-D{item}"

finalString = f"g++ -o {output} -std=c++11 {cppFiles} {links} {defineString} -O3"
print(finalString)
os.system(finalString)
#os.system("cd noose && ./noose")