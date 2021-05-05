#!/usr/bin/env python
# -*- coding:utf-8 -*-
# author: mx time:2017/10/31


import sys
import ctypes
import urllib.request
import http.cookiejar

from lxml import etree
from lxml.etree import ParseError

import subprocess
import win32api

def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False

def get_ip(url, xp):

    try:
        '''
        print ('第一种方法')
        response1=urllib.request.urlopen(url)
        print (response1.getcode())
        print (len(response1.read()))

        #'''

        # '''
        print('第二种方法')
        request = urllib.request.Request(url)
        request.add_header("user-agent", "Mozilla/5.0")  # 将爬虫伪装成浏览器
        response2 = urllib.request.urlopen(request)
        print(response2.getcode())  # 打印状态码

        htmlStr = str(response2.read())
        # '''

        '''
        print ('第三种方法')
        cj = http.cookiejar.CookieJar()
        opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(cj))
        urllib.request.install_opener(opener)
        response3=urllib.request.urlopen(url)
        print (response1.getcode())
        print (cj)   #输出cookie
        print (response1.read())

        #'''
    except urllib2.URLError as e:
        print("urlopen excpetion:", e.reason)  # 输出错误原因

    try:

        html = etree.HTML(htmlStr, etree.HTMLParser())  # 解析HTML文本内容
        #result = html.xpath('//html/body/div/main/section/table/tbody/tr/td/ul[@class="comma-separated"]/li/text()')
        result = html.xpath(xp)

        print(result)

        if(len(result) >= 1):
            return result[0]
        
    except ParseError as e:
        print("Unexpected error:", sys.exc_info()[0])
    
    return ''
if __name__ == "__main__":
    # execute only if run as a script
    if(not is_admin()):
        print("please run with admin :", __file__)
        
        win32api.ShellExecute(0, 'runas', 'python', __file__,'',1)
    else:

        ip1 = get_ip('https://fastly.net.ipaddress.com/github.global.ssl.fastly.net#ipinfo', '/html/body/div/main/section/table/tbody/tr/td/ul[@class="comma-separated"]/li/text()')
        print('github.global.ssl.fastly.net :', ip1)
        ip2 = get_ip('https://github.com.ipaddress.com/#ipinfo', '/html/body/div/main/section/table/tbody/tr/td/ul[@class="comma-separated"]/li/text()')
        print('github.com :', ip2)

        fr = open('C:\\Windows\\System32\\drivers\\etc\\hosts')
        lines=fr.readlines()
        fr.close()

        fw = open('C:\\Windows\\System32\\drivers\\etc\\hosts', 'w')

        find_ip1=False
        find_ip2=False
        for line in lines:
            if(line.find(" github.global.ssl.fastly.net") > 0):
                line = ip1 + ' github.global.ssl.fastly.net'
                find_ip1 = True
            elif (line.find(" github.com") > 0):
                line = ip2 + ' github.com'
                find_ip2 = True
            
            fw.write(line)

        if(not find_ip1 or not find_ip2):
            fw.write('\n')

        if(not find_ip1):
            line = ip1 + ' github.global.ssl.fastly.net' + '\n'
            fw.write(line)
        if(not find_ip2):
            line = ip2 + ' github.com' + '\n'
            fw.write(line)

        fw.close()
