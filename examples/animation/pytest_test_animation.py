# SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later

import pytest

frame_k = """
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃       ██████████████████████████████████████████████████       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █         ████████████████████████████████       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █         ████                 █       █       ┃
┃       █         █          ███                 █       █       ┃
┃       █         █          ███                 █       █       ┃
┃       █         █          ███                 █       █       ┃
┃       █         █          ███                 █       █       ┃
┃       █         █          ███ █████           █       █       ┃
┃       █         █          ███   █             █       █       ┃
┃       █         █          ███  █              █       █       ┃
┃       █         █          ███  █              █       █       ┃
┃       █         █          ███ █               █       █       ┃
┃       █         █          ██████              █       █       ┃
┃       █         █          ███ ███             █       █       ┃
┃       █         █          ███  ███            █       █       ┃
┃       █         █          ███   ██            █       █       ┃
┃       █         █          ███   ███           █       █       ┃
┃       █         █        ██████ ██████         █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         ████████████████████████████████       █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       ██████████████████████████████████████████████████       ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
""".strip().encode(
    "utf-8"
)

frame_y = """
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃       ██████████████████████████████████████████████████       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █         ████████████████████████████████       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █        ██████ █████          █       █       ┃
┃       █         █          ██     █            █       █       ┃
┃       █         █          ███   █             █       █       ┃
┃       █         █           ██   █             █       █       ┃
┃       █         █           ███  █             █       █       ┃
┃       █         █            ██ █              █       █       ┃
┃       █         █            ██ █              █       █       ┃
┃       █         █            ████              █       █       ┃
┃       █         █             ██               █       █       ┃
┃       █         █             ██               █       █       ┃
┃       █         █              █               █       █       ┃
┃       █         █              █               █       █       ┃
┃       █         █             █                █       █       ┃
┃       █         █         ██  █                █       █       ┃
┃       █         █         ██ █                 █       █       ┃
┃       █         █         ███                  █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         ████████████████████████████████       █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       ██████████████████████████████████████████████████       ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
""".strip().encode(
    "utf-8"
)

frame_w = """
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃       ██████████████████████████████████████████████████       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █         ████████████████████████████████       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █     ██████ ██████ █████      █       █       ┃
┃       █         █       ██     ███    █        █       █       ┃
┃       █         █       ███   █ ██   █         █       █       ┃
┃       █         █        ██   █ ██   █         █       █       ┃
┃       █         █        ██  █  ███  █         █       █       ┃
┃       █         █        ███ █   ██ █          █       █       ┃
┃       █         █         ██ █   ██ █          █       █       ┃
┃       █         █         ███    ███           █       █       ┃
┃       █         █         ███     ██           █       █       ┃
┃       █         █          █      ██           █       █       ┃
┃       █         █          █      █            █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         █                              █       █       ┃
┃       █         ████████████████████████████████       █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       █                                                █       ┃
┃       ██████████████████████████████████████████████████       ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┃                                                                ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
""".strip().encode(
    "utf-8"
)


def test_app(dut):
    dut.expect([frame_k, frame_y, frame_w], expect_all=True, timeout=3)
