#Script to export entity location data.
#Entities should be stored in a collection called 'Entity'
#This script will read all objects in that collection with a custom data property of 'class'
#This is mostly experimental right now, so use with caution.

#TODO: Create a magic number
#TODO: Add other meta-data like model filename, music, etc.

import bpy
import os
import struct

baseDir = os.path.dirname(bpy.data.filepath)

fileName = baseDir+"\\test.edata"

entityCollection = bpy.data.collections["Entity"]

entityCount = 0

for obj in entityCollection.objects:
    if(obj["class"]):
        entityCount +=1
        
print(str(entityCount) + " entities found!")

file = open(fileName,"wb")

file.write(struct.pack('I',entityCount))

for obj in entityCollection.objects:
    if(obj["class"]):
        file.write(struct.pack('I',len(obj["class"])))
        data = obj["class"].encode('utf-8')
        file.write(data)
        
        #position
        file.write(struct.pack('f',obj.location[0]))
        file.write(struct.pack('f',obj.location[2])) #Changing the order for Raylib
        file.write(struct.pack('f',obj.location[1]*-1))
        
        #rotation
        file.write(struct.pack('f',obj.rotation_euler[2]))
        
file.close()

print("Entity file successfully saved!")