mod_replace_handler
=============================================
mod_replace_handler can change the hadler configured by the user.

[![][MIT]][MIT-Link]

Requirements
--------------------------------------------------------------------------------
- Apache Development tools (CentOS:httpd-devel, ubuntsu:apach2-dev, etc)
- gcc
- make

Install
--------------------------------------------------------------------------------
- Build on Linux

```
git clone https://github.com/harasou/mod_replace_handler
cd mod_replace_handler

make
make install
```


Sample Setting
--------------------------------------------------------------------------------

user setting on .htaccess
```
AddHandler php5.2-script .php
```

server setting on httpd.conf
```
LoadModule replace_handler_module modules/mod_replace_handler.so

<Ifmodule mod_replace_handler.c>
ReplaceHandler php5.2-script php5.6-script
ReplaceHandler php5.3-script php5.6-script
</Ifmodule>
```

The user's php is executed in the php5.6-script handler.

License
--------------------------------------------------------------------------------
Paddington is licensed under the [MIT][MIT-Link] license.
Copyright (c) 2020 SOGO Haraguchi


<!-- links -->
[MIT]: https://img.shields.io/github/license/mashape/apistatus.svg?style=flat-square
[MIT-Link]: https://github.com/harasou/exbind-fuse/blob/master/LICENSE
