# ~/.bashrc: executed by bash(1) for non-login shells.
# see /usr/share/doc/bash/examples/startup-files (in the package bash-doc)
# for examples

# If not running interactively, don't do anything
[ -z "$PS1" ] && return

# don't put duplicate lines in the history. See bash(1) for more options
#export HISTCONTROL=ignoredups

# check the window size after each command and, if necessary,
# update the values of LINES and COLUMNS.
shopt -s checkwinsize

# make less more friendly for non-text input files, see lesspipe(1)
[ -x /usr/bin/lesspipe ] && eval "$(lesspipe)"

# set variable identifying the chroot you work in (used in the prompt below)
if [ -z "$debian_chroot" -a -r /etc/debian_chroot ]; then
    debian_chroot=$(cat /etc/debian_chroot)
fi

# set a fancy prompt (non-color, unless we know we "want" color)
case "$TERM" in
xterm-color)
    PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
    ;;
*)
    PS1='${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '
    ;;
esac

# Comment in the above and uncomment this below for a color prompt
#PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '

# If this is an xterm set the title to user@host:dir
case "$TERM" in
xterm*|rxvt*)
    PROMPT_COMMAND='echo -ne "\033]0;${USER}@${HOSTNAME}: ${PWD/$HOME/~}\007"'
    ;;
*)
    ;;
esac

# Alias definitions.
# You may want to put all your additions into a separate file like
# ~/.bash_aliases, instead of adding them here directly.
# See /usr/share/doc/bash-doc/examples in the bash-doc package.

#if [ -f ~/.bash_aliases ]; then
#    . ~/.bash_aliases
#fi

# enable color support of ls and also add handy aliases
if [ "$TERM" != "dumb" ]; then
    eval "`dircolors -b`"
    alias ls='ls --color=auto'
    #alias dir='ls --color=auto --format=vertical'
    #alias vdir='ls --color=auto --format=long'
fi

# some more ls aliases
alias ll='ls -l'
alias la='ls -A'
#alias l='ls -CF'

alias bb="make -j8 bzImage"
alias bm="make -j8 modules"
alias ba="bb && bm"
alias ib="sudo make install"
alias im="sudo make modules_install"
alias ia="im && ib"
alias rb="sudo reboot"
alias stn="sudo shutdown now"

alias x11="x11vnc -display :0 -shared -forever"
alias myguest="expect -f ~/guest/qemu_guest ~/guest/qemu_command root 144025shyw"
alias automig="expect -f ~/guest/auto_migrate ~/guest/qemu_command root 144025shyw"
alias autocpu="expect -f ~/guest/auto_cpu ~/guest/qemu_rhel root 123456"
alias get_maintainer="~/git/Git-Test/userspace_app/bash/extract_maintainer_email.sh"
send_patch() {
	git send-email `get_maintainer $*` $*
}

# enable programmable completion features (you don't need to enable
# this, if it's already enabled in /etc/bash.bashrc and /etc/profile
# sources /etc/bash.bashrc).
#if [ -f /etc/bash_completion ]; then
#    . /etc/bash_completion
#fi

alias vi='gvim &> /dev/null'
alias cpro='cd /media/Study/Project'
alias ckernel='cd /media/Study/Project/kernel_driver/linux-2.6/'
alias clab='cd /media/Study/Project/kernel_driver/linux-0.12-lab/'
alias candroid='cd /media/Study/Project/Android/android-sdk-linux_86'
alias grep='grep --color=auto'
PS1="[\u@\w]\n$"
export PATH=${PATH}:/media/Study/Project/Android/android-sdk-linux_86/tools:/usr/local/arm/4.4.1/bin
#export LC_ALL="zh.CN_UTF-8"

#super proxy
#http_proxy=http://autoproxy.au.ibm.com/hk1.pac
#go agent
#crossgfwwei1 wizarddewhite

