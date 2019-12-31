#pragma once
#include <functional>

#include "libxml/tree.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"

namespace mxtoolkit
{
    xmlXPathObjectPtr GetNode(xmlDocPtr pDoc, const xmlChar *xpath);
    void ReleaseNode(xmlXPathObjectPtr node);

    /*
    <?xml version="1.0" encoding="UTF-8"?>
    <root>
        <Host></Host>
        <App version="1.0.0.0" name="MXWasai.exe">
            <File library="miniblink" version="20191213" MD5="ds">node.dll</File>
            <File library="qt" version="5.13.2" MD5="dsa">D3Dcompiler_47.dll</File>
        </App>
        <App version="1.0.0.1" name="MXWasai.exe">
            <File library="miniblink" version="20191213" MD5="ds">node.dll</File>
            <File library="qt" version="5.13.2" MD5="dsa">D3Dcompiler_47.dll</File>
        </App>
    </root>
    */

    //获取节点：xpath=//节点1//节点2
    //示例    ：获取结果为所有路径为//App//File的节点
    //          xpath=//App//File              

    //获取节点，并判断节点值  ：xpath=//节点[子节点名='xxxx']
    //示例                    ：获取结果为所有路径为//App//File的节点，且File的内容为node.dll
    //                          xpath=//App[File=node.dll]
    
    //获取节点，并判断节点属性：xpath=//节点[@节点属性='xxxx']
    //示例                    ：获取结果为所有路径为//App//File的节点，且App存在两个属性name="MXWasai.exe" version="1.0.0.0"
    //                          xpath=App[@name=\"MXWasai.exe\" and @version=\"1.0.0.0\"]//File

    //ps：所有[ ] 内的多个判断，或者多个 xpath 的拼接，都可以用and or 等逻辑进行再次判断
    //[text()='xxx'] 节点的内容='xxx'
    int LoadResult(xmlDocPtr xmlDoc, const xmlChar *xpath, std::function<bool(xmlNodePtr)> onLoadNode);

}