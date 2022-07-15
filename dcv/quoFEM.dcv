#!/bin/bash
#
#-----------------------------------------------------------------------------
# This Frontera job script is designed to create a DCV session on 
# Frontera nodes through the SLURM batch system. Once the job
# is scheduled, check the output of your job (which by default is
# stored in your working directory in a file named dcvserver.out)
# and it will tell you the port number that has been setup for you so
# that you can attach via a modern web browser to any Frontera login 
# node (e.g., frontera.tacc.utexas.edu). 
#
# WARNING: DO NOT RENAME THE JOB. SLURM DEPENDS ON THE JOB NAME 'dcvserver' 
# TO CONFIGURE THE SYSTEM PROPERLY. IF YOU RENAME THE JOB, DCV WILL NOT WORK.
#
# Note that for security, DCV traffic is encoded by TLS via secure web
# connections. Once you connect, you should see a single xterm running 
# which you can use to launch any X application (e.g. Paraview or VisIt) 
#
# Note: you can fine tune the SLURM submission variables below as
# needed.  Typical items to change are the runtime limit, location of
# the job output, and the allocation project to submit against (it is
# commented out for now, but is required if you have multiple
# allocations).  
#
# DCV dynamically adjusts the underlying desktop to match the window resolution
# on your client. You can specify a static resolution through the DCV client
# interface once your connection is established.
#
# To submit the job, issue: "sbatch /share/doc/slurm/job.dcv" 
#
# For more information, please consult the User Guide at: 
#
# https://portal.tacc.utexas.edu/user-guides/frontera
#-----------------------------------------------------------------------------
#
#SBATCH -J dcvserver                  # Job name
#SBATCH -o dcvserver.out              # Name of stdout output file (%j expands to jobId)
#SBATCH -p development                    # Queue name
#SBATCH -N 1                          # Total number of nodes requested (68 cores/node)
#SBATCH -n 1                          # Total number of mpi tasks requested
#SBATCH -t 02:00:00                   # Run time (hh:mm:ss) - 4 hours

#--------------------------------------------------------------------------
# ---- You normally should not need to edit anything below this point -----
#--------------------------------------------------------------------------
#
# last update: pnav 20180703

echo "TACC: job $SLURM_JOB_ID execution at: `date`"

# our node name
NODE_HOSTNAME=`hostname -s`
echo "TACC: running on node $NODE_HOSTNAME"

# confirm DCV server is alive
DCV_SERVER_UP=`systemctl is-active dcvserver`
if [ $DCV_SERVER_UP != "active" ]; then
  echo "TACC:"
  echo "TACC: ERROR - could not confirm dcvserver active, systemctl returned '$DCV_SERVER_UP'"
  echo "TACC: ERROR - Please submit a consulting ticket at the TACC user portal"
  echo "TACC: ERROR - https://portal.tacc.utexas.edu/tacc-consulting/-/consult/tickets/create"
  echo "TACC:"
  echo "TACC: job $SLURM_JOB_ID execution finished at: `date`"
  exit 1
fi 


# create an X startup file in /tmp
# source xinitrc-common to ensure xterms can be made
# then source the user's xstartup if it exists
XSTARTUP="/tmp/dcv-startup-$USER"
cat <<- EOF > $XSTARTUP
#!/bin/sh

unset SESSION_MANAGER
unset DBUS_SESSION_BUS_ADDRESS
. /etc/X11/xinit/xinitrc-common 
EOF
#if [ -x $HOME/.vnc/xstartup ]; then
#  cat $HOME/.vnc/xstartup >> $XSTARTUP
#else 
  echo "exec startxfce4" >> $XSTARTUP 
#fi
chmod a+rx $XSTARTUP

# if X0 socket exists, then DCV will use a higher X display number and ruin our day
# therefore, cowardly bail out and appeal to an admin to fix the problem
if [ -f /tmp/.X11-unix/X0 ]; then
  echo "TACC:"
  echo "TACC: ERROR - X0 socket already exists. DCV script will fail."
  echo "TACC: ERROR - Please submit a consulting ticket at the TACC user portal"
  echo "TACC: ERROR - https://portal.tacc.utexas.edu/tacc-consulting/-/consult/tickets/create"
  echo "TACC:"
  echo "TACC: job $SLURM_JOB_ID execution finished at: `date`"
  exit 1
