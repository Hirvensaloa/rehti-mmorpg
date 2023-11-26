#pragma once

#include <network.h>

#include "RehtiReader.hpp"

/**
 * @brief Convert action to animation config
 * @param action CurrentAction
 * @return AnimationConfig
 */
AnimationConfig actionToAnimationConfig(CurrentAction action);