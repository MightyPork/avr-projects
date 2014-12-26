#!/bin/bash

dir=/tmp/avrclib_unittest

[ -d "$dir" ] || mkdir "$dir"

gcc test_calc.c -o "$dir/test_calc" &&  "$dir/test_calc"

