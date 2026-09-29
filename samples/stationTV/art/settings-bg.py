# Fondo del menú de ajustes de StationTV (images/settings-bg.jpg)
#
# Genera la escena y la guarda como .blend o la renderiza:
#   blender -b --python settings-bg.py -- --save settings-bg.blend
#   blender -b --python settings-bg.py -- --render salida.png [ancho alto muestras]
# Para retocarla a mano, abrir settings-bg.blend y renderizar con F12 (1920x1080, 192 muestras).

import bpy, bmesh, math, sys
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
MODE = argv[0] if argv else "--render"
OUT = argv[1] if len(argv) > 1 else "settings-bg.png"
W = int(argv[2]) if len(argv) > 2 else 1920
H = int(argv[3]) if len(argv) > 3 else 1080
SAMPLES = int(argv[4]) if len(argv) > 4 else 192

bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene

# ---------------------------------------------------------------- materiales
def metal(name, color, rough, coat=0.0):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    p = m.node_tree.nodes["Principled BSDF"]
    p.inputs["Base Color"].default_value = (*color, 1)
    p.inputs["Metallic"].default_value = 1.0
    p.inputs["Roughness"].default_value = rough
    if coat:
        p.inputs["Coat Weight"].default_value = coat
        p.inputs["Coat Roughness"].default_value = 0.05
    return m

gunmetal = metal("gunmetal", (0.08, 0.082, 0.09), 0.26, coat=0.8)
steel = metal("steel", (0.55, 0.56, 0.60), 0.18)
dark = metal("dark", (0.05, 0.05, 0.055), 0.48)

# ---------------------------------------------------------------- engranaje
def gear(name, teeth, r_out, r_root, r_hub, thick, holes=0, hole_r=0.0, hole_d=0.0, mat=gunmetal):
    bm = bmesh.new()
    outer = []
    for i in range(teeth):
        a0 = 2 * math.pi * i / teeth
        step = 2 * math.pi / teeth
        # raíz, flanco de subida, cresta, flanco de bajada (diente trapezoidal)
        for frac, r in ((0.00, r_root), (0.18, r_root), (0.30, r_out), (0.55, r_out), (0.67, r_root)):
            a = a0 + frac * step
            outer.append((r * math.cos(a), r * math.sin(a)))
    n = len(outer)
    inner = [(r_hub * math.cos(2 * math.pi * i / n + 0.3 * 2 * math.pi / n), r_hub * math.sin(2 * math.pi * i / n + 0.3 * 2 * math.pi / n)) for i in range(n)]
    vo = [bm.verts.new((x, y, 0)) for x, y in outer]
    vi = [bm.verts.new((x, y, 0)) for x, y in inner]
    for i in range(n):
        j = (i + 1) % n
        bm.faces.new((vo[i], vo[j], vi[j], vi[i]))
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    ob = bpy.data.objects.new(name, me)
    scene.collection.objects.link(ob)
    ob.data.materials.append(mat)

    sol = ob.modifiers.new("solid", "SOLIDIFY")
    sol.thickness = thick
    sol.offset = 0

    # Agujeros de aligerado
    for k in range(holes):
        a = 2 * math.pi * k / holes
        bpy.ops.mesh.primitive_cylinder_add(vertices=64, radius=hole_r, depth=thick * 4, location=(hole_d * math.cos(a), hole_d * math.sin(a), 0))
        cut = bpy.context.active_object
        cut.hide_render = True
        cut.hide_viewport = True
        cut.parent = ob  # se mueve con el engranaje
        b = ob.modifiers.new(f"hole{k}", "BOOLEAN")
        b.operation = "DIFFERENCE"
        b.object = cut
        b.solver = "EXACT"

    bev = ob.modifiers.new("bevel", "BEVEL")
    bev.width = thick * 0.12
    bev.segments = 3
    bev.limit_method = "ANGLE"
    bev.angle_limit = math.radians(30)
    bev.harden_normals = True
    return ob

def boss(parent, r, depth):
    bpy.ops.mesh.primitive_cylinder_add(vertices=96, radius=r, depth=depth)
    b = bpy.context.active_object
    b.data.materials.append(dark)
    bev = b.modifiers.new("bevel", "BEVEL")
    bev.width = 0.04
    bev.segments = 3
    bev.harden_normals = True
    b.parent = parent
    return b

def axle(parent, r, depth):
    boss(parent, r * 2.2, depth * 0.8)
    bpy.ops.mesh.primitive_cylinder_add(vertices=96, radius=r, depth=depth)
    a = bpy.context.active_object
    a.data.materials.append(steel)
    bev = a.modifiers.new("bevel", "BEVEL")
    bev.width = r * 0.15
    bev.segments = 4
    for poly in a.data.polygons:
        poly.use_smooth = True
    a.parent = parent
    return a

root = bpy.data.objects.new("root", None)
scene.collection.objects.link(root)

