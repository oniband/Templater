#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <print>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace fs = std::filesystem;

const std::string CONFIG_FILE_NAME = "config.json";
const std::string DEFAULT_CONFIG = R"(
{
  "default": true
}
)";

void recursive_copy(const fs::path &source, const fs::path &destination);

int main(int argc, char **argv) {
  std::vector<std::string> arguments = {argv + 1, argv + argc};
  // Make sure we have args
  if (arguments.begin() == arguments.end()) {
    std::println("no args passed, printing help");
    return EXIT_SUCCESS;
  }

  // Check for -h and -v
  for (auto it = arguments.begin(); it != arguments.end(); it++) {
    if (*it == "--help" || *it == "-h") {
      std::println("help");
      return EXIT_SUCCESS;
    }

    if (*it == "--version" || *it == "-v") {
      std::println("0.1");
      return EXIT_SUCCESS;
    }
  }

  // Check for Home
  std::string config_file_directory;
  std::string config_file_path;
  const auto HOME = std::getenv("HOME");
  if (HOME) {
    config_file_directory = HOME + std::string("/.config/templater/");
    config_file_path = config_file_directory + CONFIG_FILE_NAME;
  } else {
    std::println("$HOME not set! Quitting...");
    return EXIT_FAILURE;
  }

  // Check if config directory Exists, Create if not
  if (!fs::exists(config_file_directory)) {
    std::println("Templater folder not found in {}. Creating...",
                 config_file_directory);
    fs::create_directory(config_file_directory);
  }

  // Check if config file Exists, Create if not
  json config_json;
  if (fs::exists(config_file_path)) {
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
      return EXIT_SUCCESS;
    }
  }

  // Make sure the first argument has a corresponding config with an object
  std::string chosen_template;
  json chosen_template_options;
  auto argument_iterator = arguments.begin();

  if (config_json.contains(*argument_iterator) &&
      config_json[*argument_iterator].type() == json::object()) {
    chosen_template = *argument_iterator;
    chosen_template_options = config_json[*argument_iterator];
  } else {
    std::println("No template called {} found with matching configuration",
                 *argument_iterator);
    return EXIT_FAILURE;
  }

  // Advance arg iterator
  if (std::next(argument_iterator) == arguments.end()) {
    std::println("Too few arguments, run \'templater -h\' for usage guide.");
    return EXIT_FAILURE;
  } else {
    argument_iterator++;
  }

  // Check destination arguments. handle HOME and .(current dir)
  std::string template_destination;
  std::string second_argument = *argument_iterator;
  if (*argument_iterator == ".") {
    template_destination = fs::current_path();
  } else if (fs::exists(second_argument)) {
    template_destination = second_argument;
  } else {
    std::println("Mangled destination specifier");
    return EXIT_FAILURE;
  }

  // Advance arg iterator
  if (std::next(argument_iterator) == arguments.end()) {
    std::println("Too few arguments, run \'templater -h\' for usage guide.");
    return EXIT_FAILURE;
  } else {
    argument_iterator++;
  }

  // Validate template
  std::vector<std::string> directories;
  for (json::iterator it = chosen_template_options.begin();
       it != chosen_template_options.end(); ++it) {
    if (it.key() == "source_directory") {
      // Replace ~ with HOME
      std::string dir = it.value();
      if (dir.find("~") == 0) {
        dir.erase(0, 1);
        if (fs::exists(HOME + dir)) {
          directories.push_back(HOME + dir);
        } else {
          std::println("Invalid use of ~ found in {}", chosen_template);
          std::println("Quitting...");
          return EXIT_FAILURE;
        }
      } else if (fs::exists(dir)) {
        directories.push_back(dir);
      } else {
        std::println("Directory {} not found in {} template", dir,
                     chosen_template);
        std::println("Quitting...");
        return EXIT_FAILURE;
      }
    } else {
      std::println("invalid option detected in config: {}", it->dump());
      std::println("Quitting...");
      return EXIT_FAILURE;
    }
  }

  // Copy over files to new project
  std::string project_path = template_destination + "/" + *argument_iterator;
  for (auto dir_it = directories.begin(); dir_it != directories.end();
       ++dir_it) {
    std::string source_dir = *dir_it;
    // recursive_copy(source_dir, project_path);
    std::string command = "cp -r " + source_dir + " " + project_path;
    try {
        std::system(command.c_str());
    } catch (const std::system_error& e) {
      std::print("Error in copying files: \n{}\n", e.what());
    }
  }
  return 0;
}
