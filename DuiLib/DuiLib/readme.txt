jason ---- 2019-07-26 同步最新代码

commit eb5db5c5e432cc3fca05fdbac80a90cfa8d56b3b (HEAD -> master, origin/master, origin/HEAD)
Author: Troy <qdtroy@qq.com>
Date:   Tue May 28 10:37:33 2019 +0800

commit 63e74fed1bd6a3cde730451d64f50b414f3113ea
Author: Troy <qdtroy@qq.com>
Date:   Tue May 28 10:31:04 2019 +0800

    1、修正默认字体高分屏适配错误的问题

commit 4d01b2cb39dc5b862aab98bdcea75ea664585faf
Author: Troy <qdtroy@qq.com>
Date:   Tue May 28 10:26:27 2019 +0800

    1、修正RichEdit光标闪烁异常
    2、修正滚动条内容过多时拖动异常的问题

commit a69091d175025bfd94bea5a6b84330a2bb826b4b
Author: Troy <qdtroy@qq.com>
Date:   Mon May 27 23:20:25 2019 +0800

commit 31a709c8dd0fe1cd09a7a7ed78a4d58a60661df5
Author: Troy <qdtroy@qq.com>
Date:   Thu Apr 11 11:16:39 2019 +0800

    1、Combo支持异形
    2、修正滚动条区域计算问题

commit 01c548469fa80815bfd3883f6b3473024576b67e
Author: Troy <qdtroy@qq.com>
Date:   Wed Apr 10 11:07:09 2019 +0800

    1、优化ListHeader布局方式

commit 79e1f367cc16b7ea90421abeaceca0fbd8bca921
Author: Troy <qdtroy@qq.com>
Date:   Tue Apr 2 10:42:05 2019 +0800

    1、Style样式支持样式继承：<Style name="childStyle" value="style=&quot;parentStyle&quot;" />（注意style的顺序）
    2、容器滚动条的EndDown和EndRight实时生效，不再需要定时器延时（注意不要频繁调用）

commit 916e4a0ec9625ef7a30a61b04267acf051c2d54f
Author: Troy <qdtroy@qq.com>
Date:   Mon Apr 1 09:28:15 2019 +0800

    修正autocaclwidth失效的问题

commit e4c3cea79959b1703328322e9ed11c6c0d21481b
Author: Troy <qdtroy@qq.com>
Date:   Wed Mar 27 15:28:46 2019 +0800

    1、改进 - 移除RichEdit重复属性代码
    2、改进 - 圆角矩形绘制消除锯齿（使用Gdiplus绘制）
    3、修正 - Button和Option控件禁用状态切换时刷新不及时的问题

commit c1daa6253ef0c50548369f2241c4477023645512
Author: Troy <qdtroy@qq.com>
Date:   Tue Mar 26 11:47:33 2019 +0800

    1、统一图片资源解析代码

commit 7d74ed5c10a3395e2bf5d4254bb96bfb64941b3f
Author: Troy <qdtroy@qq.com>
Date:   Fri Mar 15 17:03:11 2019 +0800

    Update issue templates

commit 3679920c4c8f14361d26e999f2947cdfa005dfb9
Author: Troy <qdtroy@qq.com>
Date:   Fri Mar 15 16:58:34 2019 +0800

    Update README.md

commit 44c48c877bd4a6022385694f6fc50dd605d5ce3c
Author: Troy <qdtroy@qq.com>
Date:   Tue Jan 22 10:16:44 2019 +0800

commit d901dacc366a8a374a18e347be6dba1be70e3a7d
Author: Troy <qdtroy@qq.com>
Date:   Tue Jan 22 09:51:46 2019 +0800

commit e5e335112d058a0c523915f5303543619d9d3369
Author: Troy <qdtroy@qq.com>
Date:   Fri Jan 11 13:49:16 2019 +0800

    分割控件适配DPI
    阴影显示异常的问题

commit 8f4677fe831b200c3555aeacf1c31f7471ac0d06
Author: Troy <qdtroy@qq.com>
Date:   Mon Dec 10 11:49:07 2018 +0800

    1、异步清理失败的问题
    2、统一自定义消息

loki ---- 2018-12-01 同步最新代码 
=====================================================
commit a49f144bae369ef180cdfdd1f99db6ad931f59a8 (HEAD -> master, origin/master, origin/HEAD)
Author: Troy <qdtroy@qq.com>
Date:   Tue Oct 30 14:54:45 2018 +0800

    修正Button禁用时的背景色绘制错误问题

loki ---- 2018-06-23 同步最新代码
 =====================================================

commit 11f53b8f00f678ecbfe3ccaf701fe9f307992243
Merge: 4ebe9eb 527cb96
Author: Troy <qdtroy@qq.com>
Date:   Tue Jun 12 13:41:34 2018 +0800

    Merge pull request #69 from ngugc/revert-back-GetTextSize

    revert back GetTextSize which was deleted by commit 09d6e9e2ea24a3c0f…