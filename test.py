import os
import time

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

os.chdir("test")
print('--------------------------')
print('--  node provider test  --')
print('--------------------------')

cppFiles = "../noose/nodeProvider/nodeFunctionality.cpp ../noose/nodeProvider/nodeProvider.cpp ../noose/logic/connectionSystem.cpp ../noose/logic/graphOperations.cpp ../noose/logic/node.cpp ../noose/logic/nodeSystem.cpp nodeProviderTest.cpp"

finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w"
os.system(finalString)
# stay in the test directory to use the same nodes.dat every time
os.system("./test")

print('--------------------------')
print('-- logic component test --')
print('--------------------------')

cppFiles = "../noose/nodeProvider/nodeFunctionality.cpp ../noose/nodeProvider/nodeProvider.cpp ../noose/logic/connectionSystem.cpp ../noose/logic/graphOperations.cpp ../noose/logic/node.cpp ../noose/logic/nodeSystem.cpp logicComponentTest.cpp"

finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w"
os.system(finalString)
os.chdir("../noose/")
os.system("../test/test")
os.chdir("../test/")

print('--------------------------')
print('-- ui node system test  --')
print('--------------------------')

os.chdir("../noose/")
cppFiles = " ".join(["../noose" + file[1:] for file in os.popen("find . | grep cpp").read().split("\n") if "main.cpp" not in file and len(file) > 0]) + " uiNodeSystemTest.cpp"
os.chdir("../test/")
finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w"
os.system(finalString)
os.chdir("../noose/")
#
os.system("../test/test&")
time.sleep(5.0)
#os.system("cnee --record --mouse --keyboard -o ../test/uiNodeSystemTest.xns -sk q")
os.system("cnee --replay -f ../test/uiNodeSystemTest.xns --speed-percent 80")
#
os.chdir("../test/")