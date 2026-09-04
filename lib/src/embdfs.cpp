/**
 * embdfs.cpp
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

#include "embdfs.hpp"

namespace embdfs {

const byte_t *Resource::data() const {
	if (decompress_if_needed() != libdeflate_result::LIBDEFLATE_SUCCESS) {
		return nullptr;
	}
	return decompressed_content.data();
}
size_t Resource::size() const {
	if (decompress_if_needed() != libdeflate_result::LIBDEFLATE_SUCCESS) {
		return 0;
	}
	return decompressed_content.size();
}

libdeflate_result Resource::decompress_if_needed() const {
	if (!decompressor) {
		abort();
	}
	if (!decompressed_content.empty()) {
		return libdeflate_result::LIBDEFLATE_SUCCESS;
	}

	decompressed_content.resize(decompressed_content.size() * 2);
	size_t decompressed_bytes = 0;
	libdeflate_result err;
retry:
	err = libdeflate_zlib_decompress(decompressor, content.data(), content.size(), decompressed_content.data(), decompressed_content.size(), &decompressed_bytes);
	if (err == libdeflate_result::LIBDEFLATE_INSUFFICIENT_SPACE) {
		decompressed_content.resize(decompressed_bytes);
		goto retry;
	} else if (err == libdeflate_result::LIBDEFLATE_SUCCESS) {
		decompressed_content.resize(decompressed_bytes);
	}
	return err;
}

const Resource *get(const std::filesystem::path &p_path) {
	for (const ResourceLocation &resource_location : get_resources()) {
		if (resource_location.path == p_path) {
			return &resource_location.resource;
		}
	}
	return nullptr;
}

void setup() {
	decompressor = libdeflate_alloc_decompressor();
}
void cleanup() {
	libdeflate_free_decompressor(decompressor);
}

} //namespace embdfs
