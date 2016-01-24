# cliff_rand.awk --- generate Cliff random numbers



BEGIN { _cliff_seed = 0.1 }

function cliff_rand()
{
    _cliff_seed = (100 * log(_cliff_seed)) % 1
    if (_cliff_seed < 0)
        _cliff_seed = - _cliff_seed
    return _cliff_seed
}

#BEGIN {
#    print cliff_rand()
#    print cliff_rand()
#}
