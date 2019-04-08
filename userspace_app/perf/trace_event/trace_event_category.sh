perf list | awk -f trace_event_category.awk | sort -t: -k2 -nr | column
