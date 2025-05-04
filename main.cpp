#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
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
  std::string config_directory;
  std::string config_path;
  // Check for Home
  if (const auto home = std::getenv("HOME")) {
    config_directory = home;
    config_directory += "/.config/templater/";
    config_path = config_directory + CONFIG_FILE_NAME;
  } else {
    std::println("$HOME not set! Quitting...");
    return 1;
  }

  // Check args
  std::vector<std::string> arguments = {argv + 1, argv + argc};
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

  // Check if config directory Exists, Create if not
  if (!std::filesystem::exists(config_directory)) {
    std::println("Templater folder not found in {}. Creating...",
                 config_directory);
    std::filesystem::create_directory(config_directory);
  }

  // Check if config file Exists, Create if not
  json config_file_json;
  if (std::filesystem::exists(config_path)) {
    std::ifstream json_file(config_path);
    config_file_json = json::parse(json_file);
    json_file.close();
  } else {
    std::println("templater.json not found, generating default config.");

    std::ofstream created_config_file(config_path);
    created_config_file << DEFAULT_CONFIG;
    created_config_file.close();

    std::println("Created default config at {}", config_path);
  }

  // Check if default config hasn't been changed.
  for (json::iterator it = config_file_json.begin(); it != config_file_json.end(); ++it) {
    if (it.key() == "default" || it.value() == "true") {
      std::println("Default config not changed.");
      std::println("Use -h for help or check the github for a configuration guide!");
      return 0;
    }
  }
  return 0;
}
