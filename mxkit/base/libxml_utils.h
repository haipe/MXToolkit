#pragma once

#ifdef _MX_USE_LIBXML

#include <functional>

#include "libxml/tree.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_

class LibXmlUtils
{
public:
    static xmlXPathObjectPtr GetNode(xmlDocPtr pDoc, const xmlChar* xpath)
    {
        xmlXPathContextPtr context = NULL;//XPath上下文指针
        xmlXPathObjectPtr result = NULL; //XPath结果指针
        context = xmlXPathNewContext(pDoc);

        if (pDoc == NULL)
        {
            printf("pDoc is NULL\n");
            return NULL;
        }

        if (xpath)
        {
            if (context == NULL)
            {
                printf("context is NULL\n");
                return NULL;
            }

            result = xmlXPathEvalExpression(xpath, context);
            xmlXPathFreeContext(context); //释放上下文指针
            if (result == NULL)
            {
                printf("xmlXPathEvalExpression return NULL\n");
                return NULL;
            }

            if (xmlXPathNodeSetIsEmpty(result->nodesetval))
            {
                xmlXPathFreeObject(result);
                printf("find node is empty\n");
                return NULL;
            }
        }

        return result;
    }

    static void ReleaseNode(xmlXPathObjectPtr node)
    {
        xmlXPathFreeObject(node);//释放结果指针
    }

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
    static int LoadResult(xmlDocPtr xmlDoc, const xmlChar* xpath, std::function<bool(xmlNodePtr)> onLoadNode)
    {
        if (!onLoadNode)
            return 0;

        xmlXPathObjectPtr result = mxkit::GetNode(xmlDoc, xpath); //查询XPath表达式，得到一个查询结果
        if (result == NULL)
        {
            return 0;
        }

        xmlNodeSetPtr nodeSet = result->nodesetval; //获取查询到的节点指针集合
        xmlNodePtr cur;
        int cnt = 0;
        for (int i = 0; i < nodeSet->nodeNr; i++)
        {
            cnt++;
            cur = nodeSet->nodeTab[i];
            if (!onLoadNode(cur))
                break;
        }

        mxkit::ReleaseNode(result);//释放结果指针
        return cnt;
    }


};


_END_MX_KIT_NAME_SPACE_

#endif