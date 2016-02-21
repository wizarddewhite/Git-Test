# noassign.awk --- library file to avoid the need for a
# special option that disables command-line assignments



function disable_assigns(argc, argv,    i)
{
    for (i = 1; i < argc; i++)
        if (argv[i] ~ /^[[:alpha:]_][[:alnum:]_]*=.*/)
            argv[i] = ("./" argv[i])
}

BEGIN {
    if (No_command_assign)
        disable_assigns(ARGC, ARGV)
}

# usage
# awk -v No_command_assign=1 -f noassign.awk -f yourprog.awk *