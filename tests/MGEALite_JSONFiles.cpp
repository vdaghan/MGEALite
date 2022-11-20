#include <fstream>

#include <nlohmann/json.hpp>
using JSON = nlohmann::ordered_json;
#include <gtest/gtest.h>

TEST(JSONFiles, RegularInput) {
	std::ifstream f;
	f.open("testdata/Regular.input", std::ios_base::in);
	if (!f.is_open()) {
		FAIL() << "could not open file";
	}
	decltype(nlohmann::json::parse(f)) parseResult;
	try {
		parseResult = nlohmann::json::parse(f);
	}
	catch (const std::exception&) {
		FAIL() << "could not parse file";
	}
	f.close();
}

TEST(JSONFiles, RegularOutput) {
	std::ifstream f;
	f.open("testdata/Regular.output", std::ios_base::in);
	if (!f.is_open()) {
		FAIL() << "could not open file";
	}
	decltype(nlohmann::json::parse(f)) parseResult;
	try {
		parseResult = nlohmann::json::parse(f);
	}
	catch (const std::exception&) {
		FAIL() << "could not parse file";
	}
	f.close();
}

TEST(JSONFiles, RegularJSON) {
	std::ifstream f;
	f.open("testdata/Regular.json", std::ios_base::in);
	if (!f.is_open()) {
		FAIL() << "could not open file";
	}
	decltype(nlohmann::json::parse(f)) parseResult;
	try {
		parseResult = nlohmann::json::parse(f);
	}
	catch (const std::exception&) {
		FAIL() << "could not parse file";
	}
	f.close();
}

TEST(JSONFiles, SingleTimestepOutput) {
	std::ifstream f;
	f.open("testdata/SingleTimestep.output", std::ios_base::in);
	if (!f.is_open()) {
		FAIL() << "could not open file";
	}
	decltype(nlohmann::json::parse(f)) parseResult;
	try {
		parseResult = nlohmann::json::parse(f);
	} catch (const std::exception&) {
		FAIL() << "could not parse file";
	}
	f.close();
}
