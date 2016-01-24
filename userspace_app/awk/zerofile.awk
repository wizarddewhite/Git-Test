# zerofile.awk --- library file to process empty input files



BEGIN { Argind = 0 }

ARGIND > Argind + 1 {
    for (Argind++; Argind < ARGIND; Argind++)
        zerofile(ARGV[Argind], Argind)
}

ARGIND != Argind { Argind = ARGIND }

END {
    if (ARGIND > Argind)
        for (Argind++; Argind <= ARGIND; Argind++)
            zerofile(ARGV[Argind], Argind)
}

# test
#function zerofile(filename, idx)
#{
#	print filename idx
#}
#
#BEGIN {
#	print "test"
#}
