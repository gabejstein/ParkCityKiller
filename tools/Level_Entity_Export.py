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
    file.write(struct.pack('f',v[1]*-1)) #Changing the order for Raylib
    
def WriteBoundingBox(file,v):
    file.write(struct.pack('f',v[0]))
    file.write(struct.pack('f',v[2]))
    file.write(struct.pack('f',v[1])) #Changing the order for Raylib
    
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
billboardCount = 0

portalsList = [obj for obj in entityCollection.objects if "portal" in obj] #gets a list of portals
portalCount = len(portalsList)
print("Portal Count: "+str(portalCount))

entitiesList = [obj for obj in entityCollection.objects if "class" in obj]
entityCount = len(entitiesList)
print(str(entityCount) + " entities found!")

spawnPointList = [obj for obj in entityCollection.objects if "spawn_point" in obj]
spawnPointCount = len(spawnPointList)
print("Spawn Points Found: "+str(spawnPointCount))

billboardList = [obj for obj in entityCollection.objects if "billboard" in obj]
billboardCount = len(billboardList)

#Get meta data
#levelMetaData = none
for obj in entityCollection.objects:
    if obj.name=="meta_data":
        levelName = obj["level_name"]
        #levelMetaData = obj
        break   
        
fileName = baseDir+"\\"+levelName+fileExt

file = open(fileName,"wb")

#header info
file.write(struct.pack('I',magicNum))
file.write(struct.pack('I',entityCount))
file.write(struct.pack('I',portalCount))
file.write(struct.pack('I',spawnPointCount))
file.write(struct.pack('I',billboardCount))
#WriteString(file,levelMetaData["model_path"]) #TODO: make model export path the same

#save portals
for obj in portalsList:
    WriteString(file,obj["portal"]) #level id
    WriteString(file,obj["spawn_id"])
    WriteVec3(file,obj.location)
    WriteBoundingBox(file,obj.dimensions) #bounding box
    
for obj in spawnPointList:
    WriteString(file,obj["spawn_point"]) #id string
    WriteVec3(file,obj.location)
    file.write(struct.pack('f',obj.rotation_euler[2])) #exports in radians
    
for obj in billboardList:
    file.write(struct.pack('I',obj["billboard"])) #billboard type
    WriteVec3(file,obj.location)

for obj in entitiesList:
    #save the actual type of object
    WriteString(file,obj["class"]) 
    
    #position
    WriteVec3(file,obj.location)
    
    #rotation
    file.write(struct.pack('f',obj.rotation_euler[2])) #exports in radians
    
    #special data
    if obj["class"]=="npc":
        WriteString(file,obj["npc_type"])
       
            
        
file.close()

print("Entity file successfully created!")