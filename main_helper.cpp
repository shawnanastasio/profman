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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <unistd.h>
#include <fcntl.h>

#include "util.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <policy #>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *endptr;
    unsigned long policy_number = strtoul(argv[1], &endptr, 10);
    if (*argv[1] == '\0' || *endptr != '\0') {
        fprintf(stderr, "Invalid policy number specified.\n");
        return EXIT_FAILURE;
    }

    // Lookup policy number
    const Policy *policy = nullptr;
    for (auto &pol : supported_policies) {
        if (policy_number == pol.number) {
            policy = &pol;
            break;
        }
    }
    if (!policy) {
        fprintf(stderr, "Unknown policy number: %lu\n", policy_number);
        return EXIT_FAILURE;
    }

    // Open file handle to kernel thermal policy
    int policyfd = open(KERN_THERMAL_POLICY_PATH, O_RDWR);
    if (policyfd < 0) {
        fprintf(stderr, "Unable to open \"%s\": %m\n", KERN_THERMAL_POLICY_PATH);
        return EXIT_FAILURE;
    }

    std::string policy_num_str = std::to_string(policy->number);
    if (write(policyfd, policy_num_str.c_str(), policy_num_str.size()) != (ssize_t)policy_num_str.size()) {
        fprintf(stderr, "Failed to write updated policy: %m\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
