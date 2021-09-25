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

#include "util.h"

const Policy supported_policies[SUPPORTED_POLICY_COUNT] {
    {0, "default"},
    {1, "turbo"},
    {2, "silent"},
};


