#!/bin/bash
objdump -b binary -mi8086 -Mintel -D $1
