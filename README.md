<!--
SPDX-FileCopyrightText: 2023 KUNBUS GmbH

SPDX-License-Identifier: MIT
-->

<details>
<summary>We have moved to GitLab! Read this for more information.</summary>

We have recently moved our repositories to GitLab. You can find revpi-pitest
here: https://gitlab.com/revolutionpi/revpi-pitest  
All repositories on GitHub will stay up-to-date by being synchronised from
GitLab.

We still maintain a presence on GitHub but our work happens over at GitLab. If
you want to contribute to any of our projects we would prefer this contribution
to happen on GitLab, but we also still accept contributions on GitHub if you
prefer that.
</details>

# piTest

## Building

### Submodules

This repository uses git submodules. These submodules need to be initialized in
order to build `piTest`.

This can be done by initializing submodules while cloning with

```sh
git clone --recurse-submodules https://gitlab.com/revolutionpi/revpi-pitest.git
```

or afterwards with

```sh
git submodule init
git submodule update
```

### Compiling using CMake

These are the instructions to build `piTest`:

```sh
mkdir build && cd build
cmake ..
cmake --build .
```

## Documentation

Usage of `piTest` is documented in [`piTest(1)`](doc/piTest.1.scd).
