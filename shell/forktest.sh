#!/bin/bash

sub1() {
	echo $$
}

echo $(sub1) &
echo $$
echo "done"
