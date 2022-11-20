#!/bin/sh
#
# This is the configuration of the difficulty parameter test run.
#

SERVER=192.168.101.10
PORT=5003
SEED=0002
TOTAL=100
START=0
DIFFICULTY=60000000
REP_PROB_PERCENT=20
DELAY_US=600000
PRIO_LAMBDA=1.5

/home/vagrant/os-challenge-common/client/client $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA