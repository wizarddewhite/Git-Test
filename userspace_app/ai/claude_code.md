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

# 通过docker使用


做了一个docker镜像.是在官方node:16-stretch基础上npm install -g @anthropic-ai/claude-code再安装的。

```
sudo docker run -it --name my-claude -v /home/richard/Downloads/:/app/share my-claude-code:v2 bash
```

每次有永久更新，就退出再制作一个镜像

```
docker commit my-claude my-claude-code:final
```

# session

## 恢复到之前的某个会话

```
claude -r
```

会有一个会话的列表，选择想要恢复的，按回车。

# skill

## 显示已安装skill

```
/skills
```

## init项目

通常用命令

```
/init
```

但是像内核这种太大了，可以直接手写一个CLAUDE.md文件

```
Linux 内核 - mm 子系统开发指南
==================================

项目标识
--------
- 类型: Linux 内核内存管理子系统
- 源码位置: /mm/ 目录
- 内核版本: 7.0
- 工作目录: /home/richard/git/linux

核心构建命令
-----------

完整内核编译
------------
# 在 linux/ 根目录
make defconfig          # 生成默认配置
make -j$(nproc)         # 多线程编译
make -j$(nproc) bzImage  # 仅编译内核镜像

模块化编译
---------
# 仅编译 mm/ 子系统
make mm/

开发快速迭代
-----------
# 仅重新编译修改过的文件
make mm/ -j$(nproc)

# 检查编译结果
ls mm/*.o

代码结构
-------

关键文件映射
-----------
路径                   功能说明                    修改频率
------------------------------------------------------------------------
mm/page_alloc.c    物理页框分配器（伙伴系统）        低
mm/slub.c          SLUB 分配器（小对象缓存）        中
mm/vmalloc.c       虚拟内存连续区域分配             中
mm/vmscan.c        页面回收（LRU/kswapd）          高
mm/memory.c        进程地址空间管理                中
mm/mmap.c          内存映射（mmap）实现            中

头文件依赖
---------
// 必须包含的头文件
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/gfp.h>
#include <linux/page-flags.h>
#include <linux/vmalloc.h>

代码风格
-------
- 遵循内核编码规范: Linux kernel coding style
- 检查工具: scripts/checkpatch.pl -f mm/your_file.c
- 重要约定: 使用 GFP_KERNEL 等标准标志，避免直接 malloc
- 锁的规则: 优先使用 spin_lock 而非 mutex，明确锁的粒度

调试与测试
---------

静态检查
-------
# 代码风格检查
./scripts/checkpatch.pl --file mm/your_new_file.c

# 稀疏（Sparse）静态分析
make C=1 CHECK="sparse" mm/

# Smatch 分析
make mm/ CHECK="smatch"

运行时调试
--------
// 常用调试宏
pr_debug("mm: 调试信息 %p\n", ptr);
WARN_ON(condition);
BUG_ON(严重错误);
dump_stack();  // 打印调用栈

内存调试功能
----------
# 配置内核时开启
CONFIG_DEBUG_KMEMLEAK=y    # 内存泄漏检测
CONFIG_DEBUG_VM=y         # 虚拟内存调试
CONFIG_DEBUG_PAGEALLOC=y  # 页分配调试

常见开发任务
-----------

1. 添加新内存分配器
-----------------
// 参考模板: mm/slub.c
static struct kmem_cache *my_cache;
my_cache = KMEM_CACHE(my_object, SLAB_PANIC);

2. 修改页面回收策略
-----------------
- 修改点: mm/vmscan.c 中的 shrink_node
- 关键函数: get_scan_count() 计算扫描比例
- 测试命令: echo 1 > /proc/sys/vm/drop_caches

3. 处理 DMA 内存
--------------
// 一致性 DMA 映射
dma_addr = dma_map_single(dev, buf, size, direction);
// 流式 DMA 映射
dma_addr = dma_map_page(dev, page, offset, size, direction);

提交与验证
---------

Git 工作流
--------
# 创建补丁
git format-patch -1 --subject-prefix="PATCH mm"]

# 检查邮件列表: https://lore.kernel.org/linux-mm/

测试套件
-------
# 内存压力测试
stress-ng --vm 2 --vm-bytes 1G --timeout 60s

# mm 子系统专项测试
make -C tools/testing/selftests/mm run_tests

注意事项
-------

[重要警示]
1. 修改 include/linux/mm.h 等公共头文件需全局验证
2. 确保 CONFIG_DEBUG 下编译无警告
3. 跨架构兼容性: x86_64, arm64, riscv
4. 性能回归: 用 perf 测试 mm_page_alloc 等事件

```


[1]: https://daheiai.com/cc-install.html
