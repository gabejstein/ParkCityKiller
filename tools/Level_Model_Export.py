#This exports level models for use in Raylib games
#Should be used on all visible mesh objects

import bpy
import os

levelName = 'inagi_town'
exportDir = './export'

baseDir = os.path.dirname(bpy.data.filepath)

bpy.ops.wm.save_mainfile()

#TODO: apply all modifiers. Maybe it can be done in export.

bpy.ops.object.select_all(action='SELECT')

selected_objs = bpy.context.selected_objects

#apply all modifiers #currently wont work because of shared data
#for obj in selected_objs:
    #bpy.context.view_layer.objects.active = obj
    #for modifier in obj.modifiers:
        #bpy.ops.object.modifier_apply(modifier=modifier.name)

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