#include <filesystem>
#include <fstream>
#include <stdexcept>
#include "InfiniteFiles.h"
#include "Png.h"

namespace fs = std::filesystem;

namespace {
	// The regions kept at once (a region built : its plan, its tiles, its images, about 40 MB)
	constexpr std::size_t KeptRegions = 2;
	// The caches of the infinite world forgotten past this many values (pure functions : computed again when asked)
	constexpr std::size_t CacheMax = 1500000;
}

struct worldgen::InfiniteWriter::Data {
	Shapes shapes;
	std::vector<RoomTemplate> templates;
	std::unique_ptr<Atlas> atlas;
	RgbImage corr;
};

struct worldgen::InfiniteWriter::Built {
	int rx = 0, ry = 0;
	std::unique_ptr<RegionBuilder> plan;
	std::unique_ptr<World> world;
	std::unique_ptr<TileImages> images;
};

std::optional<std::int64_t> worldgen::InfiniteSeed(const std::string& folder) {
	auto in = std::ifstream{ (fs::path{ folder } / InfiniteFile).string() };
	auto line = std::string{};
	while (std::getline(in, line)) {
		if (line.rfind("seed=", 0) == 0) {
			return std::stoll(line.substr(5));
		}
	}
	return std::nullopt;
}

worldgen::InfiniteWriter::InfiniteWriter(std::int64_t seed, std::string folder, std::string worldPath, Resources resources) :
	m_seed(seed), m_folder(std::move(folder)), m_worldPath(std::move(worldPath)), m_resources(std::move(resources)),
	m_world(std::make_unique<Infinite>(seed)) {
}

worldgen::InfiniteWriter::~InfiniteWriter() = default;

void worldgen::InfiniteWriter::prepare() {
	if (m_data != nullptr) {
		return;
	}
	auto data = std::make_unique<Data>();
	const auto root = m_resources.root + "/Worldgen";
	auto shapesFile = std::ifstream{ root + "/shapes.txt" };
	auto index = std::ifstream{ root + "/tiles.txt" };
	if (!shapesFile || !index) {
		throw std::runtime_error{ "worldgen : no " + root + "/shapes.txt or tiles.txt" };
	}
	data->shapes = Shapes::Parse(shapesFile);
	data->templates = LoadRoomTemplates(root + "/rooms");
	auto paintedIndex = std::ifstream{ root + "/painted.txt" };
	if (paintedIndex.good()) {
		data->atlas = std::make_unique<Atlas>(ReadPng(root + "/tiles.png"), index, ReadPng(root + "/painted.png"), paintedIndex);
	} else {
		data->atlas = std::make_unique<Atlas>(ReadPng(root + "/tiles.png"), index);
	}
	data->corr = ReadPng(m_resources.root + "/Chipsets/corr.png");
	m_data = std::move(data);
	fs::create_directories(fs::path{ m_folder } / "Chipsets");
	auto out = std::ofstream{ (fs::path{ m_folder } / InfiniteFile).string() };
	out << "[World]\nseed=" << m_seed << "\nchunks=" << WorldChunks << "\n";
}

worldgen::Start worldgen::InfiniteWriter::start() {
	if (!m_start) {
		const auto [i, j] = m_world->start();
		const auto* v = m_world->village(i, j);
		const auto [tx, ty] = v != nullptr ? v->v.center : Block{ Center, Center };
		const auto cx = tx / Chunk;
		const auto cy = ty / Chunk;
		m_start = Start{ "w_" + std::to_string(cx) + "_" + std::to_string(cy), tx - cx * Chunk, ty - cy * Chunk + 1 };
	}
	return *m_start;
}

bool worldgen::InfiniteWriter::written(int cx, int cy) const {
	const auto name = "w_" + std::to_string(cx) + "_" + std::to_string(cy);
	// (its last file : a chunk being written, or left half written, is not)
	return fs::exists(fs::path{ m_folder } / name / "scripts.data");
}

