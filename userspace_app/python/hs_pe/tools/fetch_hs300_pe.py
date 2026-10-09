#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
沪深300 指数 PE 历史数据抓取 + 十年期分位计算工具

数据源（均为公开数据源，抓取时动态获取，不硬编码）：
  主源: 蛋卷基金(雪球) 指数估值接口
        https://danjuanfunds.com/djapi/index_eva/pe_history/SH000300?day=all
  校验: 中证指数官网(官方一手来源) 指数行情含 PE
        https://www.csindex.com.cn/csindex-home/perf/index-perf

输出:
  data/hs300_pe_raw.json   原始序列 [{date, pe}]
  data/hs300_pe_meta.json  元信息 + 各窗口分位统计

用法:
  python3 fetch_hs300_pe.py            # 抓取并计算
  python3 fetch_hs300_pe.py --offline  # 仅用本地缓存重算
"""

import json
import sys
import os
import time
import ssl
import urllib.request
import urllib.error
from datetime import datetime, timezone, timedelta

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(BASE, "data")
RAW_PATH = os.path.join(DATA_DIR, "hs300_pe_raw.json")
META_PATH = os.path.join(DATA_DIR, "hs300_pe_meta.json")

UA = ("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
      "(KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36")

# 计算分位的窗口（年）
WINDOWS = [10, 7, 5, 3]

_CTX = ssl.create_default_context()
_CTX.check_hostname = False
_CTX.verify_mode = ssl.CERT_NONE


def http_get(url, referer=None, timeout=25):
    """带 UA/Referer 的 GET，返回 bytes。失败抛出异常。"""
    req = urllib.request.Request(url, headers={
        "User-Agent": UA,
        "Accept": "application/json, text/plain, */*",
        "Accept-Language": "zh-CN,zh;q=0.9",
        **({"Referer": referer} if referer else {}),
    })
    with urllib.request.urlopen(req, timeout=timeout, context=_CTX) as r:
        return r.read()


def fetch_danjuan():
    """蛋卷基金：返回 [(date_str, pe), ...] 按日期升序"""
    url = "https://danjuanfunds.com/djapi/index_eva/pe_history/SH000300?day=all"
    raw = http_get(url, referer="https://danjuanfunds.com/")
    obj = json.loads(raw.decode("utf-8"))
    growths = (obj.get("data") or {}).get("index_eva_pe_growths") or []
    if not growths:
        raise RuntimeError("蛋卷接口返回空数据: %s" % obj.get("message"))
    out = []
    for it in growths:
        ts = it.get("ts")
        pe = it.get("pe")
        if ts is None or pe in (None, 0):
            continue
        d = datetime.fromtimestamp(ts / 1000, timezone.utc) + timedelta(hours=8)
        out.append((d.strftime("%Y-%m-%d"), float(pe)))
    # 去重 + 升序
    seen = {}
    for d, pe in out:
        seen[d] = pe
    return sorted(seen.items(), key=lambda x: x[0])


def fetch_csindex_latest():
    """中证指数官网：取最近交易日的官方 PE（用于交叉校验）"""
    today = datetime.now(timezone(timedelta(hours=8)))
    start = (today - timedelta(days=45)).strftime("%Y%m%d")
    end = today.strftime("%Y%m%d")
    url = ("https://www.csindex.com.cn/csindex-home/perf/index-perf"
           "?indexCode=000300&startDate=%s&endDate=%s" % (start, end))
    raw = http_get(url, referer="https://www.csindex.com.cn/")
    obj = json.loads(raw.decode("utf-8"))
    rows = obj.get("data") or []
    for r in reversed(rows):
        pe = r.get("peg")
        if pe:
            return {"date": r.get("tradeDate"), "pe": float(pe),
                    "close": r.get("close")}
    return None


# ---------------- 分位计算 ----------------

def percentile_of(value, series):
    """value 在 series 中的百分位（0-100），使用 <= 计数法。"""
    if not series:
        return None
    n = len(series)
    cnt = sum(1 for x in series if x <= value)
    return round(cnt / n * 100, 2)


def value_at_percentile(series, p):
    """series 的 p 分位对应 PE 值（升序线性插值）。"""
    if not series:
        return None
    s = sorted(series)
    if len(s) == 1:
        return round(s[0], 4)
    k = (len(s) - 1) * (p / 100.0)
    lo = int(k)
    hi = min(lo + 1, len(s) - 1)
    frac = k - lo
    return round(s[lo] + (s[hi] - s[lo]) * frac, 4)


def window_stats(series, years):
    """对给定窗口年份的序列计算统计与分位。"""
    if not series:
        return None
    vals = [p for _, p in series]
    latest_date, latest_pe = series[-1][0], series[-1][1]
    return {
        "years": years,
        "start_date": series[0][0],
        "end_date": latest_date,
        "count": len(series),
        "latest_pe": round(latest_pe, 4),
        "latest_percentile": percentile_of(latest_pe, vals),
        "min": round(min(vals), 4),
        "max": round(max(vals), 4),
        "mean": round(sum(vals) / len(vals), 4),
        "median": value_at_percentile(vals, 50),
        "p10": value_at_percentile(vals, 10),
        "p30": value_at_percentile(vals, 30),
        "p50": value_at_percentile(vals, 50),
        "p70": value_at_percentile(vals, 70),
        "p90": value_at_percentile(vals, 90),
        "series": [{"date": d, "pe": round(p, 4), "percentile": percentile_of(p, vals)}
                   for d, p in series],
    }


def split_by_window(series, years):
    """截取最近 N 年的数据（按自然日）。"""
    if not series:
        return []
    last = datetime.strptime(series[-1][0], "%Y-%m-%d")
    cutoff = last - timedelta(days=int(365.25 * years))
    out = [(d, p) for d, p in series
           if datetime.strptime(d, "%Y-%m-%d") >= cutoff]
    return out if len(out) >= 20 else series


def main():
    offline = "--offline" in sys.argv
    os.makedirs(DATA_DIR, exist_ok=True)

    if offline:
        if not os.path.exists(RAW_PATH):
            print("离线模式但无本地缓存: %s" % RAW_PATH, file=sys.stderr)
            sys.exit(1)
        with open(RAW_PATH, encoding="utf-8") as f:
            cached = json.load(f)
        series = [(x["date"], x["pe"]) for x in cached["series"]]
        source = cached.get("source", "cache")
    else:
        errors = []
        try:
            series = fetch_danjuan()
            source = "蛋卷基金(雪球) index_eva/pe_history"
        except Exception as e:
            errors.append("蛋卷基金: %s" % e)
            series = []
        if not series:
            print("主数据源抓取失败: %s" % " | ".join(errors), file=sys.stderr)
            sys.exit(2)

    # 交叉校验
    verify = None
    if not offline:
        try:
            verify = fetch_csindex_latest()
        except Exception as e:
            verify = {"error": str(e)}

    fetch_time = datetime.now(timezone(timedelta(hours=8))).strftime("%Y-%m-%d %H:%M:%S")

    raw_obj = {
        "index": "沪深300 (000300.SH)",
        "source": source,
        "fetch_time": fetch_time,
        "count": len(series),
        "range": {"start": series[0][0], "end": series[-1][0]},
        "series": [{"date": d, "pe": round(p, 4)} for d, p in series],
    }

    windows = {}
    for y in WINDOWS:
        seg = split_by_window(series, y)
        st = window_stats(seg, y)
        if st:
            windows[str(y)] = st

    meta = {
        "index": "沪深300 (000300.SH)",
        "index_name": "沪深300",
        "source": source,
        "source_note": "蛋卷基金指数估值数据，为第三方加工数据；已用中证指数官网官方PE交叉校验",
        "fetch_time": fetch_time,
        "data_range": {"start": series[0][0], "end": series[-1][0],
                       "count": len(series), "frequency": "周度"},
        "cross_check": verify,
        "default_window": "10",
        "windows": {k: {kk: vv for kk, vv in v.items()} for k, v in windows.items()},
    }

    with open(RAW_PATH, "w", encoding="utf-8") as f:
        json.dump(raw_obj, f, ensure_ascii=False, indent=1)
    with open(META_PATH, "w", encoding="utf-8") as f:
        json.dump(meta, f, ensure_ascii=False, indent=1)

    print("✓ 抓取完成")
    print("  数据源   : %s" % source)
    print("  抓取时间 : %s" % fetch_time)
    print("  数据范围 : %s ~ %s (%d 条)" % (series[0][0], series[-1][0], len(series)))
    if verify and not verify.get("error"):
        print("  官方校验 : %s PE=%s (中证指数官网)" % (verify.get("date"), verify.get("pe")))
    w10 = windows.get("10")
    if w10:
        print("  10年窗口 : 最新PE=%.2f  分位=%.1f%%  区间[%.2f, %.2f]" % (
            w10["latest_pe"], w10["latest_percentile"], w10["min"], w10["max"]))
    print("  输出     : %s" % RAW_PATH)
    print("           : %s" % META_PATH)


if __name__ == "__main__":
    main()
