#pragma once
#include <functional>

#include "libxml/tree.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"

namespace mxtoolkit
{
    xmlXPathObjectPtr GetNode(xmlDocPtr pDoc, const xmlChar *xpath);
    void ReleaseNode(xmlXPathObjectPtr node);

    int LoadResult(xmlDocPtr xmlDoc, const xmlChar *xpath, std::function<bool(xmlNodePtr)> onLoadNode);

}