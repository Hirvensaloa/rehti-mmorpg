#pragma once

#include <network.h>

#include "RehtiReader.hpp"

/**
 * @brief Convert action to animation config
 * @param action CurrentAction
 * @param entityLocation Coordinates
 * @return AnimationConfig
 */
AnimationConfig actionToAnimationConfig(CurrentAction action, Coordinates entityLocation);