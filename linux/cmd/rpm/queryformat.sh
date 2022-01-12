#!/bin/bash

#    BUILDHOST
#    BUILDTIME
#    DESCRIPTION
#    EPOCH
#    INSTALLTIME
#    NAME
#    RELEASE
#    SIZE
#    SUMMARY
#    VERSION

rpm -qa --queryformat "%{NAME} %{SIZE}\n"
rpm -qa --queryformat "%-30{NAME} %10{SIZE}\n"
rpm -q --queryformat "[%-50{FILENAMES} %10{FILESIZES}\n]" slang-devel
