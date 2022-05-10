#!/usr/bin/env bash

set -xe
cc -g -std=c11 -Wall -Werror -pedantic -o dvd_logo main.c -lraylib -lm
