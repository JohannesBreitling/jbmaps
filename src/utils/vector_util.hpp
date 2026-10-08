
#include <cstdint>
#include <fstream>
#include <vector>
#include <cassert>

#pragma once

template <typename T>
void write_vector_to_file(const std::vector<T> &vec, const std::string &path) {
    std::ofstream out(path, std::ios::binary);

    if (!out) {
        std::cerr << "Something went wrong creating / opening the output file.\n";
        exit(1);
    }

    uint32_t size = vec.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!vec.empty()) {
        out.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(T));
    }
    out.close();
}

template <typename T>
std::vector<T> read_vector_from_file(const std::string &path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "Something went wrong opening the file.\n";
        exit(1);
    };

    uint32_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::vector<T> vec(size);
    
    if (size > 0) {
        in.read(reinterpret_cast<char*>(vec.data()), size * sizeof(T));
    }
    
    in.close();
    return vec;
}

template <typename T, typename P>
void print_vector(const std::vector<T> &vec, P printer, uint32_t n) {
    assert(n <= vec.size());
    
    std::string sep = "";
    std::cout << "[";

    for (uint32_t i = 0; i < n; i++) {
        std::cout << sep;
        sep = ", ";
        std::cout << printer(vec[i]);
    }
    
    std::cout << "]\n";
}

template <typename T, typename P>
void print_vector(const std::vector<T> &vec, P printer) {
    print_vector(vec, printer, vec.size());
};

template<class T>
std::vector<T>read_vector_from_file_off(const std::string&file_name){
	std::ifstream in(file_name, std::ios::binary);
	if(!in)
		throw std::runtime_error("Can not open \""+file_name+"\" for reading.");
	in.seekg(0, std::ios::end);
	unsigned long long file_size = in.tellg();
	if(file_size % sizeof(T) != 0)
		throw std::runtime_error("File \""+file_name+"\" can not be a vector of the requested type because it's size is no multiple of the element type's size.");
	in.seekg(0, std::ios::beg);
	std::vector<T>vec(file_size / sizeof(T));
	in.read(reinterpret_cast<char*>(&vec[0]), file_size);
	return vec; // NVRO
}

void write_string_list_to_file(const std::vector<std::string> &list, const std::string &path) {
    std::ofstream outFile(path);
    for (const auto line : list) {
        outFile << line << "\n";    
    }
    outFile.close();
}

std::vector<std::string> read_string_list_from_file(const std::string &path) {
    std::vector<std::string> result;
    std::ifstream inFile(path);

    if (!inFile)
        throw std::runtime_error("Can not open file for reading!");

    std::string line;
    while (std::getline(inFile, line)) {
        result.push_back(line);
    }

    return result;
}