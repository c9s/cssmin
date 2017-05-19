#!/usr/bin/env bash
export REPORT_EXIT_STATUS=1
export TEST_PHP_EXECUTABLE=sapi/cli/php
export NO_INTERACTION=1
make test
