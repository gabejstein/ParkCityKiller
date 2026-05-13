#Script to export entity location data.
#Entities should be stored in a collection called 'Entity'
#This script will read all objects in that collection with a custom data property of 'class'
#This is mostly experimental right now, so use with caution.

#TODO: Add other meta-data like model filename, music, etc.

import bpy
import os
import struct

def WriteVec3(file,v):
    file.write(struct.pack('f',v[0]))
    file.write(struct.pack('f',v[2]))
    file.write(struct.pack('f',v[1]))
    
def WriteString(file,s):
    file.write(struct.pack('I',len(s)))
    file.write(s.encode('utf-8'))

magicNum = 0x48534F42 #B-O-S-H

baseDir = os.path.dirname(bpy.data.filepath)

fileExt = ".level"

levelName = "default"

entityCollection = bpy.data.collections["Entity"]

entityCount = 0
spawnPointCount = 0
portalCount = 0

#Preprocessing pass
for obj in entityCollection.objects:
    if "class" in obj:
        entityCount +=1
        if obj["class"]=="portal":
            portalCount +=1
        elif obj["class"]=="spawn_point":
            spawnPointCount +=1
    if obj.name=="meta_data":
        levelName = obj["level_name"]     
        
fileName = baseDir+"\\"+levelName+fileExt
        
        
print(str(entityCount) + " entities found!")

file = open(fileName,"wb")

file.write(struct.pack('I',magicNum))
file.write(struct.pack('I',entityCount))
file.write(struct.pack('I',portalCount))
file.write(struct.pack('I',spawnPointCount))


for obj in entityCollection.objects:
    if "class" in obj:
        #save the actual type of object
        WriteString(file,obj["class"]) 
        
        #position
        file.write(struct.pack('f',obj.location[0]))
        file.write(struct.pack('f',obj.location[2])) #Changing the order for Raylib
        file.write(struct.pack('f',obj.location[1]*-1))
        
        #rotation
        file.write(struct.pack('f',obj.rotation_euler[2])) #exports in radians
        
        #special data
        if obj["class"]=="npc":
            WriteString(file,"blue_guy")
        elif obj["class"]=="spawn_point":
            if "id" not in obj:
                print("Spawn point has no id.")
                break
            WriteString(file,obj["id"])
        elif obj["class"]=="portal":
            WriteString(file,obj["level_id"])
            WriteString(file,obj["spawn_id"])
            WriteVec3(file,obj.dimensions) #bounding box
            
        
file.close()

print("Entity file successfully created!")