#ifndef ILAYOUTMANAGER_H
#define ILAYOUTMANAGER_H

#include <QString>
#include <QtPlugin>


class ILayoutManager
{
public:

    virtual QString GetVersion() = 0;
    virtual QString GetLastLayoutId() = 0;
    virtual QString GetLastLayoutJsonData() = 0;
    virtual qulonglong GetLayoutCounter() = 0;

    virtual void ResetDebugInfo() = 0;

};

#define ILayoutManager_iid "com.axeltechnology.ILayoutManager"
Q_DECLARE_INTERFACE(ILayoutManager, ILayoutManager_iid)

#endif // ILAYOUTMANAGER_H
