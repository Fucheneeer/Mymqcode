ADMIN_HOME=$(dirname `cd "$(dirname "$0")";pwd`)
if [ -d "${ADMIN_HOME}/log/" ]; then
        find ${ADMIN_HOME}/log/ -mindepth 1 -maxdepth 1 -mtime +10 -name "arbitrator.log*" -exec rm -f {} \;
        find ${ADMIN_HOME}/log/ -mindepth 1 -maxdepth 1 -mtime +10 -name "nohup_Arbitrator.log*" -exec rm -f {} \;
      if [ $? = 0 ];then
        echo ${ADMIN_HOME}/bin/arbitrator_cleanlog.sh : INFO : $(date "+%Y-%m-%d %H:%M:%S") : "${ADMIN_HOME}/log/ 10 days ago successfully removed" >>${ADMIN_HOME}/cleanlog.txt
      else
        echo ${ADMIN_HOME}/bin/arbitrator_cleanlog.sh : ERROR : $(date "+%Y-%m-%d %H:%M:%S") : "${ADMIN_HOME}/log/ 10 days ago fail to be removed" >>${ADMIN_HOME}/cleanlog.txt
      fi
 fi