big = gear("big", 24, 2.0, 1.72, 0.42, 0.42, holes=8, hole_r=0.2, hole_d=1.18)
axle(big, 0.30, 0.62)
small_teeth = 14
small = gear("small", small_teeth, 1.2, 0.95, 0.26, 0.36, holes=6, hole_r=0.12, hole_d=0.66, mat=gunmetal)
axle(small, 0.19, 0.52)
tiny = gear("tiny", 10, 0.8, 0.60, 0.16, 0.30, mat=dark)
axle(tiny, 0.12, 0.44)

# Engranados: distancia entre centros ~ suma de radios de paso, girado medio diente
pitch_big = (2.0 + 1.72) / 2
pitch_small = (1.2 + 0.95) / 2
pitch_tiny = (0.8 + 0.60) / 2
ang = math.radians(200)
small.location = ((pitch_big + pitch_small) * math.cos(ang), (pitch_big + pitch_small) * math.sin(ang), -0.05)
small.rotation_euler.z = math.pi / small_teeth
ang2 = math.radians(-35)
tiny.location = ((pitch_big + pitch_tiny) * math.cos(ang2), (pitch_big + pitch_tiny) * math.sin(ang2), 0.08)
tiny.rotation_euler.z = math.radians(9)
big.rotation_euler.z = math.radians(4)

for ob in (big, small, tiny):
    ob.parent = root

# Conjunto inclinado hacia la cámara
root.rotation_euler = (math.radians(62), math.radians(-22), math.radians(-14))
root.location = (0.35, 0, 0.1)

# ---------------------------------------------------------------- cámara
bpy.ops.object.camera_add(location=(0, -10.5, 1.2))
cam = bpy.context.active_object
scene.camera = cam
cam.data.lens = 34
target = Vector((0.1, 0, 0.05))
cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
cam.data.dof.use_dof = True
cam.data.dof.focus_object = big
cam.data.dof.aperture_fstop = 4.0

# ---------------------------------------------------------------- luces
def area(name, loc, size, energy, color, target=(0, 0, 0)):
    bpy.ops.object.light_add(type="AREA", location=loc)
    l = bpy.context.active_object
    l.data.size = size
    l.data.energy = energy
    l.data.color = color
    l.rotation_euler = (Vector(target) - Vector(loc)).to_track_quat("-Z", "Y").to_euler()
    return l

area("key", (2.0, -6.0, 7.0), 9.0, 3500, (1.0, 0.97, 0.93))
area("top", (0.0, 1.0, 8.0), 6.0, 1200, (0.9, 0.95, 1.0))
area("rimBlue", (-6.0, 4.0, 2.5), 3.0, 2600, (0.25, 0.55, 1.0))
area("rimWarm", (6.5, 3.5, -1.0), 2.0, 900, (1.0, 0.45, 0.25))
area("fill", (-3.0, -6.0, -2.0), 5.0, 120, (0.6, 0.7, 1.0))

# Fondo: pared oscura detrás con un foco que deja un halo suave alrededor de los engranajes
bpy.ops.mesh.primitive_plane_add(size=60, location=(0, 7, 0), rotation=(math.radians(90), 0, 0))
wall = bpy.context.active_object
wm = bpy.data.materials.new("wall")
wm.use_nodes = True
wp = wm.node_tree.nodes["Principled BSDF"]
wp.inputs["Base Color"].default_value = (0.012, 0.013, 0.017, 1)
wp.inputs["Roughness"].default_value = 1.0
wall.data.materials.append(wm)
bpy.ops.object.light_add(type="SPOT", location=(0.9, 2.5, 0.6))
halo = bpy.context.active_object
halo.data.energy = 2500
halo.data.color = (0.55, 0.7, 1.0)
halo.data.spot_size = math.radians(80)
halo.data.spot_blend = 1.0
halo.data.shadow_soft_size = 1.0
halo.rotation_euler = (Vector((0.9, 7, 0.6)) - halo.location).to_track_quat("-Z", "Y").to_euler()

world = bpy.data.worlds.new("world")
world.use_nodes = True
world.node_tree.nodes["Background"].inputs["Color"].default_value = (0, 0, 0, 1)
scene.world = world

# ---------------------------------------------------------------- render
scene.render.engine = "CYCLES"
scene.cycles.device = "CPU"
scene.cycles.samples = SAMPLES
scene.cycles.use_denoising = True
scene.render.resolution_x = W
scene.render.resolution_y = H
scene.render.resolution_percentage = 100
scene.view_settings.view_transform = "AgX"
scene.view_settings.look = "AgX - Punchy"
scene.render.film_transparent = False
scene.render.image_settings.file_format = "PNG"
if MODE == "--save":
    scene.render.filepath = "//settings-bg.png"
    bpy.ops.wm.save_as_mainfile(filepath=bpy.path.abspath(OUT) if OUT.startswith("//") else __import__("os").path.abspath(OUT))
else:
    scene.render.filepath = OUT
    bpy.ops.render.render(write_still=True)
