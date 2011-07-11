#ifndef __COMMON_H
#define __COMMON_H

#pragma warning(disable : 4244 4267 4172 4018 4530)

#define W_WIDTH 32768
#define W_HEIGHT 1500
#define GRID_BLOCK 128
#define GRID_WIDTH W_WIDTH/GRID_BLOCK
#define GRID_HEIGHT W_HEIGHT/GRID_BLOCK

#include <set>
#include <list>
#include <boost/unordered_map.hpp>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "string.h"
#include "surface.h"
#include "template.h"

#include "vec2.h"

#include "log.h"
#include "scenenode.h"
#include "scene_manager.h"

#include "cvar_headers.h"

#include "shape.h"
#include "rasterizer.h"
#include "physics.h"
#include "player.h"

#include "input.h"

#include "state_manager.h"

#include "game.h"

#include "debugconsole.h"

#endif