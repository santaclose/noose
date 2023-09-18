import os
import json
import random
import subprocess

def randomRotation():
	return random.random() * 3.14159265358979323846264 * 2.0

def makeImage(lal, rectangle_count):
	rectangle_count = max(rectangle_count - 1, 0)
	with open("clitest.nsj", "r") as jsonFile:
		jsonObject = json.loads(jsonFile.read())
	jsonObject["nodes"][7]["pinData"][1]["value"] = randomRotation()
	jsonObject["nodes"][8]["pinData"][2]["x"] = int(random.random() * (500 - 126))
	jsonObject["nodes"][8]["pinData"][2]["y"] = int(random.random() * (500 - 126))
	
	for i in range(rectangle_count):
		newRotateNode = {"posX": 750.558837890625, "posY": 171.098876953125 + (i + 1) * 50.0}
		newRotateNode["name"] = "Rotate"
		newRotateNode["pinData"] = [{"content": "none", "path": "none"}, {"value": randomRotation()}]
		
		newPatchNode = {"posX": 980.9580078125, "posY": 156.09877014160156 + (i + 1) * 50.0}
		newPatchNode["name"] = "Patch"
		newPatchNode["pinData"] = [{"content": "none", "path": "none"}, {"content": "none", "path": "none"}]
		newPatchNode["pinData"].append({"x": int(random.random() * (500 - 126)), "y": int(random.random() * (500 - 126))})
		
		newBlendNode = {"posX": 1201.7584228515625, "posY": 143.2988739013672 + (i + 1) * 50.0}
		newBlendNode["name"] = "Blend"
		newBlendNode["pinData"] = [{"content": "none", "path": "none"}, {"content": "none", "path": "none"}, {"value": 0}]
		
		rotateNodeIndex = len(jsonObject["nodes"])
		jsonObject["nodes"].append(newRotateNode)
		patchNodeIndex = len(jsonObject["nodes"])
		jsonObject["nodes"].append(newPatchNode)
		blendNodeIndex = len(jsonObject["nodes"])
		jsonObject["nodes"].append(newBlendNode)
		
		jsonObject["connections"].append({"nodeA": 3, "nodeB": rotateNodeIndex, "pinA": 3, "pinB": 0})
		jsonObject["connections"].append({"nodeA": 2, "nodeB": patchNodeIndex, "pinA": 2, "pinB": 0})
		jsonObject["connections"].append({"nodeA": 1, "nodeB": blendNodeIndex, "pinA": 2, "pinB": 1})
		jsonObject["connections"].append({"nodeA": rotateNodeIndex, "nodeB": patchNodeIndex, "pinA": 2, "pinB": 1})
		jsonObject["connections"].append({"nodeA": patchNodeIndex, "nodeB": blendNodeIndex, "pinA": 3, "pinB": 0})
		jsonObject["connections"].append({"nodeA": blendNodeIndex - 3, "nodeB": blendNodeIndex, "pinA": 3, "pinB": 1})

	with open("clitestOut.nsj", "w") as jsonFile:
		jsonFile.write(json.dumps(jsonObject, indent='\t'))

	subprocess.run(["../bin/Release-windows-x86_64/noose/noose.exe", "clitestOut.nsj", "-1:3"])
	os.rename("-1_3.png", f"proc{lal}.png")

for i in range(10):
	makeImage(i, i * 20)