#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <variant>
#include <iostream>
#include <unordered_map>

struct key_value {
	std::string key;
	std::variant<std::string, double, int, bool> value;
};

const std::unordered_map<std::string, std::string> pair_types = {
	{"updatesPerSecond", "int"},
	{"simulationTime", "int"},
	{"templateFile", "string"},
	{"gravity", "bool"}
};

std::vector<std::pair<std::string, std::string>> read(std::string file_path) {
	std::vector<std::pair<std::string, std::string>> config;
	std::ifstream t(file_path);

	if (!t.is_open()) {
		return config;
	}

	std::stringstream buffer;
	buffer << t.rdbuf();

	std::string line;
	std::istringstream s(buffer.str());
	std::string commentKey("#");

	while (std::getline(s, line)) {
		if (!line.empty() && line.compare(0, commentKey.length(), commentKey)) {
			std::vector<std::string> segList;
			std::string segment;
			std::istringstream ss(line);

			while (std::getline(ss, segment, '=')) {
				std::stringstream trimmer;
				trimmer << segment;
				segment.clear();
				trimmer >> segment;

				segList.push_back(segment);
			}

			if (segList.size() == 2) {
				config.push_back(std::pair(segList[0], segList[1]));
			}
		}
	}

	return config;
}

std::vector<key_value> parse(const std::vector<std::pair<std::string, std::string>>& pairs) {
	std::vector<key_value> values;

	for (auto& i : pairs) {
		if (pair_types.find(i.first) == pair_types.end()) {
			values.clear();
			std::cerr << "Invalid key: \"" << i.first << "\".\n";
			return values;
		}else{
			if (pair_types.at(i.first) == "int") {
				values.push_back({i.first, std::stoi(i.second)});
			}else if (pair_types.at(i.first) == "string") {
				values.push_back({i.first, i.second});
			}else if (pair_types.at(i.first) == "double") {
				values.push_back({i.first, std::stod(i.second)});
			}else if (pair_types.at(i.first) == "bool") {
				if (i.second == "true") {
					values.push_back({i.first, true});
				}else{
					values.push_back({i.first, false});
				}
			}
		}
	}

	return values;
}

key_value find_element(const std::vector<key_value>& values, const std::string& key) {
	for (auto i : values) {
		if (i.key == key) {
			return i;
		}
	}

	return {"", 0};
}