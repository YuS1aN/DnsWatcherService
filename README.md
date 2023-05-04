# DnsWatcherService

配合<a href="https://github.com/jeessy2/ddns-go">DDNS-GO</a>的Webhook使用，实现内网搭建的DNS服务器ip更改后同步更新Windows的ipv6 DNS。
<br/><br/>
`.\DnsWatcherService.exe --run`开启服务端，默认地址为 `http://0.0.0.0:11451/webhook`，目标网卡为`以太网`
<br/><br/>
支持安装服务 `.\DnsWatcherService.exe --install`并在 `services.msc`服务面板中手动开启或将其设置为自动启动。<br/><br/>
卸载服务 `.\DnsWatcherService.exe --uninstall`


## 感谢
<a href="https://github.com/cesanta/mongoose">cesanta/mongoose</a> Embedded Web Server / Embedded Networking Library
