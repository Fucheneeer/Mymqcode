#!/bin/bash
ulimit -c unlimited
CFMQ_HOME=$(cd "$(dirname "$0")";pwd -P)/..
export CFMQ_HOME
#echo $CFMQ_HOME
export LD_LIBRARY_PATH=$CFMQ_HOME/lib:$LD_LIBRARY_PATH
mkdir -p $CFMQ_HOME/log $CFMQ_HOME/sessionStore

pid=0
PROCESSOR_NAME=CFMQBroker
broker=$CFMQ_HOME/etc/broker.properties
broker_log=$CFMQ_HOME/log/broker.log

arbitrator=$CFMQ_HOME/etc/arbitrator.properties

check_env()
{
  if [ -n "$LD_LIBRARY_PATH" ]; then
     echo "Using LD_LIBRARY_PATH at " $LD_LIBRARY_PATH
         echo "run as: " ${USER}
  else
     echo "Could not find LD_LIBRARY_PATH, please check LD_LIBRARY_PATH."
     exit -1
  fi
}

refresh_pid()
{
    pid=`ps x|grep $PROCESSOR_NAME|grep -w ${CFMQ_HOME} | grep -v "grep" | awk '{print $1}'`
        if [ ! -n "$pid" ];then
                pid=0
        fi
        echo "get $PROCESSOR_NAME pid:" $pid
}

start_before()
{
    check_env
    refresh_pid
    echo "check process is stop......."
    if [ $pid -ne 0 ]; then
        processnum1=`ps -p $pid|wc -l`
        if [ "$processnum1" = "2" ]; then
            echo "process[pid:$pid] is running,kill -15 process before start......"
            kill -15 $pid
        else
            echo "[process is stop,before start process]"
        fi
    else
        echo "[process is stop,before start process]"
    fi
    echo "check process is stop again......."
    sleep 10
    refresh_pid
    if [ $pid -ne 0 ]; then
        processnum1=`ps -p $pid|wc -l`
        if [ "$processnum1" = "2" ]; then
            echo "process[pid:$pid] is running,kill -9 process before start......"
            kill -9 $pid
        else
            echo "[process is stop,before start process]"
        fi
    else
        echo "[process is stop,before start process]"
    fi
}

check_start()
{
    sleep 5
    refresh_pid
  if [ $pid -ne 0 ]; then
        processnum1=`ps -p $pid|wc -l`
        if [ "$processnum1" = "2" ]; then
            echo "process[pid:$pid] is running......"
        else
            echo "!!!!start is Fail!!!!"
            exit -1
        fi
    else
        echo "!!!app.pid is bad!!!"
        exit -1
    fi
}

stop_pid()
{
    check_env
    refresh_pid
    if [ $pid -ne 0 ]; then
        echo "stop process pid:$pid"
        kill -15 $pid
    else
        echo "process is stop"
    fi
}


check_stop()
{
    echo "check process is stop......"
    refresh_pid
    if [ $pid -ne 0 ]; then
        processnum2=`ps -p $pid|wc -l`
        if [ "$processnum2" = "2" ]; then
            echo "process[pid:$pid] is running on,then kill -9 process......"
            kill -9 $pid
        else
            echo "process is stop"
        fi
    else
        echo "process is stop"
    fi
    rm -rf app.pid
}
start_Broker(){

start_before
echo "start process begin......"
if [ -f "$broker_log" ];then
    nowtime=$(date "+%Y-%m-%d-%H:%M:%S")
    mv $broker_log $broker_log.$nowtime
fi
nohup $CFMQ_HOME/bin/$PROCESSOR_NAME  $broker > $CFMQ_HOME/log/nohup_broker.log &
#echo $!>app.pid
check_start
}

stop_Broker(){
stop_pid
sleep 10
check_stop
}


start_Arbitrator(){
PROCESSOR_NAME=CFMQArbitrator
start_before
echo "start process begin......"
nohup $CFMQ_HOME/bin/$PROCESSOR_NAME  $CFMQ_HOME/etc/arbitrator.properties > $CFMQ_HOME/log/nohup_Arbitrator.log &
#echo $!>app.pid
check_start

}

stop_Arbitrator(){
PROCESSOR_NAME=CFMQArbitrator
stop_pid
sleep 10
check_stop
}

Broker_app(){

    if [ $1 == "start" ]
    then
        start_Broker
    elif [ $1 == "stop" ]
    then
       stop_Broker
    else
     echo $1  
    fi
}
Arbitrator_app(){
    if [ $1 == "start" ]
    then
        start_Arbitrator
    elif [ $1 == "stop" ]
     then
        stop_Arbitrator
    else
     echo $1  
    fi


}
broker_status(){
    broker_pid=`ps x | grep -w "$CFMQ_HOME" | grep -v grep | awk '{print$1}'`
    if [ -n $broker_pid ]
    then
        echo "Broker [pid: $broker_pid] Is Running...."
    else
        echo "Broker is not running !!!"
    fi
    broker_log=`grep "dep.framework.log.sink.s_file.path" $broker | awk -F"=" '{print $2}'`
    if [ -e $broker_log ]
    then
        broker_status=`grep "role" ${broker_log}* | grep Application | tail -n 1| grep -E -o "ONLINE|HOT_STANDBY"`
        echo "Broker  IS : $broker_status"
    else
        echo "Broker  log file does not exis, Unable to get broker status."
    fi
}
Usage(){
echo "
Usage app.sh start/stop/restart      default Broker application
    
    -a,start        start Arbitrator  
       stop         stop Arbitrator
    
    -b,start        start Broker
       stop         stop Broker

"
}
while getopts "a:b:hs" opt;do
    case ${opt} in

  b)
    Broker_app $OPTARG
  ;;

  a)
    Arbitrator_app $OPTARG
  ;;

  h)
    Usage
  ;;
  *)
   Usage
  exit 1;;
esac
done

case "$1" in
  start)
    Broker_app start
 ;;
  stop)
    Broker_app stop
 ;;
  restart)
    Broker_app stop
    Broker_app start
 ;;
  status)
    broker_status
 ;;
esac

