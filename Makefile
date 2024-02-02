# SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later

VERSION := $(shell git describe --tags | sed -E -e 's/-[0-9]+-/-/' -e 's/^v//')

DEVKIT := docker run -v $$PWD:$$PWD -w $$PWD -it -e LOCAL_IDF_PATH=$$IDF_PATH \
	  -e LOCAL_IDF_TOOLS_PATH=$${IDF_TOOLS_PATH:-~/.espressif} -e SDKCONFIG_DEFAULTS \
	  kywy/devkit:latest

.PHONY: license
license:
	$(DEVKIT) reuse lint

.PHONY: flash-server
flash-server:
	@if [ -z $$IDF_PORT ]; then \
		echo "IDF_PORT is required but not set"; exit 1; \
	fi
	esp_rfc2217_server.py -v -p 4000 $$IDF_PORT

.PHONY: fonts
fonts:
	$(DEVKIT) sh -c 'make --file Makefile.fonts generate'

# component management
.PHONY: publish_component
publish_component:
	compote component upload --namespace kywyerik --name display --version $(VERSION)

# combine targets and forward into devkit container to avoid duplicate container startup costs
.PHONY: build lint test clean compile_commands.json
$(eval $(firstword $(filter-out flash-server license fonts publish_component,$(MAKECMDGOALS))):; $$(DEVKIT) sh -c 'make --file /devkit/Makefile $(MAKECMDGOALS)')
