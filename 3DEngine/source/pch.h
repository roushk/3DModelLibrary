//*****************************************************//
// Coleman Jonas
// 3DEngine
//*****************************************************//


#pragma once
#define _CRT_SECURE_NO_WARNINGS

// Windows includes
#define ENGINE_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef near
#undef far
#undef GetObject


// SDL2 library
#define ENGINE_SDL
#include <SDL2/SDL.h>

// GLEW and OpenGL
#define ENGINE_GL
#include <GL/glew.h>

// SOIL
#define ENGINE_SOIL
#include <SOIL/SOIL.h>

// ASSIMP AND ZLIB
#define ENGINE_ASSIMP
#include <assimp/TinyFormatter.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// GLM 
#define ENGINE_GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

// IMGUI
#define ENGINE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_stl.h"

// JSON 
#define ENGINE_JSON
#include <JSON/json.hpp>
#include <MetaStuff/Meta.h>
namespace json = nlohmann;


// FMOD
#define ENGINE_FMOD
#include <FMOD/fmod.hpp>
#include <FMOD/fmod.h>

// SPDLOG
#define ENGINE_SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/details/pattern_formatter.h>

// RTTR
#define ENGINE_RTTR
#include <rttr/registration.h>
#include <rttr/type.h>

#define ENGINE_FREETYPE
#include "ft2build.h"
#include FT_FREETYPE_H

// C headers
#define ENGINE_C
#include <cmath>
#include <cassert>

// STL includes
#define ENGINE_STL
#include <algorithm>
#include <vector>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <set>
#include <regex>
#include <stdexcept>
#include <type_traits>
#include <random>
#include <optional>
#include <any>
#include <unordered_set>
#include <experimental/filesystem> // C++-standard header file name  
#include <filesystem> // Microsoft-specific implementation header file name  
namespace filesystem = std::experimental::filesystem::v1;
using namespace std::chrono_literals;

#include "singleton.h"
#include "InputManager.h"
#include "Render.h"
#include "GUI.h"
#include "Wireframe.h"
#include "Camera.h"
#include "Light.h"

//#include <vector>
//#include <GL/glew.h>
//#include <glm/gtc/type_ptr.hpp>
