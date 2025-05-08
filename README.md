# Templater
A program that makes starting new projects with annoying startups easier. Configured in a single json file from .config/templater.
The only real reason for this project to exist is that I want to learn more C++ but setting up new openGL projects annoys me.

This could absolutely be a bash script but I just want an excuse to program something.

# Configuration
Configuration is done via config.json in ~/.config/templater

your config file should look something like this.
```json
{
    "c++" : {"source_directory" : "~/Templates/c++"},
    "opengl" : {"source_directory" : "~/Templates/opengl"},
}
```

You can have as many templates and source source directories as you want.
Also note that relative directories with ~ are allowed.

That's it!

# Building
Dependencies: nlohmann/json and a c++23 compatible compiler.

If you don't have nlohmann/json installed as a system dependency, you'll have to change the include line in main.cpp to a relative location 

## example
`clang++ -std=c++23 main.cpp -o templater`
