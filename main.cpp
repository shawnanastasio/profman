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

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>

extern "C" {
#include <libnotify/notify.h>
}

#include "util.h"

static const char *PROGRAM_NAME = "profman";

static void help(const char *);
static void usage(const char *);
static bool do_get_policy();
static bool do_set_next_policy();
static bool do_set_policy(const char *selected_policy);
void display_notification(const char *message);

enum ProgramOption {
    OptNone,
    OptHelp,
    OptGet,
    OptSet,
    OptNext,
};

int main(int argc, char **argv) {
    ProgramOption selected_option = OptNone;
    char *progname = argv[0];
    char *selected_policy = nullptr;

    if (!notify_init(PROGRAM_NAME))
        fprintf(stderr, "Warning: Unable to initialize libnotify: %m.\n");

    for (;;) {
        static struct option long_options[] = {
            {"help", no_argument, nullptr, OptHelp},
            {"get", no_argument, nullptr, OptGet},
            {"set", required_argument, nullptr, OptSet},
            {"next", no_argument, nullptr, OptNext},
        };

        int c = getopt_long(argc, argv, "hgs:n", long_options, nullptr);
        switch (c) {
            case -1:
                goto out;

            case OptHelp:
            case 'h':
                help(argv[0]);
                break;

            case OptGet:
            case 'g':
                selected_option = OptGet;
                goto out;

            case OptSet:
            case 's':
                selected_policy = strdup(optarg);
                assert(selected_policy);
                selected_option = OptSet;
                break;

            case OptNext:
            case 'n':
                selected_option = OptNext;
                break;

            default:
                usage(progname);
                exit(EXIT_FAILURE);
        }
    }
out:

    if (selected_option == OptNone) {
        usage(progname);
        exit(EXIT_FAILURE);
    }

    bool res = false;
    switch (selected_option) {
        case OptGet:
            res = do_get_policy();
            break;

        case OptSet:
            res = do_set_policy(selected_policy);
            break;

        case OptNext:
            res = do_set_next_policy();
            break;

        default:
            abort(); /* ?? */
    }

    return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void usage(const char *progname) {
    printf("Usage: %s <-h>/<-g>/<-p profile>/<-n>\nSee %s --help for more information.\n", progname, progname);
}

static void help(const char *progname) {
    printf("Usage: %s <-h>/<-g>/<-p profile>/<-n>\n", progname);
    printf("Manage the current thermal throttle policy on some ASUS laptops.\n\n");
    printf("Supported arguments:\n"
    "   -h, --help          Display this help message\n"
    "   -g, --get           Get the current policy\n"
    "   -s <p>, --set <p>   Set the current policy. <p> is either policy name or number.\n"
    "   -n, --next          Increment the current policy to the next one.\n"
    );
    printf("\n\nFor more information or to submit bug reports: https://github.com/shawnanastasio/profman\n");
    exit(EXIT_SUCCESS);
}

const Policy *get_current_policy() {
    char buf[10];
    ssize_t n;
    char *endptr;
    unsigned long policy;
    const Policy *res = nullptr;

    // Open file handle to kernel thermal policy
    int policyfd = open(KERN_THERMAL_POLICY_PATH, O_RDONLY);
    if (policyfd < 0) {
        fprintf(stderr, "Unable to open \"%s\": %m\n", KERN_THERMAL_POLICY_PATH);
        goto out;
    }

    if ((n = read(policyfd, buf, sizeof(buf))) < 1) {
        fprintf(stderr, "Failed to read current throttle policy: %m\n");
        goto out_open;
    }
    buf[n] = '\0';

    policy = strtoul(buf, &endptr, 10);
    if (endptr == buf) {
        fprintf(stderr, "Failed to parse current throttle policy from kernel.\n");
        goto out_open;
    }

    if (policy > ARRAY_SIZE(supported_policies)) {
        fprintf(stderr, "Currently set policy is not known: %lu\n", policy);
        goto out_open;
    }

    res = &supported_policies[policy];

out_open:
    close(policyfd);
out:
    return res;
}

static bool do_get_policy() {
    const Policy *policy = get_current_policy();
    printf("Current thermal_throttle_policy: %ld (%s)\n", policy->number, policy->name);

    return true;
}

static bool do_set_policy(const char *selected_policy) {
    const Policy *policy = nullptr;
    for (const auto &cur : supported_policies) {
        if (!strcasecmp(selected_policy, cur.name)
                || !strcmp(selected_policy, std::to_string(cur.number).c_str())) {
            policy = &cur;
        }
    }

    if (!policy) {
        fprintf(stderr, "Unknown policy: %s\n", selected_policy);
        fprintf(stderr, "Known policies: {");
        for (size_t i = 0; i < ARRAY_SIZE(supported_policies); i++) {
            fprintf(stderr, "%s%s", supported_policies[i].name, i == ARRAY_SIZE(supported_policies) - 1 ? "}\n" : ",");
        }
        return false;
    }

    FILE *fp = popen(std::string{std::string{PROFMAN_HELPER_BIN_PATH " "} + std::to_string(policy->number) + " 2>&1"}.c_str(), "r");
    if (!fp) {
        fprintf(stderr, "Failed to run 'profman_helper' program: %m\n");
        return false;
    }

    char output[128];
    fgets(output, sizeof(output), fp);
    int status = pclose(fp);
    if (status == -1) {
        fprintf(stderr, "Error occurred while running helper: %m.\n");
        return false;
    }
    assert(WIFEXITED(status));

    if (WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Helper reported error: %s", output);
        return false;
    }

    char *msg = nullptr;
    asprintf(&msg, "thermal_throttle_policy updated to: %lu (%s)", policy->number, policy->name);
    assert(msg);

    display_notification(msg);
    puts(msg);
    free(msg);

    return true;
}

static bool do_set_next_policy() {
    const Policy *policy = get_current_policy();

    // Get the next policy in the supported_policies array w/ wraparound
    assert(policy >= &supported_policies[0] && policy <= &supported_policies[ARRAY_SIZE(supported_policies) - 1]);
    if (policy == &supported_policies[ARRAY_SIZE(supported_policies) - 1]) {
        policy = &supported_policies[0];
    } else {
        policy = policy + 1;
    }

    return do_set_policy(policy->name);
}

void display_notification(const char *message) {
    NotifyNotification *notif = notify_notification_new(PROGRAM_NAME, message, NULL);
    if (!notify_notification_show(notif, NULL)) {
        fprintf(stderr, "Warning: Failed to display notification.\n");
        return;
    }

    g_object_unref(G_OBJECT(notif));
}
