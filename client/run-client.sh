#!/bin/sh

SERVER=192.168.101.10
PORT=5003
SEED=1234
TOTAL=20
START=1
DIFFICULTY=10
REP_PROB_PERCENT=0
DELAY_US=100000
PRIO_LAMBDA=0

/home/vagrant/os-challenge-common/client/client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA
