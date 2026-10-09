#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""把多指数 PE 数据内联进单页 HTML 工具，生成自包含的 index.html"""
import json, os

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
META = os.path.join(BASE, "data", "index_pe_meta.json")
OUT = os.path.join(BASE, "index.html")

meta = json.load(open(META, encoding="utf-8"))

# 每个指数一个主题色，用于 tab 指示条与 PE 曲线
ACCENT = {"000300": "#378ADD", "000016": "#7F77DD", "000905": "#BA7517"}

payload_obj = {
    "fetch_time": meta["fetch_time"],
    "primary_source": meta.get("primary_source", ""),
    "cross_source": meta.get("cross_source", ""),
    "method_note": meta.get("method_note", ""),
    "order": meta["order"],
    "indices": {},
}
for code in meta["order"]:
    src = meta["indices"][code]
    # 只保留 10 年窗口的完整序列，其余窗口由前端按 start_date 切片
    windows = {}
    for k, w in src["windows"].items():
        windows[k] = {kk: vv for kk, vv in w.items() if kk != "series"}
    payload_obj["indices"][code] = {
        "name": src["name"],
        "short": src["short"],
        "code": src["code"],
        "source": src["source"],
        "frequency": src["frequency"],
        "data_range": src["data_range"],
        "cross_check": src["cross_check"],
        "default_window": src.get("default_window", "10"),
        "accent": ACCENT.get(code, "#378ADD"),
        "windows": windows,
        "series": src["windows"]["10"]["series"] if "10" in src["windows"] else [],
    }

payload = json.dumps(payload_obj, ensure_ascii=False, separators=(",", ":"))

HTML = r"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>宽基指数 PE 分位工具</title>
<style>
:root{
  --bg:#f7f8fa; --panel:#ffffff; --line:#e5e7eb; --txt:#1f2328; --muted:#6b7280;
  --idx:#378ADD;
}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--txt);
  font-family:-apple-system,BlinkMacSystemFont,"Segoe UI","PingFang SC","Hiragino Sans GB","Microsoft YaHei",sans-serif;
  font-size:14px;line-height:1.6;-webkit-font-smoothing:antialiased}
.wrap{max-width:1180px;margin:0 auto;padding:24px 20px 48px}
header{margin-bottom:18px}
h1{font-size:22px;font-weight:600;margin:0 0 6px}
.sub{color:var(--muted);font-size:13px}
.card{background:var(--panel);border:1px solid var(--line);border-radius:12px;padding:18px 20px;margin-bottom:16px}
.tabcard{padding:6px 20px 0}
.tabs{display:flex;gap:4px;flex-wrap:wrap}
.tabs button{position:relative;border:0;background:transparent;cursor:pointer;font-family:inherit;
  padding:13px 20px 15px;text-align:left;color:var(--muted);border-radius:9px 9px 0 0;transition:background .15s,color .15s}
