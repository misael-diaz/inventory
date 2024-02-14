#!/usr/bin/make
#
# Inventory					February 10, 2024
#
# source: src/inventory/Makefile
# author: @misael-diaz
#
# Synopsis:
# Defines the Makefile for building the program with GNU make.
#
# Copyright (c) 2024 Misael Díaz-Maldonado
# This file is released under the GNU General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#

include make-inc

all: $(INVENTORY_BIN)

$(INVENTORY_BIN): $(INVENTORY_OBJ)
	$(CXX) $(CXXOPT) $(INVENTORY_OBJ) -o $(INVENTORY_BIN)

$(INVENTORY_OBJ): $(HEADERS) $(INVENTORY_CXX)
	$(CXX) $(INC) $(CXXOPT) -c $(INVENTORY_CXX) -o $(INVENTORY_OBJ)

clean:
	/bin/rm -f *.obj *.bin
