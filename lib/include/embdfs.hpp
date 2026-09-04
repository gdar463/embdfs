#pragma once

/**
 * embdfs.hpp
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

#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

#define byte_t std::uint8_t

namespace embdfs {
static libdeflate_decompressor *decompressor = nullptr;

class Resource {
public:
	constexpr Resource(const std::span<byte_t> &p_content) : content(p_content) {}

	const byte_t *data() const;
	size_t size() const;

private:
	libdeflate_result decompress_if_needed() const;

	mutable std::vector<byte_t> decompressed_content;
	const std::span<const byte_t> content;
};

class ResourceLocation {
public:
	const std::string path;
	const Resource resource;

	constexpr ResourceLocation(const std::string &p_path, const Resource &p_resource) : path(p_path), resource(p_resource) {}
};

std::span<std::string> get_paths();
std::span<ResourceLocation> get_resources();

const Resource *get(const std::filesystem::path &p_path);
void setup();
void cleanup();
} //namespace embdfs
