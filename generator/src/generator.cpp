/**
 * generator.cpp
 *
 * Copyright (C) 2026 gdar463 <dev@gdar463.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with this program. If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include <libdeflate.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace std {
namespace fs = std::filesystem;
}

#define INCLUDE_FOR_byte_t "#include <cstdint>\n"
#define byte_t std::uint8_t

#define _STR(m_p) #m_p
#define _MKSTR(m_p) _STR(m_p)

libdeflate_compressor *compressor = libdeflate_alloc_compressor(6);
void cleanup() {
	libdeflate_free_compressor(compressor);
	compressor = nullptr;
}

void replace(std::string &p_str, const std::string &p_from, const std::string &p_to) {
	for (size_t pos = 0; pos = p_str.find(p_from, pos), pos != std::string::npos; pos += p_to.size()) {
		p_str.replace(pos, p_from.size(), p_to);
	}
}

std::string canonicalize(std::string p_path) {
#if defined(WIN32)
	replace(p_path, "\\", "/");
#endif

	replace(p_path, "\"", "\\\"");
	return p_path;
}

int main() {
	std::ofstream output{ EMBDFS_GENERATED_RESOURCES };

	std::cout << "[embdfs]: resource path set to  EMBDFS_PATH " << std::endl;
	if (!std::fs::exists(EMBDFS_PATH)) {
		std::cerr << "[embdfs]: resource path doesn't exist" << std::endl;
		cleanup();
		return 1;
	}

	output << "#include <embdfs.hpp>\n\n";
	output << "#include <array>\n";
	output << "#include <span>\n";
	output << "#include <string>\n";
	output << INCLUDE_FOR_byte_t "\n";

	std::vector<std::fs::path> paths{};
	int n = 0;
	for (const std::fs::directory_entry &entry : std::fs::recursive_directory_iterator(EMBDFS_PATH)) {
		if (!entry.is_regular_file()) {
			continue;
		}

		std::fs::path path = std::fs::canonical(std::fs::absolute(entry.path()));
		std::fs::path relative = std::fs::relative(path, std::fs::absolute(EMBDFS_PATH));

		std::cout << "[embdfs]: embedding " << relative << std::endl;
		std::vector<byte_t> data(entry.file_size());

		std::ifstream input{ entry.path() };
		input.read((char *)data.data(), entry.file_size());
		input.close();

		data.push_back(0);

		std::vector<byte_t> compressed_data(entry.file_size() + 100);
		size_t compressed_bytes = 0;
		int retries = 0;
	retry:
		compressed_bytes = libdeflate_zlib_compress(compressor, data.data(), data.size(), compressed_data.data(), compressed_data.size());
		// libdeflare returns 0 if out_size was too small, so we increase it by a factor and retry
		if (compressed_bytes == 0) {
			std::cerr << "[embdfs]: failed to compress data with out_size " << compressed_data.size() + 100;
			// but it could be signs of error, so we stop after the 3rd attempt
			if (retries >= 2) {
				std::cerr << std::endl
						  << "[embdfs]: 3rd attempt, crashing" << std::endl;
				cleanup();
				return 2;
			}
			retries++;
			compressed_data.resize(entry.file_size() * 1.25f);
			std::cerr << ", retrying with " << compressed_data.size() << std::endl;
			goto retry;
		}
		compressed_data.resize(compressed_bytes);

		output << "// Resource for path: " << relative << "\n";
		output << "static std::array<std::uint8_t, " << compressed_bytes + 1 << "> resource_" << n << " = {\n  ";
		for (byte_t byte : compressed_data) {
			output << static_cast<uint64_t>(byte) << ",";
		}
		output << " };\n\n";

		paths.push_back(canonicalize(relative.string()));
		n++;
	}

	output << "\n";

	output << "std::span<std::string> embdfs::get_paths() {\n";
	output << "  static std::array<std::string, " << n << "> paths = {\n";
	for (const std::fs::path &path : paths) {
		output << "    " << path << ",\n";
	}
	output << "  };\n";
	output << "  return paths;\n";
	output << "}\n\n";

	output << "std::span<embdfs::ResourceLocation> embdfs::get_resources() {\n";
	output << "  static std::array<embdfs::ResourceLocation, " << n << "> resources = {\n";
	for (size_t i = 0; i < n; i++) {
		output << "    embdfs::ResourceLocation{" << paths[i] << ",embdfs::Resource({ resource_" << i << ".data(), resource_" << i << ".size() - 1})},\n ";
	}
	output << "  };\n";
	output << "  return resources;\n";
	output << "}\n\n";

	cleanup();
	return 0;
}
