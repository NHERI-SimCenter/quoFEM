#!/bin/sh
dprepro $1 TrussTemplate.tcl Truss.tcl --output-format='\%10.5f'
OpenSees Truss.tcl
mv results.out $2