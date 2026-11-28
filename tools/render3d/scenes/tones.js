// The colours of the tiles of each biome, shared by the scenes of the tiles (tiles*.js) : light, base, deep for the
// ramps ; near the voxel palettes of tools/tilegen BIOMES, in the palette of the identity (Cube World, washed).
// A scene renders the same geometry in every biome, only these colours change (the blocks between two biomes blend
// the two versions, pixel by pixel).
export // The colours of each biome (light, base, deep), near the voxel palettes of tools/tilegen BIOMES ; the shadow of a tree :
// the colour of the grass under it
const TONES = {
	// ground : the grounds of the map (grass, path, sand, paving : its slabs, its joints), flat colours (the wash gives
	// them their grain) ; tuft : the small tufts on the grass
	// conifer : the pines and the cactus (tools/tilegen "leaf" : red in autumn, as the voxel palette)
	meadow: { conifer: ["#78bc3a", "#3c9634", "#22702c"], leaves: ["#c8e03c", "#86c22c", "#4e8f1c"], grass: ["#c8e03c", "#92c832", "#5a9620"], shadow: "#5f9a2c", trunk: "#7c5a3a",
		ground: { grass: "#86c234", path: "#d0844e", sand: "#f0d89a", pave: "#cdcad8", joint: "#8e8aa0", pebble: "#c8c0dc", rock: "#dcd4c2", stone: "#bdb8cc" }, tuft: "#5f9a2c" },
	autumn: { conifer: ["#f0a050", "#c8552c", "#8a3020"], leaves: ["#f6c860", "#e88a34", "#b44a2c"], grass: ["#e2c878", "#b88a3a", "#8a6228"], shadow: "#8c8a34", trunk: "#6e4a30",
		ground: { grass: "#b8b84a", path: "#c47c48", sand: "#ecd49a", pave: "#cdc6c6", joint: "#8e8484", pebble: "#c8bcb4", rock: "#d8ccb8", stone: "#bcb4b4" }, tuft: "#8c8a34" },
	snow: { conifer: ["#6fa890", "#2f6f5c", "#1c4a40"], leaves: ["#f2f8fa", "#9cc4b6", "#4f8478"], grass: ["#dceaec", "#8ab0b4", "#56808a"], shadow: "#a8b8d2", trunk: "#7a6a5e",
		ground: { grass: "#eef3fa", path: "#bcb0a8", sand: "#d2d4da", pave: "#c6cad6", joint: "#8a8ea0", pebble: "#b4b8c8", rock: "#d6dbe6", stone: "#b8bcc8" }, tuft: "#a8b8d2" },
	desert: { conifer: ["#a8c860", "#5a9a40", "#336a2c"], leaves: ["#c8d870", "#86b048", "#4a7a34"], grass: ["#e6b672", "#c09a50", "#8e6a2c"], shadow: "#c08848", trunk: "#7c5a3a",
		ground: { grass: "#e6b672", path: "#c98a52", sand: "#f2dcae", pave: "#dccab0", joint: "#a08868", pebble: "#d0b898", rock: "#e2c6a2", stone: "#ccbea6" }, tuft: "#c08848" },
};

// The materials that do not change with the biome (the buildings, the objects, the rooms) : light, base, deep
export const MATERIALS = {
	wood: ["#d9a56a", "#a8703f", "#6e4526"],
	plank: ["#f2dcb4", "#dcbf8e", "#b08e60"],
	stone: ["#e4e0ee", "#bcb7cc", "#857f98"],
	white: ["#ffffff", "#eef0f2", "#c3c8d0"],
	glass: ["#d6f2ff", "#8cc8ec", "#4f8cbc"],
	orange: ["#ffbf6a", "#f08a2c", "#b8561c"],
	roofr: ["#ff8a6a", "#e2493a", "#a8302a"],
	roofb: ["#8cb8ff", "#4a78d8", "#2e4c9c"],
	roofg: ["#a6dc6a", "#5aa83a", "#367028"],
	roofo: ["#ffc46a", "#f0922c", "#b8601c"],
	teal: ["#7ad0d0", "#3aa0a8", "#24707c"],
	water: ["#bfe8ff", "#58aef0", "#2f72c0"],
	metal: ["#9aa2b4", "#5c6478", "#363c4c"],
	ink: ["#33301c", "#33301c", "#33301c"],
};
