1. Repo & Reference

https://github.com/istio/tools.git 

https://github.com/istio/tools/blob/master/perf/benchmark/README.md

2. Directory

cd perf/benchmark/

3. Setup Env

Apply yaml under config_yaml according to 0-order.txt.

Then:

$ kubectl -n twopods-istio port-forward svc/fortioclient 9076:9076 &
$ export FORTIO_CLIENT_URL=http://localhost:9076
$ kubectl -n kube-system port-forward svc/prometheus-metrics 9090:9090 &
$ export PROMETHEUS_URL=http://localhost:9090

4. Run test

python3.6 runner/runner.py --config_file latency_yaml/latency-runc.yaml
python3.6 runner/runner.py --config_file latency_yaml/latency.yaml

5. Retrieve Data

python3.6 ./runner/fortio.py $FORTIO_CLIENT_URL --prometheus=$PROMETHEUS_URL \
    --csv StartTime,ActualDuration,Labels,NumThreads,ActualQPS,p50,p90,p99,p999,cpu_mili_avg_istio_proxy_fortioclient,cpu_mili_avg_istio_proxy_fortioserver,cpu_mili_avg_istio_proxy_istio-ingressgateway,mem_Mi_avg_istio_proxy_fortioclient,mem_Mi_avg_istio_proxy_fortioserver,mem_Mi_avg_istio_proxy_istio-ingressgateway

Result is generated into /tmp/xxx.json and /tmp/xxx.csv.
And this step would retrieve all results executed by runner/fortio.py above.

cd graph_plotter/

python3.6 ./graph_plotter.py  --graph_type=latency-p99 --x_axis=conn \
  --telemetry_modes=v2-stats-kata_both,v2-stats-runc_both,v2-stats-brunc_both \
  --query_list=2,4,8,16,32,64 --query_str=ActualQPS==8000 \
  --csv_filepath=<上个命令生成的csv> --graph_title=<存放生成的图.png>

Note:

--telemetry_modes: used to match Labels in csv
--query_str: used to match ActualQPS filed
--graph_type: extract fields of p99 or p50 etc.

6. Clear previous results

The test result is stored in directory "/var/lib/fortio" of the "shell" container.
And ./runner/fortio.py copy test result to /tmp/fortio_json_data/ on host for futher processing.

If you don't remove those files, test result would accumulate.
If you want to do a fresh test, remove those files under /tmp/fortio_json_data.
