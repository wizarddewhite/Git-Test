# 安装及配置

我参考的是[安装教程][1]进行的安装和启动。

然后配置了使用智谱的模型

```
{
  "env": {
    "ANTHROPIC_BASE_URL": "https://open.bigmodel.cn/api/anthropic",
    "ANTHROPIC_AUTH_TOKEN": "key.token",
    "ANTHROPIC_MODEL": "glm-4-flash"
  },
  "model": "glm-4-flash"
}
```

好消息是能用！当然效果不知道。

# 使用

## 用docker

做了一个docker镜像.是在官方node:16-stretch基础上npm install -g @anthropic-ai/claude-code再安装的。

```
sudo docker run -it --name my-claude -v /home/richard/Downloads/:/app/share my-claude-code:v2 bash
```

每次有永久更新，就退出再制作一个镜像

```
docker commit my-claude my-claude-code:final
```


[1]: https://daheiai.com/cc-install.html
