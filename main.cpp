#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <print>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

const std::string CONFIG_FILE_NAME = "config.json";
const std::string DEFAULT_CONFIG = R"(
{
  "default": true
}
)";

int main(int argc, char **argv) {
  std::vector<std::string> arguments = {argv + 1, argv + argc};
  // Make sure we have args
  if (arguments.begin() == arguments.end()) {
    std::println("no args passed, printing help");
    return 0;
  }

  // Check for -h and -v
  for (auto it = arguments.begin(); it != arguments.end(); it++) {

    if (*it == "--help" || *it == "-h") {
      std::println("help");
      return 0;
    }

    if (*it == "--version" || *it == "-v") {
      std::println("0.1");
      return 0;
    }
  }

  // Check for Home
  std::string config_file_directory;
  std::string config_file_path;
  if (const auto home = std::getenv("HOME")) {
    config_file_directory = home + std::string("/.config/templater/");
    config_file_path = config_file_directory + CONFIG_FILE_NAME;
  } else {
    std::println("$HOME not set! Quitting...");
    return 1;
  }

  // Check if config directory Exists, Create if not
  if (!std::filesystem::exists(config_file_directory)) {
    std::println("Templater folder not found in {}. Creating...",
                 config_file_directory);
    std::filesystem::create_directory(config_file_directory);
  }

  // Check if config file Exists, Create if not
  json config_json;
  if (std::filesystem::exists(config_file_path)) {
    std::ifstream json_file_handle(config_file_path);
    config_json = json::parse(json_file_handle);
    json_file_handle.close();
  } else {
    std::println("templater.json not found, generating default config.");

    std::ofstream created_config_file(config_file_path);
    created_config_file << DEFAULT_CONFIG;
    created_config_file.close();

    std::println("Created default config at {}", config_file_path);
  }

  // Check if default config hasn't been changed.
  for (json::iterator it = config_json.begin(); it != config_json.end(); ++it) {
    if (it.key() == "default" || it.value() == "true") {
      std::println("Default config detected!");
      std::println("Use -h for help or check "
                   "https://github.com/oniband/Templater/tree/main for a "
                   "configuration guide!");
      return 0;
    }
  }

  // Make sure the first argument has a corresponding config
  std::string chosen_template;
  auto argument_iterator = arguments.begin();

  if (config_json.contains(*argument_iterator)) {
    chosen_template = *argument_iterator;
    if (std::next(argument_iterator) == arguments.end()) {
      std::println("Too few arguments, run \'templater -h\' for usage guide.");
      return 0;
    }
  } else {
    std::println("No config called {} found", *argument_iterator);
    return 1;
  }


  
  return 0;
}
