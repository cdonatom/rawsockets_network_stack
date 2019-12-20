#! /bin/bash

rawnetcc /tmp/eth_client eth_client.c eth.c debug.c
rawnetcc /tmp/eth_server eth_server.c eth.c debug.c