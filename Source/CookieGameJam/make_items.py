import unreal

DEST = "/Game/Items"
PARENT = unreal.load_class(None, "/Script/CookieGameJam.AInteractableBase")
tools = unreal.AssetToolsHelpers.get_asset_tools()

for asset in unreal.EditorUtilityLibrary.get_selected_assets():
    if not isinstance(asset, unreal.StaticMesh):
        continue

    base = asset.get_name()
    if base.startswith("SM_"):
        base = base[3:]
    bp_name = "BP_" + base
    bp_path = DEST + "/" + bp_name

    if unreal.EditorAssetLibrary.does_asset_exist(bp_path):
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    else:
        factory = unreal.BlueprintFactory()
        factory.set_editor_property("parent_class", PARENT)
        bp = tools.create_asset(bp_name, DEST, unreal.Blueprint, factory)

    if not bp:
        unreal.log_error("Failed: " + bp_name)
        continue

    cls = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    cdo = unreal.get_default_object(cls)
    cdo.set_editor_property("item_mesh", asset)
    cdo.set_editor_property("item_id", base)
    cdo.set_editor_property("prompt_text", "[E] " + base.replace("_", " "))

    comp = cdo.get_editor_property("static_mesh_comp")
    comp.set_collision_profile_name("Pickup")
    comp.set_simulate_physics(True)

    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.EditorAssetLibrary.save_loaded_asset(bp)
    unreal.log("Done " + bp_path)