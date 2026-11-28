// The pure C++ of the chunks of the world (src/src/Game/Gameplay/World/Chunks) : the grid, the images, the records of the
// entities, the rules of the wild Pokemon that leave.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <chrono>
#include <filesystem>
#include <sstream>
#include <thread>
#include "doctest.h"
#include "Game/Gameplay/World/Chunks/ChunkGrid.h"
#include "Game/Gameplay/World/Chunks/ChunkImages.h"
#include "Game/Gameplay/World/Chunks/ChunkPrepareQueue.h"
#include "Game/Gameplay/World/Chunks/DespawnRules.h"
#include "Game/Gameplay/World/Chunks/EntityRecords.h"

TEST_CASE("The outdoor maps are named by their chunk") {
	CHECK(chunks::Parse("w_5_4") == std::optional<chunks::Coordinates>{ { 5, 4 } });
	CHECK(chunks::Parse("w_12_0") == std::optional<chunks::Coordinates>{ { 12, 0 } });
	for (const auto* other : { "w_room_0", "w_tiles", "w_5", "w_5_", "w__4", "w_5_4a", "Town", "" }) {
		CHECK_FALSE(chunks::Parse(other).has_value());
	}
}

TEST_CASE("The chunks around a block") {
	const auto grid = chunks::Grid{ 8, 8, 40, 40, {} };
	CHECK(grid.name({ 5, 4 }) == "w_5_4");
	CHECK(grid.at(200, 160) == chunks::Coordinates{ 5, 4 });
	CHECK(grid.at(239, 199) == chunks::Coordinates{ 5, 4 });
	CHECK(grid.at(-3, 1000) == chunks::Coordinates{ 0, 7 });
	CHECK(grid.distance({ 5, 4 }, 220, 180) == 0);
	CHECK(grid.distance({ 6, 4 }, 220, 180) == 20);
	CHECK(grid.distance({ 4, 3 }, 202, 161) == 3);
	// In the middle of a chunk : it and no other within 19 blocks
	CHECK(grid.around(220, 180, 19).size() == 1);
	// By a corner : the four, the nearest first
	const auto corner = grid.around(238, 198, chunks::LoadRadius);
	REQUIRE(corner.size() == 4);
	CHECK(corner.front() == chunks::Coordinates{ 5, 4 });
	// The edge of the world : none out of it
	CHECK(grid.around(1, 1, chunks::LoadRadius).size() == 1);
	// A room : one chunk
	const auto room = chunks::Grid{ 1, 1, 22, 16, "w_room_0" };
	CHECK(room.name({ 0, 0 }) == "w_room_0");
	CHECK(room.around(11, 8, chunks::LoadRadius).size() == 1);
}

TEST_CASE("A chunk comes before it is seen and leaves later") {
	CHECK(chunks::VisibleRadius < chunks::LoadRadius);
	CHECK(chunks::LoadRadius < chunks::UnloadRadius);
}

TEST_CASE("BMP and the messages of the worker") {
	auto image = worldgen::RgbImage{ 3, 2 };
	for (auto i = std::size_t{ 0 }; i < image.rgb.size(); i++) {
		image.rgb[i] = static_cast<std::uint8_t>(i * 7);
	}
	const auto decoded = chunks::DecodeBmp(worldgen::EncodeBmp(image));
	CHECK(decoded.width == 3);
	CHECK(decoded.height == 2);
	CHECK(decoded.rgb == image.rgb);
	CHECK_THROWS(chunks::DecodeBmp({ 1, 2, 3 }));

	auto files = chunks::Files{};
	for (auto i = 0; i < chunks::ImageCount; i++) {
		files[i] = i >= chunks::Ground && i <= chunks::Top ? worldgen::EncodeBmp(image) : worldgen::EncodePng(image);
	}
	// The optional ones may miss
	files[chunks::Mask].clear();
	const auto packed = chunks::Pack(files);
	const auto images = chunks::Decode(chunks::UnpackFiles(packed.data(), packed.size()));
	CHECK(images.error.empty());
	const auto back = chunks::Pack(images);
	const auto unpacked = chunks::UnpackImages(back.data(), back.size());
	CHECK(unpacked.image[chunks::Middle].rgb == image.rgb);
	CHECK(unpacked.image[chunks::Chipset].rgb == image.rgb);
	CHECK(unpacked.image[chunks::Mask].width == 0);
	CHECK_THROWS(chunks::UnpackImages(back.data(), back.size() - 1));
	// A layer missing : an error
	files[chunks::Top].clear();
	CHECK_FALSE(chunks::Decode(files).error.empty());
}

