#include <embdfs.hpp>

#include <iostream>

int main() {
	embdfs::setup();
	const embdfs::Resource *resource = embdfs::get("test.txt");
	std::cout << resource->size() << std::endl;
	std::cout << resource->data() << std::endl;
	embdfs::cleanup();
	return 0;
}
