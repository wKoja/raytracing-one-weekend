#!/bin/sh
g++ -o output main.cpp && ./output > imgoutput && sxiv imgoutput
