perf record -e 'kvm:kvm_pio' -a sleep 30
perf script > pio_stat

awk -f pio_calculate.awk pio_stat | sort -nk 2 -r
