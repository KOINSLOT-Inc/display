# SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later

idf_build_get_property(target IDF_TARGET)

set(requirements "")
if(NOT ${target} STREQUAL "linux")
        list(APPEND requirements driver)
endif()

idf_component_register(
        SRC_DIRS "src" "src/drivers" "src/generated"
        INCLUDE_DIRS "include" "include/generated"
        REQUIRES ${requirements}
)
