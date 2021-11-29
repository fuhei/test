#!/bin/bash 

jq 'del(.packages[].evra)' test.json
