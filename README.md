OS COURSE Huji-2021.
=========

Connecting to the Aquarium ( :tropical_fish: )
---

first go to ```https://registrar.cs.huji.ac.il/hotp```, login with your CSE account, (Moodle Computer Science Faculty user). Then a First OTP password will be send to your mobile. Enter it and then you will receive a bunch of ~5 OTP's.
Create a tunnel via (or ```./tunnel.sh user```), And enter one of the OTP and (again) your CSE password:
```html
ssh -CL 22222:hm-gw:22 <user>%hm-gw@gw.cs.huji.ac.il
```
Define your VsCode remote extension to point on ```ssh://user@localhost:22222```. example of config file, found in ./config.txt. The host will request again for CSE password. And it successfully setup should plot the follow: 
```html
The authenticity of host '[localhost]:22222 ([::1]:22222)' can't be established.
ED25519 key fingerprint is SHA256:<fingerprint>.
Are you sure you want to continue connecting (yes/no)? y
Please type 'yes' or 'no': yes
Warning: Permanently added '[localhost]:22222' (ED25519) to the list of known hosts.
(IDng) Password: 
98876a1478c8: running
Acquiring lock on /cs/usr/<user>/.vscode-server/bin/<commit-number>/vscode-remote-lock.<user>.<commit-number>
\ln /cs/usr/<user>/.vscode-server/bin/<commit-number>/vscode-remote-lock.<user>.<commit-number>.target /cs/usr/<user>/.vscode-server/bin/<commit-number>/vscode-remote-lock.<user>.<commit-number>
Installing to /cs/usr/<user>/.vscode-server/bin/<commit-number>...
98876a1478c8%%1%%
Downloading with wget
Download complete
98876a1478c8%%2%%
SHELL=/bin/csh
LMOD_arch=x86_64
GROUP=stud
MACHTYPE=x86_64
LANGUAGE=en_US:en_GB:en
_ModuleTable002_=<key>
NO_AT_BRIDGE=1
LMOD_DIR=/usr/share/lmod/lmod/libexec
EDITOR=emacs
PWD=/cs/usr/<user>
LOGNAME=<user>
XDG_SESSION_TYPE=tty
MODULESHOME=/usr/share/lmod/lmod
MANPATH=/usr/share/lmod/lmod/share/man:
LMOD_PREPEND_BLOCK=normal
LMOD_SHORT_TIME=86400
HOME=/cs/usr/<user>
_ModuleTable_Sz_=3
LANG=en_IL.UTF-8
OSTYPE=linux
LOCATE_PATH=/var/cache/locate/localdb:/var/cache/locate/locatedb
VSCODE_AGENT_FOLDER=/cs/usr/<user>/.vscode-server
TARG_TITLE_BAR_PAREN=
LMOD_VERSION=6.6
SSH_CONNECTION=<ip> 58721 <ip> 22
LMOD_DEFAULT_MODULEPATH=/etc/lmod/modulefiles:/usr/modulefiles/Linux:/usr/modulefiles/Core:/usr/share/lmod/lmod/modulefiles/Core
_ModuleTable003_=<key>
iJdPTIsfQ==
MODULEPATH_ROOT=/usr/modulefiles
XDG_SESSION_CLASS=user
LMOD_PKG=/usr/share/lmod/lmod
PYTHONPATH=/usr/local/matlab/2018b/lib/pythonX.Y/site-packages
TERM=vt100
HOST=rory
USER=<user>
LOADEDMODULES=mathematica/10.0:matlab/2018b
LMOD_SETTARG_CMD=:
SHLVL=2
BASH_ENV=/usr/share/lmod/lmod/init/bash
PAGER=less
LMOD_FULL_SETTARG_SUPPORT=no
LMOD_sys=Linux
LMOD_PAGER=cat
XDG_SESSION_ID=53474
_ModuleTable001_=<key>
CLASSPATH=
LMOD_COLORIZE=yes
XDG_RUNTIME_DIR=/run/user/203965
SSH_CLIENT=<ip> 58721 22
VENDOR=unknown
PATH=/usr/local/matlab/2018b/bin:/usr/local/mathematica/10.0/Executables:.:/usr/local/bin:/usr/bin:/bin:/usr/games:/usr/X11R6/bin:/usr/bin/mh:/usr/lib/mh
MODULEPATH=/etc/lmod/modulefiles:/usr/modulefiles/Linux:/usr/modulefiles/Core:/usr/share/lmod/lmod/modulefiles/Core
BLOCKSIZE=1k
_LMFILES_=/etc/lmod/modulefiles/mathematica/10.0.lua:/etc/lmod/modulefiles/matlab/2018b.lua
DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/203965/bus
LMOD_CMD=/usr/share/lmod/lmod/libexec/lmod
MAIL=/var/mail/<user>
HOSTTYPE=x86_64-linux
LMOD_SYSTEM_DEFAULT_MODULES=mathematica,matlab
BASH_FUNC_ml%%=() {  eval $($LMOD_DIR/ml_cmd "$@")
}
BASH_FUNC_module%%=() {  eval $($LMOD_CMD bash "$@");
 [ $? = 0 ] && eval $(${LMOD_SETTARG_CMD:-:} -s sh)
}
_=/usr/bin/printenv
OLDPWD=/cs/usr/<user>/.vscode-server/bin/<commit-number>
Starting server with command... /cs/usr/<user>/.vscode-server/bin/<commit-number>/server.sh --host=127.0.0.1 --enable-remote-auto-shutdown  --port=0 &> "/cs/usr/<user>/.vscode-server/.<commit-number>.log" < /dev/null
Waiting for server log...
Waiting for server log...

*
* Reminder: You may only use this software with Visual Studio family products,
* as described in the license (https://go.microsoft.com/fwlink/?linkid=2077057)
*

98876a1478c8: start
agentPort==43931==
osReleaseId==debian==
arch==x86_64==
webUiAccessToken====
tmpDir==/run/user/203965==
platform==linux==
98876a1478c8: end
```
I am not sure, if the process requires an installed version of VsCode on the Aquarium station. ( In that case you should download via ```wget ...``` ).
