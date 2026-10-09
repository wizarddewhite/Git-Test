#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
多指数 PE 历史数据抓取 + 分位计算工具

指数(按页面 Tab 展示顺序): 上证50(000016.SH) / 沪深300(000300.SH) / 中证500(000905.SH)

数据源:
  主源 中证指数官网(一手来源, 指数编制机构) 日度 PE
       https://www.csindex.com.cn/csindex-home/perf/index-perf
       按年分块请求(单次跨度过大连接会被重置), 字段 peg = 市盈率
  校验 蛋卷基金(雪球) 指数估值 周度 PE
       https://danjuanfunds.com/djapi/index_eva/pe_history/{code}?day=all

输出:
  data/index_pe_raw.json   各指数原始序列
  data/index_pe_meta.json  元信息 + 各窗口分位统计(内联给页面用)

用法:
  python3 fetch_index_pe.py             # 抓取全部指数
  python3 fetch_index_pe.py --offline   # 用本地缓存重算
"""

import json
import os
import ssl
import sys
import time
import urllib.request
from datetime import datetime, timedelta, timezone

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(BASE, "data")
RAW_PATH = os.path.join(DATA_DIR, "index_pe_raw.json")
META_PATH = os.path.join(DATA_DIR, "index_pe_meta.json")

UA = ("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
      "(KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36")

CST = timezone(timedelta(hours=8))
WINDOWS = [10, 7, 5, 3]

# 展示顺序: (中证代码, 蛋卷代码, 名称, 简称)
INDICES = [
    ("000016", "SH000016", "上证50", "上证50"),
    ("000300", "SH000300", "沪深300", "沪深300"),
    ("000905", "SH000905", "中证500", "中证500"),
]

_CTX = ssl.create_default_context()
_CTX.check_hostname = False
_CTX.verify_mode = ssl.CERT_NONE


def http_get(url, referer, timeout=30, tries=3):
    last = None
    for i in range(tries):
        try:
            req = urllib.request.Request(url, headers={
                "User-Agent": UA,
                "Accept": "application/json, text/plain, */*",
                "Accept-Language": "zh-CN,zh;q=0.9",
                "Referer": referer,
            })
            with urllib.request.urlopen(req, timeout=timeout, context=_CTX) as r:
                return r.read()
        except Exception as e:
            last = e
            time.sleep(1.5 * (i + 1))
    raise RuntimeError("请求失败 %s: %s" % (url, last))


def fetch_csindex_daily(code):
    """中证指数官网日度 PE，按年分块。返回 [(date, pe, close)] 升序。"""
    now = datetime.now(CST)
    rows = []
    for y in range(now.year - 10, now.year + 1):
        url = ("https://www.csindex.com.cn/csindex-home/perf/index-perf"
               "?indexCode=%s&startDate=%d0101&endDate=%d1231" % (code, y, y))
        try:
            obj = json.loads(http_get(url, "https://www.csindex.com.cn/").decode("utf-8"))
        except Exception as e:
            print("    %d 年失败: %s" % (y, e), file=sys.stderr)
            continue
        for x in (obj.get("data") or []):
            pe = x.get("peg")
            d = x.get("tradeDate")
            if pe and d:
                rows.append((d, float(pe), x.get("close")))
        time.sleep(1.1)
    seen = {}
    for d, pe, cl in rows:
        seen[d] = (pe, cl)
    out = []
    for d in sorted(seen):
        pe, cl = seen[d]
        out.append(("%s-%s-%s" % (d[:4], d[4:6], d[6:]), pe, cl))
    return out


def fetch_danjuan_weekly(djcode):
    """蛋卷基金周度 PE。返回 [(date, pe)] 升序。"""
    url = ("https://danjuanfunds.com/djapi/index_eva/pe_history/%s?day=all" % djcode)
    obj = json.loads(http_get(url, "https://danjuanfunds.com/").decode("utf-8"))
    g = (obj.get("data") or {}).get("index_eva_pe_growths") or []
    out = {}
    for it in g:
        ts, pe = it.get("ts"), it.get("pe")
        if ts is None or not pe:
            continue
        d = datetime.fromtimestamp(ts / 1000, timezone.utc) + timedelta(hours=8)
        out[d.strftime("%Y-%m-%d")] = float(pe)
    return sorted(out.items(), key=lambda x: x[0])


def fetch_danjuan_detail(djcode):
    url = "https://danjuanfunds.com/djapi/index_eva/detail/%s" % djcode
    try:
        obj = json.loads(http_get(url, "https://danjuanfunds.com/").decode("utf-8"))
        d = obj.get("data") or {}
        return {"pe": d.get("pe"), "percentile": d.get("pe_percentile"),
                "date": d.get("date"), "pb": d.get("pb")}
    except Exception as e:
        return {"error": str(e)}


# ---------------- 分位计算 ----------------

def percentile_of(value, series):
    if not series:
        return None
    return round(sum(1 for x in series if x <= value) / len(series) * 100, 2)


def value_at_percentile(series, p):
    if not series:
        return None
    s = sorted(series)
    if len(s) == 1:
        return round(s[0], 4)
    k = (len(s) - 1) * (p / 100.0)
    lo = int(k)
    hi = min(lo + 1, len(s) - 1)
    return round(s[lo] + (s[hi] - s[lo]) * (k - lo), 4)


def split_by_window(series, years):
    if not series:
        return []
    last = datetime.strptime(series[-1][0], "%Y-%m-%d")
    cutoff = last - timedelta(days=int(365.25 * years))
    out = [(d, p) for d, p, *_ in series
           if datetime.strptime(d, "%Y-%m-%d") >= cutoff]
    return out if len(out) >= 20 else [(d, p) for d, p, *_ in series]


def window_stats(seg, years):
    if not seg:
        return None
    vals = [p for _, p in seg]
    latest = seg[-1][1]
    return {
        "years": years,
        "start_date": seg[0][0],
        "end_date": seg[-1][0],
        "count": len(seg),
        "latest_pe": round(latest, 4),
        "latest_percentile": percentile_of(latest, vals),
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
                   for d, p in seg],
    }


def build_index(code, djcode, name, short, official, djweekly, djdetail, offline):
    """用官方日度序列构建分位统计，蛋卷作为交叉校验。"""
    if official:
        series = official
        src_name = "中证指数官网（指数编制机构，日度）"
    else:
        series = [(d, p, None) for d, p in djweekly]
        src_name = "蛋卷基金（雪球）周度（降级）"

    if not series:
        return None

    # 主口径 = 中证指数官网日度；蛋卷周度作为同期对照
    windows = {}
    for y in WINDOWS:
        st = window_stats(split_by_window(series, y), y)
        if not st:
            continue
        # 同一时间窗口内，用蛋卷口径重算一次分位，供页面并列展示
        if djweekly:
            dj_seg = [(d, p) for d, p in djweekly
                      if st["start_date"] <= d <= st["end_date"]]
            if len(dj_seg) >= 5:
                dj_vals = [p for _, p in dj_seg]
                st["cross_last_date"] = dj_seg[-1][0]
                st["cross_last_pe"] = round(dj_seg[-1][1], 4)
                st["cross_percentile"] = percentile_of(dj_seg[-1][1], dj_vals)
        windows[str(y)] = st

    # 最新时点两口径对比
    cross = {"danjuan_detail": djdetail}
    if djweekly and official:
        dj_last_date, dj_last_pe = djweekly[-1]
        near = min(official, key=lambda x: abs(
            datetime.strptime(x[0], "%Y-%m-%d") - datetime.strptime(dj_last_date, "%Y-%m-%d")))
        diff = (near[1] - dj_last_pe) / dj_last_pe * 100 if dj_last_pe else None
        cross.update({
            "danjuan_last": {"date": dj_last_date, "pe": round(dj_last_pe, 4)},
            "official_last": {"date": near[0], "pe": round(near[1], 4)},
            "diff_pct": round(diff, 2) if diff is not None else None,
        })

    w10 = windows.get("10") or {}
    return {
        "code": code, "dj_code": djcode, "name": name, "short": short,
        "source": src_name,
        "frequency": "日度" if official else "周度",
        "data_range": {"start": series[0][0], "end": series[-1][0], "count": len(series)},
        "cross_check": cross,
        "default_window": "10",
        "windows": windows,
        "_w10": {k: w10.get(k) for k in
                 ("latest_pe", "latest_percentile", "min", "max", "median", "p30", "p70")},
    }


def main():
    offline = "--offline" in sys.argv
    os.makedirs(DATA_DIR, exist_ok=True)

    cache = {}
    if os.path.exists(RAW_PATH):
        try:
            cache = json.load(open(RAW_PATH, encoding="utf-8"))
        except Exception:
            cache = {}

    fetch_time = datetime.now(CST).strftime("%Y-%m-%d %H:%M:%S")
    raw_out, meta_idx, notes = {}, {}, []

    for code, djcode, name, short in INDICES:
        print("== %s (%s) ==" % (name, code))

        official, djweekly, djdetail = [], [], {}
        if offline:
            c = cache.get(code) or {}
            official = [(x["date"], x["pe"], None) for x in c.get("official_series", [])]
            djweekly = [(x["date"], x["pe"]) for x in c.get("danjuan_series", [])]
            djdetail = c.get("danjuan_detail") or {}
        else:
            try:
                official = fetch_csindex_daily(code)
                print("   官方日度: %d 条" % len(official))
            except Exception as e:
                print("   官方源失败: %s" % e, file=sys.stderr)
            try:
                djweekly = fetch_danjuan_weekly(djcode)
                print("   蛋卷周度: %d 条" % len(djweekly))
            except Exception as e:
                print("   蛋卷源失败: %s" % e, file=sys.stderr)
            djdetail = fetch_danjuan_detail(djcode)

        if not official and not djweekly:
            notes.append("%s: 两个数据源均未取到" % name)
            continue

        idx = build_index(code, djcode, name, short, official, djweekly, djdetail, offline)
        if not idx:
            continue

        raw_out[code] = {
            "name": name, "code": code,
            "official_series": [{"date": d, "pe": round(p, 4), "close": c}
                                for d, p, c in official],
            "danjuan_series": [{"date": d, "pe": round(p, 4)} for d, p in djweekly],
            "danjuan_detail": djdetail,
        }
        meta_idx[code] = idx

        cc = idx["cross_check"]
        w = idx["_w10"]
        print("   10年: PE=%.2f 分位=%.1f%% 区间[%.2f, %.2f] 中位=%.2f" % (
            w["latest_pe"], w["latest_percentile"], w["min"], w["max"], w["median"]))
        if cc.get("danjuan_last"):
            print("   最新对比: 官方 %s PE=%.2f vs 蛋卷 %s PE=%.2f (差 %.2f%%)" % (
                cc["official_last"]["date"], cc["official_last"]["pe"],
                cc["danjuan_last"]["date"], cc["danjuan_last"]["pe"], cc["diff_pct"] or 0))

    meta = {
        "fetch_time": fetch_time,
        "order": [c for c, *_ in INDICES if c in meta_idx],
        "indices": meta_idx,
        "primary_source": "中证指数官网（一手来源，指数编制机构，日度）",
        "cross_source": "蛋卷基金（雪球）指数估值（周度）",
        "method_note": (
            "三个指数的主序列统一取中证指数官网发布的日度市盈率（一手来源，指数编制机构口径）。"
            "分位采用窗口内「≤ 计数法」计算；分位带 p10/p30/p50/p70/p90 由窗口内 PE 分布线性插值得到。"
            "蛋卷基金（雪球）口径与官方口径对 PE 的定义不同（主要差别在于对亏损股与净利润口径的处理），"
            "两者分位不可直接混用，页面在每个窗口下并列展示蛋卷口径的对照值。"
        ),
        "notes": notes,
    }

    for c in meta_idx:
        meta_idx[c].pop("_w10", None)

    with open(RAW_PATH, "w", encoding="utf-8") as f:
        json.dump(raw_out, f, ensure_ascii=False, separators=(",", ":"))
    with open(META_PATH, "w", encoding="utf-8") as f:
        json.dump(meta, f, ensure_ascii=False, separators=(",", ":"))

    print("\n✓ 完成 | 抓取时间 %s" % fetch_time)
    print("  %s" % RAW_PATH)
    print("  %s" % META_PATH)
    if notes:
        print("  ⚠ %s" % " | ".join(notes))


if __name__ == "__main__":
    main()