TEST_CASE("The queues give the chunks back, in order") {
	const auto folder = std::filesystem::temp_directory_path() / "sad_chunks_test";
	std::filesystem::create_directories(folder);
	const auto image = worldgen::RgbImage{ 2, 2, 1, 2, 3 };
	const auto paths = chunks::PathsOf(folder.string(), "map", (folder / "chip.png").string());
	for (auto i = 0; i < chunks::ImageCount; i++) {
		worldgen::WriteFile(paths[i], i >= chunks::Ground && i <= chunks::Top ? worldgen::EncodeBmp(image) : worldgen::EncodePng(image));
	}
	for (const auto synchronous : { true, false }) {
		auto queue = chunks::MakePrepareQueue(synchronous);
		queue->request(1, paths);
		queue->request(2, paths);
		auto ids = std::vector<std::uint64_t>{};
		for (auto i = 0; i < 2000 && ids.size() < 2; i++) {
			if (const auto prepared = queue->poll()) {
				CHECK(prepared->images.error.empty());
				ids.push_back(prepared->id);
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			}
		}
		CHECK(ids == std::vector<std::uint64_t>{ 1, 2 });
		// Destroyed with a request on the way : it does not wait for the others
		queue->request(3, paths);
	}
	std::filesystem::remove_all(folder);
}

TEST_CASE("The records of the entities are written and read") {
	auto store = chunks::EntityStore{};
	store.time = 123456;
	auto wild = chunks::EntityRecord{};
	wild.x = 3;
	wild.y = 39;
	wild.facing = 2;
	wild.monster = 16;
	wild.level = 7;
	wild.attitude = "fearful";
	wild.pathType = 1;
	wild.stay = 4000;
	wild.calm = 12;
	auto character = chunks::EntityRecord{};
	character.kind = chunks::EntityRecord::Kind::Character;
	character.sprite = -900;
	character.name = "item_w_1_2_3_4";
	character.script = "Levels/item.miniska";
	character.arguments = { "take", "0", "a\ttab", "a\nline \\ é", "" };
	character.movement = "path";
	character.path = { 0, 1, 2 };
	character.layer = 1;
	character.anchored = true;
	character.animatedWhenStill = true;
	store.chunks["w_1_2"] = { 100, { wild, character } };
	// A chunk known with nothing left
	store.chunks["w_room_0"] = { 5, {} };

	auto text = std::stringstream{};
	store.write(text);
	auto read = chunks::EntityStore{};
	read.read(text);
	CHECK(read.time == 123456);
	REQUIRE(read.chunks.size() == 2);
	CHECK(read.chunks["w_room_0"].entities.empty());
	const auto& chunk = read.chunks["w_1_2"];
	CHECK(chunk.left == 100);
	REQUIRE(chunk.entities.size() == 2);
	CHECK(chunk.entities[0].monster == 16);
	CHECK(chunk.entities[0].attitude == "fearful");
	CHECK(chunk.entities[0].stay == 4000);
	CHECK(chunk.entities[0].calm == 12);
	CHECK(chunk.entities[1].kind == chunks::EntityRecord::Kind::Character);
	CHECK(chunk.entities[1].arguments == character.arguments);
	CHECK(chunk.entities[1].path == character.path);
	CHECK(chunk.entities[1].anchored);

	// A broken line is left out
	auto broken = std::stringstream{ "time\t4\nchunk\tw_0_0\t1\nwild\tx\ngarbage\n" };
	read.read(broken);
	CHECK(read.time == 4);
	CHECK(read.chunks["w_0_0"].entities.empty());
}

TEST_CASE("The wild Pokemon leave when far and out of the screen") {
	// Near, seen or coming at the player : they stay
	CHECK_FALSE(chunks::Despawns(chunks::NearRadius, false, false, 0));
	CHECK_FALSE(chunks::Despawns(100.F, true, false, 0));
	CHECK_FALSE(chunks::Despawns(100.F, false, true, 0));
	// Between : by chance
	CHECK(chunks::Despawns(40.F, false, false, chunks::LeaveChance - 1));
	CHECK_FALSE(chunks::Despawns(40.F, false, false, chunks::LeaveChance));
	// Very far : at once
	CHECK(chunks::Despawns(chunks::FarRadius + 1.F, false, false, 99));
	CHECK_FALSE(chunks::Forgotten(chunks::ForgetAfter, 0));
	CHECK(chunks::Forgotten(chunks::ForgetAfter + 1, 0));
}
