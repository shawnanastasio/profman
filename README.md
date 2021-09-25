profman
=======

Profman is a small **Prof**ile **Man**ager for certain ASUS laptops running Linux. It allows configuration
of the kernel thermal throttle policy exposed in `/sys/class/platform/asus-nb-wmi/throttle_thermal_policy`.

Currently only the GA401QM (Zephyrus G14 2021 / RTX 3060 Mobile) has been tested, but other laptops
in the range should work too.

Installation
------------
profman requires a C++ compiler, the `libnotify-devel` library, and CMake to be installed.

On Fedora, this is just:
```
$ sudo dnf install gcc-c++ cmake libnotify-devel
```

Afterwards, clone the repository, build, and install:
```
$ git clone https://github.com/shawnanastasio/profman
$ mkdir profman/build && cd profman/build
$ cmake ..
$ make
$ sudo make install
```

By default, profman will install to /usr/local/bin/profman.

Usage
-----

Profman accepts one of three mode arguments:

* `-g/--get`: Get the current throttle policy and print it to stdout
* `-s/--set <policy>`: Set the current throttle policy and display a notification.
  Accepts policy numbers (0, 1, 2) or names (default, turbo, silent).
* `-n`: Increment the set policy number. This is useful for binding to the profile key on some laptops (Fn+F5 on mine).

Ex.
```
$ profman -g
Current thermal_throttle_policy: 1 (turbo)
$ profman -n
thermal_throttle_policy updated to: 2 (silent)
$ profman -s default
thermal_throttle_policy updated to: 0 (default)
```

License
-------
profman is licensed under the terms of the GNU General Public License v3 or later.
See LICENSE.md for full license text.
