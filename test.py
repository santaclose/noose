import os
import time
import sys

defines = ["LINUX", "TEST"]
links = "-lsfml-graphics -lsfml-window -lsfml-system -lgtest -pthread"
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

print('---------------------------')
print('-- graph operations test --')
print('---------------------------')

cppFiles = "../noose/nodeProvider/nodeFunctionality.cpp ../noose/nodeProvider/nodeProvider.cpp ../noose/logic/connectionSystem.cpp ../noose/logic/graphOperations.cpp ../noose/logic/node.cpp ../noose/logic/nodeSystem.cpp graphOperationsTest.cpp"

finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w"
os.system(finalString)
os.system("./test")

print('--------------------------')
print('--  node provider test  --')
print('--------------------------')

cppFiles = "../noose/nodeProvider/nodeFunctionality.cpp ../noose/nodeProvider/nodeProvider.cpp ../noose/logic/connectionSystem.cpp ../noose/logic/graphOperations.cpp ../noose/logic/node.cpp ../noose/logic/nodeSystem.cpp nodeProviderTest.cpp"

finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w"
os.system(finalString)
os.system("./test")

print('--------------------------')
print('--    searcher test     --')
print('--------------------------')

cppFiles = "../noose/nodeProvider/nodeFunctionality.cpp ../noose/nodeProvider/nodeProvider.cpp ../noose/logic/connectionSystem.cpp ../noose/logic/graphOperations.cpp ../noose/logic/node.cpp ../noose/logic/nodeSystem.cpp ../noose/searcher.cpp searcherTest.cpp"

finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w"
os.system(finalString)
os.system("./test")

print('--------------------------')
print('-- logic component test --')
print('--------------------------')

cppFiles = "../noose/nodeProvider/nodeFunctionality.cpp ../noose/nodeProvider/nodeProvider.cpp ../noose/logic/connectionSystem.cpp ../noose/logic/graphOperations.cpp ../noose/logic/node.cpp ../noose/logic/nodeSystem.cpp logicComponentTest.cpp"

finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w"
os.system(finalString)
os.system("./test")



'''
'''
'''
'''



if 'ui' not in sys.argv:
	exit()

print('--------------------------')
print('-- ui node system test  --')
print('--------------------------')

os.chdir("../noose/")
cppFiles = " ".join(["../noose" + file[1:] for file in os.popen("find . | grep cpp").read().split("\n") if len(file) > 0])
os.chdir("../test/")
finalString = f"g++ -o {output} {cppFiles} {links} {defineString} -w -DTEST"
os.system(finalString)
os.system("./test")