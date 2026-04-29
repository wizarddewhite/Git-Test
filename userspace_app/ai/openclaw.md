# 启动
openclaw gateway run

openclaw gateway start 后台运行
openclaw gateway stop 停止后台

# 访问web端

##  获取带 token 的完整 URL

openclaw dashboard 

# 模型

显示配置的模型

openclaw models list

设置默认模型

openclaw models set zai/glm-4.7-flash

检查模型连接情况

openclaw models status --probe

## 智谱

[智谱AI开放平台][1]

创建一个api key，并获得key id 和 key。填入配置中。

我在openclaw.json配置文件中没有找到这个，不知道是不是加密存储了。

但是在agents.defaults.model.primary中设置了用智谱的模型。

也可以用openclaw models来查看这个信息。

## 开源

[future repo][2]

```
model = "gpt-5.2"
model_reasoning_effort = "xhigh"
model_provider = "91vip"

[model_providers.91vip]
name = "91vip"
base_url = "https://91vip.futureppo.top/v1"
wire_api = "responses"
requires_openai_auth = true
```

# 配置

打开交互配置界面

openclaw configure

openclaw onboard 调出最开始初始化的界面


# 连接飞书

要到[开发者后台-飞书开放平台][3]上申请appid以及权限。

我是参考了[15分钟用OpenClaw+GLM5打造国内最强AI助手][4]中5分30秒开始的内容配置的。

作为一个偷懒的人，其实不用连接也能用。就直接用web页面版直接调用大模型就行了。

# 问题排查

日志查看

openclaw logs --follow

# 升级

openclaw update

[1]: https://bigmodel.cn/usercenter/settings/account
[2]: https://91vip.futureppo.top/console/token
[3]: https://open.feishu.cn/app?lang=zh-CN
[4]: https://www.bilibili.com/video/BV1rvcpzDEsH/?spm_id_from=333.1007.top_right_bar_window_history.content.click&vd_source=e3c29015092663e6a072187b5b84c181
