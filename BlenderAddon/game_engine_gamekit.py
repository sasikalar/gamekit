#Copyright (c) 2010 Xavier Thomas (xat)
#
# ***** BEGIN MIT LICENSE BLOCK *****
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.
#
# ***** END MIT LICENCE BLOCK *****


bl_addon_info = {
    'name': 'Gamekit Engine',
    'author': 'Xavier Thomas (xat)',
    'version': '0.1 2010/08/18',
    'blender': (2, 5, 4),
    'location': 'Info Window > Render Engine > Gamekit',
    'description': 'Launch game using the fine gamekit engine',
    'wiki_url': 'http://wiki.blender.org/index.php/Extensions:2.5/Py/Scripts/Gamekit_Engine',
    'tracker_url': 'https://projects.blender.org/tracker/index.php?func=detail&aid=22933&group_id=153&atid=467',
    'category': 'Game Engine'}


import bpy
from bpy.props import *
import os
import subprocess
import shlex
   

# Use some of the existing buttons.
import properties_game
properties_game.RENDER_PT_game_player.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.RENDER_PT_game_performance.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.RENDER_PT_game_shading.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.RENDER_PT_game_sound.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_game.RENDER_PT_game_stereo.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.WORLD_PT_game_context_world.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.WORLD_PT_game_world.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.WORLD_PT_game_mist.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.WORLD_PT_game_physics.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.PHYSICS_PT_game_physics.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.PHYSICS_PT_game_collision_bounds.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_game
    
import properties_material
properties_material.MATERIAL_PT_context_material.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_custom_props.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_diffuse.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_flare.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_halo.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_mirror.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_options.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_physics.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_shading.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_shadow.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_specular.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_sss.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_strand.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_transp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_transp_game.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_density.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_integration.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_lighting.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_options.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_shading.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_transp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_material

import properties_data_mesh
properties_data_mesh.DATA_PT_context_mesh.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_custom_props_mesh.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_normals.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_settings.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_shape_keys.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_texface.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_uv_texture.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_vertex_colors.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_vertex_groups.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.MESH_MT_shape_key_specials.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.MESH_MT_vertex_group_specials.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_data_mesh
 
import properties_texture
properties_texture.TEXTURE_MT_envmap_specials.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_MT_specials.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_blend.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_clouds.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_colors.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_context_texture.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_custom_props.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_distortednoise.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_envmap.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_envmap_sampling.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_image.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_image_mapping.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_image_sampling.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_influence.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_magic.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_mapping.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_marble.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_musgrave.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_texture.TEXTURE_PT_plugin.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_pointdensity.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_pointdensity_turbulence.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_preview.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_stucci.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_voronoi.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_voxeldata.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_wood.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TextureSlotPanel.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_texture.TextureTypePanel.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_texture

import properties_data_camera
properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_camera.DATA_PT_camera_display.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_camera.DATA_PT_context_camera.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_camera.DATA_PT_custom_props_camera.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_data_camera

import properties_data_lamp
properties_data_lamp.DATA_PT_area.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_context_lamp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_custom_props_lamp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_falloff_curve.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_lamp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_preview.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_shadow.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_spot.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_sunsky.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.LAMP_MT_sunsky_presets.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_data_lamp


# Setting for this addon
class GamekitSettings(bpy.types.IDPropertyGroup):
    pass  
      

# Declare gamekit as a render engine
class GamekitRender(bpy.types.RenderEngine):
    bl_idname = 'GAMEKIT_RENDER'
    bl_label = "Gamekit"


# Base class for ou panels
class RenderButtonsPanel(bpy.types.Panel):
    bl_label = "Base Panel Class"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "render"
    # COMPAT_ENGINES must be defined in each subclass, external engines can add themselves here
    
    @classmethod
    def poll(self, context):
        rd = context.scene.render
        return (rd.engine in self.COMPAT_ENGINES)

