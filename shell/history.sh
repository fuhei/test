#history
# 1、修改了非root用户不能创建目录和其它permission deny问题。
# 2、修改了日志时间，便于日志排序寻找某一时刻操作记录。
USER=$( whoami )
USER_IP=$( who -u am i 2>/dev/null|egrep -o "([0-9]{1,3}\.){3}[0-9]{1,3}" )
if [ "$USER_IP" = "" ]; then
        USER_IP=$( hostname )
fi
if [ ! -d /var/log/history ]; then
  sudo mkdir /var/log/history/
  sudo chmod 777 /var/log/history
fi
if [ ! -d /var/log/history/$USER ]; then
  sudo mkdir /var/log/history/$USER
  sudo chmod 300 /var/log/history/$USER
  sudo chown $USER:$USER /var/log/history/$USER
fi

export HISTSIZE=10240

DT=`date +"%Y%m%d%H%M%S"`
export HISTFILE="/var/log/history/$USER/${USER}@${USER_IP}_$DT"
if [ ! -e $HISTFILE ]; then
  sudo touch $HISTFILE
  sudo chown $USER:$USER $HISTFILE
fi
export PROMPT_COMMAND='{ date "+%Y%m%d%H%M%S ##### $(who am i |awk "{print \$1\" \"\$2\" \"\$5}")  #### $(history 1 | { read x cmd; echo "$cmd"; })"; } >> $HISTFILE'
sudo chmod 600 /var/log/history/$USER/*history* 2>/dev/null

