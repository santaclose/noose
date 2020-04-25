import os

defines = ["LINUX", "TEST"]
links = "-lsfml-graphics -lsfml-window -lsfml-system"
output = "test"

defineString = ""
first = True
for item in defines:
	if not first:
		defineString += " "
	else:
		first = False
	defineString += f"-D{item}"

##########################
## logic component test ##
##########################

cppFiles = "../noose/nodeProvider/nodeFunctionality.cpp ../noose/nodeProvider/nodeProvider.cpp ../noose/logic/connectionSystem.cpp ../noose/logic/graphOperations.cpp ../noose/logic/node.cpp ../noose/logic/nodeSystem.cpp logicComponentTest.cpp"

os.chdir("test")
finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w"
print(finalString)
os.system(finalString)
os.chdir("../noose/")
os.system("../test/test")