# Runtime panel
class RENDER_PT_gamekit_runtime(RenderButtonsPanel):
    bl_label = "Gamekit Runtime"
    COMPAT_ENGINES = {'GAMEKIT_RENDER'}

    def draw(self, context):
        layout = self.layout

        scene = context.scene
        gks = scene.gamekit

        col = layout.column()
        
        col.prop(gks, "gk_window_title")
        col.prop(gks, "gk_runtime_exec_path")
        col.prop(gks, "gk_runtime_working_dir")
        col.prop(gks, "gk_export_tmp_dir")
        col.prop(gks, "gk_log_file")

# Main Gamekit panel
class RENDER_PT_gamekit(RenderButtonsPanel):
    bl_label = "Gamekit"
    COMPAT_ENGINES = {'GAMEKIT_RENDER'}

    def draw(self, context):
        gks = context.scene.gamekit
        render = context.scene.render
        layout = self.layout
        
        layout.operator("scene.gamekit_start_game", text="Start Game", icon='GAME')
        
        col = layout.column()
        
        col.prop(render, "fps")
        col.prop(gks, "gk_start_frame")
        col.prop(gks, "gk_render_system")
        col.prop(gks, "gk_grab_input")
        col.prop(gks, "gk_verbose")
        col.prop(gks, "gk_frustrum_culling")
        col.prop(gks, "gk_debug_sound")
        col.prop(gks, "gk_debug_physicsaabb")
        col.prop(gks, "gk_build_instances")
        
        col.prop(gks, "gk_use_shadows")
        col.prop(gks, "gk_shadow_type")
        col.prop(gks, "gk_shadow_color")
        col.prop(gks, "gk_far_dist_shadow")

# Export startup config fo gamekit
class GamekitExportStartupFileOperator(bpy.types.Operator):
    ''''''
    bl_idname = "scene.gamekit_export_startup_config"
    bl_label = "Export Gamekit Startup.cfg file"
    filepath = StringProperty(name="File Path", description="File path used for exporting the CFG file", maxlen= 1024, default= "")
    
    @classmethod
    def poll(self, context):
        return context.scene.render.engine == 'GAMEKIT_RENDER'

    def execute(self, context):
        gdata = context.scene.game_settings
        gks = context.scene.gamekit
        scene = context.scene

        try:
            file = open(self.properties.filepath, 'w')
        except IOError as er:
            self.report('ERROR', str(er))
            return {'CANCELLED'}
            
        
        file.write("rendersystem        = ")
        file.write(gks.gk_render_system.lower())
        file.write("\n")
        
        file.write("log                 = ")
        file.write(gks.gk_log_file)
        file.write("\n")
        
        file.write("debugfps            = ")
        file.write( str(gdata.show_framerate_profile))
        file.write("\n")
        
        file.write("debugphysics        = ")
        file.write( str(gdata.show_physics_visualization))
        file.write("\n")
        
        file.write("debugphysicsaabb    = ")
        file.write( str(gks.gk_debug_physicsaabb))
        file.write("\n")
        
        file.write("grabinput           = ")
        file.write( str(gks.gk_grab_input))
        file.write("\n")
        
        file.write("verbose             = ")
        file.write(str(gks.gk_verbose))
        file.write("\n")

        file.write("winsize             = ")
        file.write( str(gdata.resolution_x) + " " + str(gdata.resolution_y) )
        file.write("\n")
        
        file.write("wintitle            = ")
        file.write(gks.gk_window_title)
        file.write("\n")
        
        file.write("fullscreen          = ")
        file.write( str(gdata.show_fullscreen))  
        file.write("\n")      
        
