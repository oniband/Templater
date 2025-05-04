# Templater
A program that makes starting new projects with annoying startups easier. Configured in a single json file from .config/templater.
The only real reason for this project to exist is that I want to learn more C++ but setting up new openGL projects annoys me.

This could absolutely be a bash script but I just want an excuse to program something.
# JSON structure
```json
{
    "templates": 
                "cpp": { "sourcedir": "~/templates/cpp"},
                "opengl": { "sourcedir": "~/templates/opengl"},
}
```
# Examples
```bash
templater opengl . new_project
```
This would create a new openGL project at your current directory with the folder name of new_project

# MVP
- Should be able to parse command line arguments
- Should be able to parse a JSON files in .config and respond to those values
- Should be able to create new directories based on the configuration file and command line arguments
- Should be a single binary that I can just compile and forget.
