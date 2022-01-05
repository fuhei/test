#!/bin/bash

rpm -qp   --queryformat '%{NAME} %{EPOCH} %{VERSION} %{RELEASE} %{ARCH}\n'