#        file.write("resources           = ")
#        file.write(gks.gk_resources)
#        file.write("\n")
        
        file.write("animspeed           = ")
        file.write( str(scene.render.fps))
        file.write("\n")
        
        file.write("startframe          = ")
        file.write( str(gks.gk_start_frame))
        file.write("\n")
        
        file.write("blendermat          = ")
        if gdata.material_mode == 'MULTITEXTURE':
            file.write("True")
        else:
            file.write("False")
        file.write("\n")
        
        file.write("buildinstances      = ")
        file.write( str(gks.gk_build_instances))
        file.write("\n")
        
        file.write("frustrumculling     = ")
        file.write( str(gks.gk_frustrum_culling))
        file.write("\n")
        
        file.write("showdebugprops      = ")
        file.write( str(gdata.show_debug_properties))
        file.write("\n")
        
        file.write("debugsounds         = ")
        file.write( str(gks.gk_debug_sound))
        file.write("\n")
        
        file.write("enableshadows       = ")
        file.write( str(gks.gk_use_shadows))
        file.write("\n")
        
        file.write("shadowtechnique     = ")
        file.write(gks.gk_shadow_type.replace('_','').lower())
        file.write("\n")
        
        file.write("colourshadow        = ")
        file.write( str(gks.gk_shadow_color.r) + " " + str(gks.gk_shadow_color.g) + " " + str(gks.gk_shadow_color.b))
        file.write("\n")
        
        file.write("fardistanceshadow   = ")
        file.write( str(gks.gk_far_dist_shadow))
        file.write("\n")
        
        file.close()
        return {'FINISHED'}
    
    def invoke(self, context, event):
        context.manager.add_fileselect(self)
        return {'RUNNING_MODAL'}
    

# Start gamekit
class GamekitStartGameOperator(bpy.types.Operator):
    ''''''
    bl_idname = "scene.gamekit_start_game"
    bl_label = "Start Game"
    
    @classmethod
    def poll(self, context):
        return context.scene.render.engine == 'GAMEKIT_RENDER'

    def execute(self, context):
        scene = context.scene
        gks = scene.gamekit
        
        # make sure path are absolute 
        gks.gk_runtime_exec_path = bpy.path.abspath(gks.gk_runtime_exec_path)
        #gks.gk_runtime_working_dir = bpy.utils.expandpath(gks.gk_runtime_working_dir)
        if not gks.gk_runtime_working_dir.endswith(os.sep):
            gks.gk_runtime_working_dir += os.sep
        #gks.gk_export_tmp_dir = bpy.utils.expandpath(gks.gk_export_tmp_dir)
        if not gks.gk_export_tmp_dir.endswith(os.sep):
            gks.gk_export_tmp_dir += os.sep

        gamefile = gks.gk_export_tmp_dir + "BOgreKitGame.blend"
        cfgfile = gks.gk_export_tmp_dir + "BOgreKitStartup.cfg"
        cmdline = gks.gk_runtime_exec_path + " -c " + cfgfile + " " + gamefile
        
        args =  shlex.split(cmdline.replace(os.sep, '/'))        

        # save temporary game .blend         
        returnv = bpy.ops.wm.save_as_mainfile(filepath=gamefile, compress= False, relative_remap=True, check_existing=True, copy=True)
        if returnv != {'FINISHED'}:
            self.report('ERROR',"Could not save temporary blend file: " + gamefile)
            return {'CANCELLED'}

        # save temporary game .cfg
        returnv = bpy.ops.scene.gamekit_export_startup_config(filepath=cfgfile)
        if returnv != {'FINISHED'}:
            self.report('ERROR',"Could not save temporary cfg file: " + cfgfile)
            return {'CANCELLED'}

        # launch game
        try:
            subp = subprocess.Popen(args, cwd=gks.gk_runtime_working_dir, shell=False)
            subp.communicate() #like wait() but without the risk of deadlock with verbose output
            returnv = subp.returncode

            if returnv != 0:
                self.report('ERROR',"Gamekit exited anormally.")
                return {'CANCELLED'}

            self.report('INFO',"Gamekit exited normally.")

        except OSError as er:
            self.report('ERROR', "Could not launch: " + cmdline + " in " + gks.gk_runtime_working_dir+ " : " + str(er))
            return {'CANCELLED'}
            
        return {'FINISHED'}
     
    def invoke(self, context, event):
        return self.execute(context)