fi

# create DCV session
DCV_HANDLE="$USER-session"
dcv create-session --init=$XSTARTUP $DCV_HANDLE
if ! `dcv list-sessions | grep -q $USER`; then
  echo "TACC:"
  echo "TACC: ERROR - could not find a DCV session for $USER"
  echo "TACC: ERROR - This could be because all DCV licenses are in use."
  echo "TACC: ERROR - Consider using job.dcv2vnc which launches VNC if DCV is not available."
  echo "TACC: ERROR - If you receive this message repeatedly, "
  echo "TACC: ERROR - please submit a consulting ticket at the TACC user portal:"
  echo "TACC: ERROR - https://portal.tacc.utexas.edu/tacc-consulting/-/consult/tickets/create"
  echo "TACC:"
  echo "TACC: job $SLURM_JOB_ID execution finished at: `date`"
  exit 1
fi


LOCAL_VNC_PORT=8443  # default DCV port
echo "TACC: local (compute node) DCV port is $LOCAL_VNC_PORT"

LOGIN_VNC_PORT=`echo $NODE_HOSTNAME | perl -ne 'print (($2+1).$3.$1) if /c\d(\d\d)-(\d)(\d\d)/;'`
if `echo ${NODE_HOSTNAME} | grep -q c2`; then
    # on a c200 node, bump the login port 
    LOGIN_VNC_PORT=$(($LOGIN_VNC_PORT + 36000))
fi
echo "TACC: got login node DCV port $LOGIN_VNC_PORT"

# create reverse tunnel port to login nodes.  Make one tunnel for each login so the user can just
# connect to frontera.tacc
for i in `seq 4`; do
    ssh -q -f -g -N -R $LOGIN_VNC_PORT:$NODE_HOSTNAME:$LOCAL_VNC_PORT login$i
done
echo "TACC: Created reverse ports on Frontera logins"

echo "TACC: Your DCV session is now running!"
echo "TACC: To connect to your DCV session, please point a modern web browser to:"
echo "TACC:          https://frontera.tacc.utexas.edu:$LOGIN_VNC_PORT"


# Warn the user when their session is about to close
# see if the user set their own runtime
#TACC_RUNTIME=`qstat -j $JOB_ID | grep h_rt | perl -ne 'print $1 if /h_rt=(\d+)/'`  # qstat returns seconds
#TACC_RUNTIME=`squeue -l -j $SLURM_JOB_ID | grep $SLURM_QUEUE | awk '{print $7}'` # squeue returns HH:MM:SS
TACC_RUNTIME=`squeue -j $SLURM_JOB_ID -h --format '%l'`
if [ x"$TACC_RUNTIME" == "x" ]; then
	TACC_Q_RUNTIME=`sinfo -p $SLURM_QUEUE | grep -m 1 $SLURM_QUEUE | awk '{print $3}'`
	if [ x"$TACC_Q_RUNTIME" != "x" ]; then
		# pnav: this assumes format hh:dd:ss, will convert to seconds below
		#       if days are specified, this won't work
		TACC_RUNTIME=$TACC_Q_RUNTIME
	fi
fi

# run an xterm for the user; execution will hold here
mkdir -p $HOME/.tap
TAP_LOCKFILE=${HOME}/.tap/${SLURM_JOB_ID}.lock
DISPLAY=:0 xterm -fg white -bg red3 +sb -geometry 55x2+0+0 -T 'END SESSION HERE' -e "echo 'TACC: Press <enter> in this window to end your session' && read && rm ${TAP_LOCKFILE}" &
sleep 1
#DISPLAY=:0 xterm -ls -geometry 80x24+100+50 &
DISPLAY=:0 source /work2/00477/tg457427/frontera/quoFEM/build/run_quoFEM.sh & 

echo $(date) > ${TAP_LOCKFILE}
while [ -f ${TAP_LOCKFILE} ]; do
    sleep 1
done

# job is done!

echo "TACC: closing DCV session" 
dcv close-session $DCV_HANDLE

# wait a brief moment so vncserver can clean up after itself
sleep 1

# remove X11 sockets so DCV will find :0 next time
find /tmp/.X11-unix -user $USER -exec rm -f '{}' \;

echo "TACC: job $SLURM_JOB_ID execution finished at: `date`"
