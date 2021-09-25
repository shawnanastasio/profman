/**
 * Copyright 2021 Shawn Anastasio.
 *
 * This file is part of profman.
 *
 * profman is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * profman is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with profman.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdlib>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

struct Policy {
    unsigned long number;
    const char *name;
};
static constexpr size_t SUPPORTED_POLICY_COUNT = 3;

extern const Policy supported_policies[SUPPORTED_POLICY_COUNT];

[[maybe_unused]] static const char *KERN_THERMAL_POLICY_PATH = "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy";
