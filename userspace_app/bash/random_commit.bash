#!/bin/bash

if [ $# == 0 ]; then
    echo "Usage: $0 auth_patten [auth_patten]"
    exit 0
fi


#ls -l | grep ^d | awk '{print $9}' > .current_dirs

rm -f project_commit_summary.txt
touch project_commit_summary.txt

for project in `cat .current_dirs`
do
    # change pwd to project
    cd $project

    # generate commit stats
    rm commit.log

    echo "Total commit: " `git log --oneline | wc -l` > commit.log
    
    for auth in "$@"
    do
        git log | grep "^Author" | sort -n | uniq -c | sort -n | grep $auth >> commit.log
    done

    # generate own_commits.log
    rm own_commits.log
    touch own_commits.log
    for auth in "$@"
    do
        git log --author="$auth" | grep ^commit >> own_commits.log
    done

    echo "" >> commit.log

    NUM=`cat own_commits.log | wc -l`
    #echo $NUM
    for ((i=0;i<5;i++))
    do
        LINE=`expr $RANDOM % $NUM`
        # echo $LINE
        awk 'NR==num {print $2}' num=$LINE own_commits.log >> commit.log
    done

    # change pwd back
    cd ../

    cat $project/commit.log >> project_commit_summary.txt
done