worldgen::InfiniteWriter::Built& worldgen::InfiniteWriter::region(int rx, int ry) {
	for (auto it = m_regions.begin(); it != m_regions.end(); ++it) {
		if ((*it)->rx == rx && (*it)->ry == ry) {
			m_regions.splice(m_regions.begin(), m_regions, it);
			return *m_regions.front();
		}
	}
	prepare();
	if (m_world->cached() > CacheMax) {
		m_world->forget();
	}
	while (m_regions.size() >= KeptRegions) {
		m_regions.pop_back();
	}
	auto built = std::make_unique<Built>();
	built->rx = rx;
	built->ry = ry;
	built->plan = std::make_unique<RegionBuilder>(*m_world, rx, ry);
	built->world = built->plan->build(m_data->shapes, m_data->templates);
	built->images = std::make_unique<TileImages>(*m_data->atlas, built->world->tiles());
	// The rooms of the region (their doors are in its chunks : entered once one of them is written)
	const auto chipset = "chipset_rooms_" + std::to_string(rx) + "_" + std::to_string(ry);
	if (!fs::exists(fs::path{ m_folder } / "Chipsets" / (chipset + ".png"))) {
		WriteRegionRooms(*built->world, *built->images, m_data->corr, m_resources.root + "/Chipsets/tile_scripts", m_folder, m_worldPath, chipset);
	}
	m_regions.push_front(std::move(built));
	return *m_regions.front();
}

void worldgen::InfiniteWriter::plan(int rx, int ry) {
	region(rx, ry);
}

void worldgen::InfiniteWriter::write(int cx, int cy) {
	auto& built = region(FloorDiv(cx, RegionChunks), FloorDiv(cy, RegionChunks));
	WriteRegionChunk(*built.world, *built.images, m_data->corr, m_resources.root + "/Chipsets/tile_scripts", m_folder, m_worldPath, cx, cy);
}

worldgen::InfiniteWorldWriter::InfiniteWorldWriter(std::int64_t seed, std::string folder, std::string worldPath, Resources resources) :
	m_writer(std::make_unique<InfiniteWriter>(seed, std::move(folder), std::move(worldPath), std::move(resources))) {
}

worldgen::InfiniteWorldWriter::~InfiniteWorldWriter() = default;

std::string worldgen::InfiniteWorldWriter::stage() const {
	if (m_step == 0) {
		return "Préparation";
	}
	if (m_step == 1) {
		return "Le climat et les villages";
	}
	if (m_step == 2) {
		return "Le relief, les routes et la nature";
	}
	const auto count = (2 * StartRadius + 1) * (2 * StartRadius + 1);
	return done() ? "Terminé" : "Les cartes (" + std::to_string(m_step - 2) + " / " + std::to_string(count) + ")";
}

bool worldgen::InfiniteWorldWriter::step() {
	if (done() || m_writer == nullptr) {
		return false;
	}
	if (m_step == 0) {
		// A world written again : the files of the world before are left
		const auto folder = fs::path{ m_writer->folder() };
		if (fs::exists(folder)) {
			fs::remove_all(folder);
		}
		m_writer->prepare();
	} else if (m_step == 1) {
		m_start = m_writer->start();
		const auto at = m_start.map.find('_', 2);
		m_cx = std::stoi(m_start.map.substr(2, at - 2));
		m_cy = std::stoi(m_start.map.substr(at + 1));
	} else if (m_step == 2) {
		m_writer->plan(FloorDiv(m_cx, RegionChunks), FloorDiv(m_cy, RegionChunks));
	} else {
		const auto k = m_step - 3;
		const auto side = 2 * StartRadius + 1;
		m_writer->write(m_cx - StartRadius + k % side, m_cy - StartRadius + k / side);
	}
	m_step++;
	return !done();
}

worldgen::Start worldgen::GenerateInfiniteWorld(std::int64_t seed, const std::string& folder, const std::string& worldPath, const Resources& resources) {
	auto writer = InfiniteWorldWriter{ seed, folder, worldPath, resources };
	while (writer.step()) {
	}
	return writer.start();
}
