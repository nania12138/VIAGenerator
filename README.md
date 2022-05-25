# VIAGenerator
***
用于自动生成via自动识别json文件，Qt6.2.4 + jsoncpp1.9.5，仅在Windows11 21H2中测试
非软件专业的代码菜鸡边百度边写的，难免有问题 ~~~碰到问题请自己解决~~~

## 使用方法
***
1. 选择（VIA 中import keymap 可识别的json文件） [格式参考](https://www.caniusevia.com/docs/specification)
2. 目标路径一般不用修改
3. 生成方式可选
 * 新建：删除原有的config.json，并以项目路径下的template.json为模板创建新config.json
 * 替换：不改变原有的config.json，如原有的config.json有输入文件相同的vendorproductid，则替换，如没有，则插入
 * 屏蔽更新：通过修改hosts，屏蔽打开via时config.json自动更新，**会使[via官网](https://caniusevia.com/)无法访问**
 
 
## 原理
***
* 修改用户目录/AppData/Roaming/VIA/config.json,层级可以看下图，只是遍历做了格式的转换，想了解可以看代码 ~~~没有注释，会很痛苦~~~
* 原文件地址：https://caniusevia.com/keyboards.v2.json（注意hosts有没有修改），release中的config.json也是一样的，二选一即可，当你搞砸的时候可以替换
http://assets.processon.com/chart_image/628b6ff56376893bcc138617.png
