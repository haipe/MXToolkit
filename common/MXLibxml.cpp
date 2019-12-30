#include "MXLibxml.h"

namespace mxtoolkit
{
    xmlXPathObjectPtr GetNode(xmlDocPtr pDoc, const xmlChar *xpath)
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

    void ReleaseNode(xmlXPathObjectPtr node)
    {
        xmlXPathFreeObject(node);//释放结果指针
    }

    int LoadResult(xmlDocPtr xmlDoc, const xmlChar *xpath, std::function<bool(xmlNodePtr)> onLoadNode)
    {
        if (!onLoadNode)
            return 0;

        xmlXPathObjectPtr result = mxtoolkit::GetNode(xmlDoc, xpath); //查询XPath表达式，得到一个查询结果
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
            if(!onLoadNode(cur))
                break;
        }

        mxtoolkit::ReleaseNode(result);//释放结果指针
        return cnt;
    }

}