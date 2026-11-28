#include <istream>
#include <ostream>
#include <sstream>
#include "EntityRecords.h"

namespace {
	// A line : fields separated by tabs (a tab, a line break, a backslash in a field : \t, \n, \r, \\)
	std::string Escape(const std::string& value) {
		auto result = std::string{};
		for (const auto c : value) {
			switch (c) {
			case '\\': result += "\\\\"; break;
			case '\t': result += "\\t"; break;
			case '\n': result += "\\n"; break;
			case '\r': result += "\\r"; break;
			default: result += c;
			}
		}
		return result;
	}

	std::vector<std::string> Fields(const std::string& line) {
		auto result = std::vector<std::string>(1);
		for (auto i = std::size_t{ 0 }; i < line.size(); i++) {
			if (line[i] == '\t') {
				result.emplace_back();
			} else if (line[i] == '\\' && i + 1 < line.size()) {
				const auto c = line[++i];
				result.back() += c == 't' ? '\t' : c == 'n' ? '\n' : c == 'r' ? '\r' : c;
			} else {
				result.back() += line[i];
			}
		}
		return result;
	}
}

void chunks::EntityStore::write(std::ostream& out) const {
	out << "time\t" << time << "\n";
	for (const auto& [name, chunk] : chunks) {
		out << "chunk\t" << Escape(name) << "\t" << chunk.left << "\n";
		for (const auto& e : chunk.entities) {
			if (e.kind == EntityRecord::Kind::Wild) {
				out << "wild\t" << e.x << "\t" << e.y << "\t" << e.facing << "\t" << e.monster << "\t" << e.level << "\t" << Escape(e.attitude)
					<< "\t" << e.pathType << "\t" << e.stay << "\t" << e.calm << "\n";
			} else {
				out << "character\t" << e.x << "\t" << e.y << "\t" << e.facing << "\t" << e.sprite << "\t" << Escape(e.name) << "\t" << Escape(e.script)
					<< "\t" << Escape(e.movement) << "\t" << e.layer << "\t" << e.anchored << "\t" << e.animatedWhenStill << "\t" << e.path.size();
				for (const auto direction : e.path) {
					out << "\t" << direction;
				}
				for (const auto& argument : e.arguments) {
					out << "\t" << Escape(argument);
				}
				out << "\n";
			}
		}
	}
}

void chunks::EntityStore::read(std::istream& in) {
	time = 0;
	chunks.clear();
	auto line = std::string{};
	ChunkRecord* current = nullptr;
	while (std::getline(in, line)) {
		const auto fields = Fields(line);
		const auto number = [&fields](std::size_t i) { return std::stoll(fields.at(i)); };
		const auto integer = [&number](std::size_t i) { return static_cast<int>(number(i)); };
		try {
			if (fields[0] == "time") {
				time = number(1);
			} else if (fields[0] == "chunk") {
				current = &chunks[fields.at(1)];
				current->left = number(2);
			} else if (current != nullptr && (fields[0] == "wild" || fields[0] == "character")) {
				auto e = EntityRecord{};
				e.x = integer(1);
				e.y = integer(2);
				e.facing = integer(3);
				if (fields[0] == "wild") {
					e.monster = integer(4);
					e.level = integer(5);
					e.attitude = fields.at(6);
					e.pathType = integer(7);
					e.stay = integer(8);
					e.calm = static_cast<unsigned int>(number(9));
				} else {
					e.kind = EntityRecord::Kind::Character;
					e.sprite = integer(4);
					e.name = fields.at(5);
					e.script = fields.at(6);
					e.movement = fields.at(7);
					e.layer = integer(8);
					e.anchored = integer(9) != 0;
					e.animatedWhenStill = integer(10) != 0;
					const auto steps = static_cast<std::size_t>(number(11));
					for (auto i = std::size_t{ 0 }; i < steps; i++) {
						e.path.push_back(integer(12 + i));
					}
					for (auto i = 12 + steps; i < fields.size(); i++) {
						e.arguments.push_back(fields[i]);
					}
				}
				current->entities.push_back(std::move(e));
			}
		} catch (const std::exception&) {
			// A broken line (a save written by an older game) : left out
		}
	}
}