def register():
    bpy.types.Scene.PointerProperty(attr="gamekit", type=GamekitSettings, name="Gamekit", description="Gamekit Settings")
    
    default_path = os.environ.get("TEMP")
    if not default_path:
        if os.name == 'nt':
            default_path = "c:/tmp/"
        else:
            default_path = "/tmp/"
        
    GamekitSettings.StringProperty( attr="gk_runtime_exec_path",
                    name="Runtime",
                    description="Path of the gamekit executable",
                    maxlen = 512,
                    default = "/usr/local/bin/OgreKit",
                    subtype='FILE_PATH')
                    
                    
    GamekitSettings.StringProperty( attr="gk_runtime_working_dir",
                    name="Working Directory",
                    description="Directory in which to launch the game",
                    maxlen = 512,
                    default = default_path,
                    subtype='FILE_PATH')

    GamekitSettings.StringProperty( attr="gk_export_tmp_dir",
                    name="Temporary files",
                    description="Directory in which to store temporary files",
                    maxlen = 512,
                    default = default_path,
                    subtype='FILE_PATH')

    GamekitSettings.EnumProperty( attr="gk_render_system",
                    name="Render system",
                    items=(('GL', 'OpenGL', 'gl'),
                    ('D3D9', 'Direct 3D 9', 'd3d9'),
                    ('D3D10', 'Direct 3D 10', 'd3d10'),
                    ('D3D11', 'Direct 3D 11', 'd3d11')),
                    default = 'GL')

    GamekitSettings.StringProperty( attr="gk_log_file",
                    name="Log",
                    description="Name of the log file that will be created in the working directory.",
                    maxlen=512,
                    default="OgreKit.log")

    GamekitSettings.StringProperty( attr="gk_window_title",
                    name="Window title",
                    description="Name of the log file that will be created in the working directory.",
                    maxlen=512,
                    default="OgreKit")

    GamekitSettings.BoolProperty( attr="gk_grab_input",
                    name="Grab input",
                    description="Grab mouse and keyboard event (cause other app to be unusable while game is runing).",
                    default=True)

    GamekitSettings.BoolProperty( attr="gk_verbose",
                    name="Verbose",
                    description="Verbose output on the console.",
                    default=False)

    GamekitSettings.IntProperty( attr="gk_start_frame",
                    name="Start frame",
                    min=1,
                    soft_min=1,
                    default =1)

    GamekitSettings.BoolProperty( attr="gk_frustrum_culling",
                    name="Frustrum culling",
                    default=True)

    GamekitSettings.BoolProperty( attr="gk_debug_sound",
                    name="Debug sound",
                    default=False)

    GamekitSettings.BoolProperty( attr="gk_debug_physicsaabb",
                    name="Physics AABB",
                    description="Show physics axis aligned bounding boxes",
                    default=False)

    GamekitSettings.BoolProperty( attr="gk_build_instances",
                    name="Build instances",
                    default=False)

    GamekitSettings.BoolProperty( attr="gk_use_shadows",
                    name="Enable shadows",
                    default=True)

    GamekitSettings.EnumProperty( attr="gk_shadow_type",
                    name="Shadow type",
                    items=(('NONE', 'None', 'n'),
                    ('STENCIL_MODULATIVE', 'Stencil modulative', 'sm'),
                    ('STENCIL_ADDITIVE', 'Stencil additive', 'sa'),
                    ('TEXTURE_MODULATIVE', 'Texture modulative', 'tm'),
                    ('TEXTURE_ADDITIVE', 'Texture additive', 'ta'),
                    ('TEXTURE_MODULATIVE_INTEGRATED', 'Texture modulative integrated', 'tmi'),
                    ('TEXTURE_ADDITIVE_INTEGRATED', 'Texture additive integrated', 'tai')),
                    description = "Ogre shadow technique.",
                    default = 'STENCIL_ADDITIVE')

    GamekitSettings.FloatVectorProperty( attr="gk_shadow_color",
                    name="Sadow color",
                    subtype='COLOR',
                    min=0.0,
                    max=1.0,
                    soft_min=0.0,
                    soft_max=1.0,
                    default = (0.0, 0.0, 0.0))

    GamekitSettings.FloatProperty( attr="gk_far_dist_shadow",
                    name="Far distance shadow",
                    min=0.0,
                    max=1.0,
                    soft_min=0.0,
                    soft_max=1.0,
                    default =0.0)


def unregister():
    bpy.types.Scene.RemoveProperty("gamekit")

if __name__ == "__main__":
    register()