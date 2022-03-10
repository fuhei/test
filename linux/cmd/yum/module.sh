#!/bin/bash

dnf module list
dnf module list nginx

dnf module info nginx
dnf module info nginx:1.18

dnf moudle info --profile nginx

