#This exports level models for use in Raylib games
#Should be used on all visible mesh objects

import bpy
import os

levelName = 'hotel'
exportDir = './export'

baseDir = os.path.dirname(bpy.data.filepath)

bpy.ops.wm.save_mainfile()

bpy.ops.object.select_all(action='SELECT')

bpy.ops.object.join()

#Set cursor to world origin and make object origin
bpy.context.scene.cursor.location[0] = 0
bpy.context.scene.cursor.location[1] = 0
bpy.context.scene.cursor.location[2] = 0

bpy.ops.object.origin_set(type='ORIGIN_CURSOR',center='MEDIAN')

bpy.context.view_layer.objects.active.name = levelName

fn = os.path.join(baseDir,levelName)
print(fn)

bpy.ops.export_scene.gltf(filepath=fn+".glb",use_selection=True)

bpy.ops.object.select_all(action='DESELECT')


bpy.ops.wm.revert_mainfile()

print("Level successfully exported.")