.tabs button b{display:block;font-size:15.5px;font-weight:600;color:inherit;line-height:1.3}
.tabs button span{display:block;font-size:11px;color:var(--muted);letter-spacing:.2px}
.tabs button em{display:block;font-style:normal;font-size:11px;color:var(--muted);margin-top:3px}
.tabs button:hover{background:#fafbfc}
.tabs button.on{color:var(--idx)}
.tabs button.on::after{content:'';position:absolute;left:14px;right:14px;bottom:0;height:3px;
  border-radius:3px 3px 0 0;background:var(--idx)}
.bar{display:flex;flex-wrap:wrap;align-items:center;gap:12px;justify-content:space-between}
.seg{display:inline-flex;background:#eef0f3;border-radius:9px;padding:3px;gap:2px}
.seg button{border:0;background:transparent;padding:7px 15px;border-radius:7px;cursor:pointer;
  font-size:13px;color:var(--muted);font-family:inherit;font-weight:500;transition:.15s}
.seg button.on{background:#fff;color:var(--txt);box-shadow:0 1px 3px rgba(0,0,0,.09)}
.seg button:hover:not(.on){color:var(--txt)}
.meta{font-size:12px;color:var(--muted);text-align:right;line-height:1.8}
.meta b{color:var(--txt);font-weight:500}
.kpis{display:grid;grid-template-columns:repeat(auto-fit,minmax(168px,1fr));gap:12px;margin-bottom:16px}
.kpi{background:var(--panel);border:1px solid var(--line);border-radius:12px;padding:16px 18px}
.kpi .lab{font-size:12px;color:var(--muted);margin-bottom:7px}
.kpi .val{font-size:27px;font-weight:600;letter-spacing:-.5px;line-height:1.15}
.kpi .hint{font-size:12px;color:var(--muted);margin-top:5px}
.chartbox{position:relative;height:400px}
.chartbox.sm{height:230px}
.legend{display:flex;flex-wrap:wrap;gap:16px;font-size:12px;color:var(--muted);margin-bottom:10px}
.legend i{display:inline-block;width:11px;height:11px;border-radius:2px;margin-right:5px;vertical-align:-1px}
h2{font-size:16px;font-weight:600;margin:0 0 14px;display:flex;align-items:center;gap:8px}
h2 small{font-weight:400;color:var(--muted);font-size:12px}
table{width:100%;border-collapse:collapse;font-size:13px;table-layout:fixed}
th,td{padding:10px 8px;text-align:right;border-bottom:1px solid var(--line)}
th:first-child,td:first-child{text-align:left}
th{color:var(--muted);font-weight:500;font-size:12px}
tbody tr:hover{background:#fafbfc}
tbody tr.hi{background:#fff8e6}
td.pct{font-weight:600}
.tag{display:inline-block;padding:2px 9px;border-radius:20px;font-size:12px;font-weight:500}
.t-low{background:#e8f6ee;color:#0d7a3d}
.t-mid{background:#fdf3e2;color:#9a6100}
.t-high{background:#fdeaea;color:#b02323}
.note{font-size:12px;color:var(--muted);line-height:1.8;margin-top:12px;padding-top:12px;border-top:1px solid var(--line)}
.note b{color:var(--txt);font-weight:500}
.warn{background:#fffaf0;border:1px solid #f2e2c4;border-radius:9px;padding:11px 14px;margin-top:12px;
  font-size:12px;line-height:1.75;color:#7a5a12}
.warn b{color:#5c4409}
.grid2{display:grid;grid-template-columns:1fr 1fr;gap:16px}
@media(max-width:820px){.grid2{grid-template-columns:1fr}.chartbox{height:320px}}
.foot{text-align:center;color:var(--muted);font-size:12px;margin-top:22px;line-height:1.8}
.ruler{position:relative;height:46px;margin:2px 0 16px}
.ruler .track{position:absolute;top:20px;left:0;right:0;height:8px;border-radius:5px;
  background:linear-gradient(90deg,#5DCAA5 0%,#97C459 28%,#EF9F27 55%,#F09595 78%,#E24B4A 100%)}
.ruler .mk{position:absolute;top:12px;width:3px;height:24px;background:#1f2328;border-radius:2px;transform:translateX(-50%)}
.ruler .mk span{position:absolute;top:26px;left:50%;transform:translateX(-50%);white-space:nowrap;font-size:12px;font-weight:600;color:#1f2328}
.ruler .sc{position:absolute;top:-4px;font-size:11px;color:var(--muted);transform:translateX(-50%)}
</style>
</head>
<body>
<div class="wrap">
  <header>
    <h1>宽基指数 PE 分位工具</h1>
    <div class="sub"><span id="hIdxName">沪深300</span> · 回溯 <span id="hRange">—</span> · 中证指数官网口径 · 滑动窗口可切换</div>
  </header>

  <div class="card tabcard">
    <div class="tabs" id="tabs"></div>
  </div>

  <div class="card">
    <div class="bar">
      <div>
        <div style="font-size:12px;color:var(--muted);margin-bottom:7px">回溯窗口</div>
        <div class="seg" id="seg">
          <button data-w="10">十年</button>
          <button data-w="7">七年</button>
          <button data-w="5">五年</button>
          <button data-w="3">三年</button>
        </div>
      </div>
      <div class="meta">
        <div>数据源：<b id="hSrc">—</b></div>
        <div>抓取时间：<b id="hTime">—</b></div>
        <div>交叉校验：<b id="hChk">—</b></div>
      </div>
    </div>
  </div>

  <div class="kpis">
    <div class="kpi">
      <div class="lab">当前 PE (TTM)</div>
      <div class="val" id="kPe">—</div>
      <div class="hint" id="kPeHint">—</div>
    </div>
    <div class="kpi">
      <div class="lab">历史分位</div>
      <div class="val" id="kPct">—</div>
      <div class="hint" id="kPctHint">—</div>
    </div>
    <div class="kpi">
      <div class="lab">区间最低 / 最高</div>
      <div class="val" id="kRange" style="font-size:20px">—</div>
      <div class="hint" id="kRangeHint">—</div>
    </div>
    <div class="kpi">
      <div class="lab">区间中位数</div>
      <div class="val" id="kMed">—</div>
      <div class="hint" id="kMedHint">—</div>
    </div>
  </div>

  <div class="card">
    <h2>当前分位位置 <small id="posSub"></small></h2>
    <div class="ruler" id="ruler">
      <div class="track"></div>
      <div class="mk" id="mkNow"><span id="mkNowTxt">—</span></div>
      <div class="sc" style="left:0%">0%</div>
      <div class="sc" style="left:25%">25%</div>
      <div class="sc" style="left:50%">50%</div>
      <div class="sc" style="left:75%">75%</div>
      <div class="sc" style="left:100%">100%</div>
    </div>
    <div class="grid2">
      <div>
        <div class="legend">
          <span><i id="lgPe"></i>PE 走势</span>
          <span><i style="background:#EF9F27"></i>区间中位数</span>
        </div>
        <div class="chartbox sm"><canvas id="cPe" role="img" aria-label="指数 PE 历史走势图">PE 走势图</canvas></div>
      </div>
      <div>
        <div class="legend">
          <span><i style="background:#5DCAA5"></i>分位 (%)</span>
          <span><i style="background:#e5e7eb"></i>50% 中位线</span>
        </div>
        <div class="chartbox sm"><canvas id="cPct" role="img" aria-label="指数 PE 历史分位走势图">分位走势图</canvas></div>
      </div>
    </div>
  </div>

  <div class="card">
    <h2>分位带与关键阈值 <small id="bandSub"></small></h2>
    <div class="chartbox"><canvas id="cBand" role="img" aria-label="PE 分布带状图与当前值位置">PE 分位带</canvas></div>
    <div class="note">
      <b>怎么读：</b>分位带由区间内 PE 的历史分布切分。PE 落在低分位区（&lt;30%）通常对应相对便宜的估值区间，
      高分位区（&gt;70%）对应相对偏贵的估值区间。分位只衡量"相较于自身历史"的相对位置，不代表绝对便宜或贵，也不构成买卖信号。
    </div>
  </div>

  <div class="card">
    <h2>各窗口分位对照 <small id="tblSub"></small></h2>
    <table>
      <thead>
        <tr><th style="width:14%">窗口</th><th>起始</th><th>PE 最低</th><th>PE 最高</th><th>中位数</th><th>30分位</th><th>70分位</th><th>当前分位</th><th style="width:12%">状态</th></tr>
      </thead>
      <tbody id="tbody"></tbody>
    </table>
    <div class="note" id="footNote"></div>
    <div class="warn" id="warnBox" style="display:none"></div>
  </div>

  <div class="foot">
    主数据源：中证指数官网（指数编制机构，日度市盈率）；交叉校验：蛋卷基金（雪球）指数估值。<br>
    本工具仅为数据整理与可视化，不构成任何投资建议。市场有风险，投资需谨慎。
  </div>
</div>

<script id="payload" type="application/json">__PAYLOAD__</script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/4.4.1/chart.umd.js"></script>
<script>
var META = JSON.parse(document.getElementById('payload').textContent);
var CH = {};
var curIdx = META.order[0];
var curWin = '10';

function IDX(){ return META.indices[curIdx]; }
function f2(v){ return (v==null||isNaN(v))?'—':Number(v).toFixed(2); }

function pctTag(p){
  if(p==null) return ['—',''];
  if(p<30) return ['低估区','t-low'];
  if(p<=70) return ['中性区','t-mid'];
  return ['高估区','t-high'];
}

function pctOf(v, sorted){
  var lo=0, hi=sorted.length;
  while(lo<hi){ var mid=(lo+hi)>>1; if(sorted[mid]<=v) lo=mid+1; else hi=mid; }
  return lo/sorted.length*100;
}

function buildTabs(){
  var html='';
  META.order.forEach(function(c){
    var d=META.indices[c], w=d.windows[d.default_window]||{}, tg=pctTag(w.latest_percentile);
    html += '<button data-idx="'+c+'" style="--idx:'+d.accent+'">'
      + '<b>'+d.name+'</b><span>'+d.code+'</span>'
      + '<em>10年分位 '+f2(w.latest_percentile)+'% · '+tg[0]+'</em></button>';
  });
  document.getElementById('tabs').innerHTML = html;
}

function slice(d, w){
  if(+w.years===10) return d.series;
  return d.series.filter(function(x){ return x.date >= w.start_date; });
}

function render(){
  var d = IDX(), w = d.windows[curWin];
  if(!w) return;
  document.documentElement.style.setProperty('--idx', d.accent);

  Array.prototype.forEach.call(document.querySelectorAll('#tabs button'), function(b){
    b.classList.toggle('on', b.dataset.idx===curIdx);
  });
  Array.prototype.forEach.call(document.querySelectorAll('#seg button'), function(b){
    b.classList.toggle('on', b.dataset.w===curWin);
  });

  document.getElementById('hIdxName').textContent = d.name + '（' + d.code + '）';
  document.getElementById('hRange').textContent =
    d.data_range.start+' ~ '+d.data_range.end+'（'+d.data_range.count+' 条 / '+d.frequency+'）';
  document.getElementById('hSrc').textContent = d.source;
  document.getElementById('hTime').textContent = META.fetch_time;
  var cc = d.cross_check || {};
  if(cc.danjuan_last && cc.official_last){
    document.getElementById('hChk').textContent =
      '蛋卷 '+f2(cc.danjuan_last.pe)+' vs 官方 '+f2(cc.official_last.pe)
      +'（差 '+f2(cc.diff_pct)+'%）';
  } else {
    document.getElementById('hChk').textContent = '未取到';
  }

  document.getElementById('kPe').textContent = f2(w.latest_pe);
  document.getElementById('kPe').style.color = d.accent;
  document.getElementById('kPeHint').textContent = '最新 ' + w.end_date + '（' + d.frequency + '）';
  var tg = pctTag(w.latest_percentile);
  document.getElementById('kPct').innerHTML = f2(w.latest_percentile)
    + '% <span class="tag '+tg[1]+'" style="font-size:12px;vertical-align:2px">'+tg[0]+'</span>';
  document.getElementById('kPctHint').textContent = '近 ' + w.years + ' 年 PE 分布中的位置';
  document.getElementById('kRange').textContent = f2(w.min) + ' / ' + f2(w.max);
  document.getElementById('kRangeHint').textContent = '期间极值区（低 / 高）';
  document.getElementById('kMed').textContent = f2(w.median);
  document.getElementById('kMedHint').textContent = '区间 50 分位对应 PE';

  document.getElementById('lgPe').style.background = d.accent;
  document.getElementById('posSub').textContent = d.name + ' · 近 ' + w.years + ' 年 · ' + w.start_date + ' 起';
  var mk = document.getElementById('mkNow');
  mk.style.left = Math.min(100, Math.max(0, w.latest_percentile)) + '%';
  document.getElementById('mkNowTxt').textContent = '当前 ' + f2(w.latest_percentile) + '%';

  document.getElementById('bandSub').textContent = d.name + ' · 近 ' + w.years + ' 年';
  document.getElementById('tblSub').textContent = d.name;
  document.getElementById('footNote').innerHTML =
    '统计窗口：' + w.start_date + ' ~ ' + w.end_date + '，共 ' + w.count + ' 个' + d.frequency + '观测。'
    + '当前 PE <b>' + f2(w.latest_pe) + '</b>，处于该窗口 <b>' + f2(w.latest_percentile) + '%</b> 分位。'
    + (w.cross_percentile!=null
        ? '<br><span style="color:#8a8f98">对照：同一窗口按蛋卷基金口径计算，最新 PE '
          + f2(w.cross_last_pe) + '，分位 <b>' + f2(w.cross_percentile) + '%</b>（口径不同，仅供参考）。</span>'
        : '');

  var wb = document.getElementById('warnBox');
  if(cc.diff_pct!=null && Math.abs(cc.diff_pct)>=8){
    wb.style.display = 'block';
    wb.innerHTML = '<b>口径提示：</b>' + d.name + ' 在中证指数官网与蛋卷基金两个数据源的 PE 差异较大（最新时点相差 '
      + f2(Math.abs(cc.diff_pct)) + '%）。原因是两者对市盈率的计算口径不同，'
      + '主要差别在于对亏损股与净利润口径的处理方式（中证500 成分股中亏损股占比高于沪深300与上证50，差异因此更明显）。'
      + '本页所有分位均基于<b>中证指数官网口径</b>计算，与雪球/蛋卷等平台的分位数值不可直接混用。';
  } else {
    wb.style.display = 'none';
  }

  var seg = slice(d, w);
  drawPe(d, w, seg);
  drawPct(d, w, seg);
  drawBand(d, w);
  renderTable(d);
}

function axisStyle(){
  return { grid:{color:'#eef0f3'}, ticks:{color:'#6b7280',font:{size:11}},
           border:{color:'#e5e7eb'} };
}

function drawPe(d, w, seg){
  var labels = seg.map(function(x){return x.date;});
  var data = seg.map(function(x){return x.pe;});
  var med = seg.map(function(){return w.median;});
  if(CH.pe) CH.pe.destroy();
  CH.pe = new Chart(document.getElementById('cPe'), {
    type:'line',
    data:{ labels:labels, datasets:[
      {label:'PE', data:data, borderColor:d.accent, backgroundColor:d.accent+'1f',
       borderWidth:1.5, pointRadius:0, tension:.2, fill:true},
      {label:'中位数', data:med, borderColor:'#EF9F27', borderWidth:1.3,
       borderDash:[5,4], pointRadius:0, fill:false}
    ]},
    options:{ responsive:true, maintainAspectRatio:false, animation:{duration:350},
      interaction:{mode:'index',intersect:false},
      plugins:{legend:{display:false},
        tooltip:{callbacks:{label:function(c){return c.dataset.label+': '+Number(c.parsed.y).toFixed(2);}}}},
      scales:{ x:Object.assign({},axisStyle(),{ticks:{color:'#6b7280',font:{size:11},maxTicksLimit:6,autoSkip:true}}),
               y:Object.assign({},axisStyle(),{ticks:{color:'#6b7280',font:{size:11}}}) }
    }
  });
}

function drawPct(d, w, seg){
  var labels = seg.map(function(x){return x.date;});
  var pes = seg.map(function(x){return x.pe;});
  var sorted = pes.slice().sort(function(a,b){return a-b;});
  var data = pes.map(function(v){return pctOf(v, sorted);});
  if(CH.pct) CH.pct.destroy();
  CH.pct = new Chart(document.getElementById('cPct'), {
    type:'line',
    data:{ labels:labels, datasets:[
      {label:'分位', data:data, borderColor:'#1D9E75', backgroundColor:'rgba(93,202,165,.16)',
       borderWidth:1.5, pointRadius:0, tension:.2, fill:true},
      {label:'50%', data:seg.map(function(){return 50;}), borderColor:'#B4B2A9',
       borderWidth:1, borderDash:[4,4], pointRadius:0, fill:false}
    ]},
    options:{ responsive:true, maintainAspectRatio:false, animation:{duration:350},
      interaction:{mode:'index',intersect:false},
      plugins:{legend:{display:false},
        tooltip:{callbacks:{label:function(c){return c.dataset.label+': '+Number(c.parsed.y).toFixed(1)+'%';}}}},
      scales:{ x:Object.assign({},axisStyle(),{ticks:{color:'#6b7280',font:{size:11},maxTicksLimit:6,autoSkip:true}}),
               y:Object.assign({},axisStyle(),{min:0,max:100,ticks:{color:'#6b7280',font:{size:11},stepSize:25}}) }
    }
  });
}

function drawBand(d, w){
  var now = w.latest_pe;
  var bands = [
    {n:'0-30分位', lo:w.min, hi:w.p30, c:'#5DCAA5'},
    {n:'30-50分位', lo:w.p30, hi:w.p50, c:'#97C459'},
    {n:'50-70分位', lo:w.p50, hi:w.p70, c:'#EF9F27'},
    {n:'70-90分位', lo:w.p70, hi:w.p90, c:'#F09595'},
    {n:'90-100分位', lo:w.p90, hi:w.max, c:'#E24B4A'}
  ];
  var span = w.max - w.min;
  var lo = w.min - span*0.10, hi = w.max + span*0.10;
  if(CH.band) CH.band.destroy();
  CH.band = new Chart(document.getElementById('cBand'), {
    type:'bar',
    data:{ labels:bands.map(function(b){return b.n;}),
      datasets:[{label:'分位区间', data:bands.map(function(b){return [b.lo,b.hi];}),
        backgroundColor:bands.map(function(b){return b.c;}), borderWidth:0, borderRadius:4, barPercentage:.62}]},
    options:{ responsive:true, maintainAspectRatio:false, indexAxis:'y', animation:{duration:350},
      plugins:{ legend:{display:false},
        tooltip:{callbacks:{label:function(c){var b=bands[c.dataIndex];
          return b.lo.toFixed(2)+' ~ '+b.hi.toFixed(2);}}}},
      scales:{
        x:{ min:lo, max:hi, grid:{color:'#eef0f3'}, ticks:{color:'#6b7280',font:{size:11}},
            border:{color:'#e5e7eb'}, title:{display:true,text:d.name+' PE',color:'#6b7280',font:{size:11}}},
        y:{ grid:{display:false}, ticks:{color:'#6b7280',font:{size:11}}, border:{color:'#e5e7eb'} }
      }
    },
    plugins:[{
      id:'nowLine',
      afterDatasetsDraw:function(chart){
        var xa=chart.scales.x, ya=chart.scales.y, ctx=chart.ctx;
        var x = xa.getPixelForValue(now);
        if(x<xa.left||x>xa.right) return;
        ctx.save();
        ctx.strokeStyle='#1f2328'; ctx.lineWidth=2; ctx.setLineDash([5,4]);
        ctx.beginPath(); ctx.moveTo(x, ya.top); ctx.lineTo(x, ya.bottom); ctx.stroke();
        ctx.setLineDash([]);
        var t='当前 '+now.toFixed(2);
        ctx.font='600 12px -apple-system,PingFang SC,sans-serif';
        var tw=ctx.measureText(t).width+12;
        var bx=Math.min(x+8, xa.right-tw);
        ctx.fillStyle='#1f2328';
        ctx.beginPath(); ctx.roundRect(bx, ya.top+4, tw, 21, 5); ctx.fill();
        ctx.fillStyle='#fff'; ctx.textBaseline='middle';
        ctx.fillText(t, bx+6, ya.top+15);
        ctx.restore();
      }
    }]
  });
}

function renderTable(d){
  var html='';
  ['10','7','5','3'].forEach(function(k){
    var w = d.windows[k];
    if(!w) return;
    var tg = pctTag(w.latest_percentile);
    html += '<tr class="'+(k===curWin?'hi':'')+'">'
      + '<td>近 '+w.years+' 年</td>'
      + '<td>'+w.start_date+'</td>'
      + '<td>'+f2(w.min)+'</td>'
      + '<td>'+f2(w.max)+'</td>'
      + '<td>'+f2(w.median)+'</td>'
      + '<td>'+f2(w.p30)+'</td>'
      + '<td>'+f2(w.p70)+'</td>'
      + '<td class="pct">'+f2(w.latest_percentile)+'%</td>'
      + '<td><span class="tag '+tg[1]+'">'+tg[0]+'</span></td>'
      + '</tr>';
  });
  document.getElementById('tbody').innerHTML = html;
}

document.getElementById('tabs').addEventListener('click', function(e){
  var b = e.target.closest('button');
  if(!b) return;
  curIdx = b.dataset.idx;
  render();
});

document.getElementById('seg').addEventListener('click', function(e){
  var b = e.target.closest('button');
  if(!b) return;
  curWin = b.dataset.w;
  render();
});

buildTabs();
curWin = IDX().default_window || '10';
render();
</script>
</body>
</html>
"""

html = HTML.replace("__PAYLOAD__", payload)
with open(OUT, "w", encoding="utf-8") as f:
    f.write(html)
print("✓ 生成 %s (%.1f KB)" % (OUT, os.path.getsize(OUT) / 1024))
