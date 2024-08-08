#!/bin/bash

echo "该脚本获取core和stack"
nowtime=$(date "+%Y%m%d%H%M%S")

PROCESSOR_NAME=CFMQBroker
CFMQ_HOME=$(cd "$(dirname "$0")";pwd)/..
SYSTEM_Name=`echo "${CFMQ_HOME}"|awk -F_ '{print $2}'`


pid=0
refresh_pid()
{
	pid=`ps x|grep $PROCESSOR_NAME|grep -w ${CFMQ_HOME} | grep -v "grep" | awk '{print $1}'`
	if [ ! -n "$pid" ];then
		pid=0
		exit 1
	fi
	echo "get $PROCESSOR_NAME pid:" $pid
}
work()
{

gcore $pid
pstack $pid > stack_$pid.txt
tar zcvf debug_core_stack.tar.gz core.$pid stack_$pid.txt

mv debug_core_stack.tar.gz ../log/debug_core_stack.${SYSTEM_Name}.${nowtime}.tar.gz
echo "打包完成开始清理"
rm core.$pid 
rm stack_$pid.txt

}
refresh_pid
work

