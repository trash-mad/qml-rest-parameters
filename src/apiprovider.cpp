#include "apiprovider.h"

/*****************************************************************************/

Element::Element(QString type, QLinkedList<Element *> child)
  : QObject(nullptr) {
    qDebug() << "Element ctor";
    this->child=child;
    this->type=type;
    QLinkedList<Element*>::iterator iter;
    for (iter=child.begin();iter!=child.end();iter++) {
        Element* elem = (*iter);
        elem->setParent(this);
    }
}

/*---------------------------------------------------------------------------*/

Element::Element() {
    qDebug() << "Element dtor";
}

/*---------------------------------------------------------------------------*/

QLinkedList<Element *> Element::getChild() const {
    return child;
}

/*---------------------------------------------------------------------------*/

QString Element::getType() const {
    return type;
}

/*---------------------------------------------------------------------------*/

void Element::printTree(Element *root) {
    static int space=0;
    char* tab = new char[space];
    for (int i=0;i!=space;i++) {
        tab[i]=' ';
    }
    qInfo() << QString::fromStdString(std::string(tab)) << root->getType();
    space+=4;
    QLinkedList<Element*> child=root->getChild();
    QLinkedList<Element*>::iterator iter;
    for (iter=child.begin();iter!=child.end();iter++) {
        printTree(*iter);
    }
    space-=4;
    delete tab;
}

/*****************************************************************************/

ApiProvider::ApiProvider(QJSEngine* engine)
  : QObject(nullptr) {
    qDebug() << "ApiProvider ctor";
    this->engine=engine;
}

/*---------------------------------------------------------------------------*/

ApiProvider::~ApiProvider() {
    qDebug() << "ApiProvider dtor";
}

/*---------------------------------------------------------------------------*/

QJSValue ApiProvider::createElement(
    QJSValue type,
    QJSValue props,
    QJSValue child
) {
    qDebug() << "ApiProvider createElement type"<<type<<" props"<<props;
    QLinkedList<Element*> elems;
    if (child.isArray()) {
        const uint length = child.property("length");
        for (uint i=0;i!=length;i++) {
            elems.append(static_cast<Element*>(child.property(i).toQObject()));
        }
    } else if (child.isQObject()) {
        elems.append(static_cast<Element*>(child));
    } else if (!child.isUndefined()) {
        qCritical() << "ApiProvider createElement invalid child";
        return;
    }
    return engine->newQObject(new Element(type.toString(),elems));
}

/*---------------------------------------------------------------------------*/

void ApiProvider::render(QJSValue root) {
    qDebug() << "ApiProvider render";
    if (root.isQObject()) {
        Element* elem = qobject_cast<Element*>(root.toQObject());
        Element::printTree(elem);
        elem->deleteLater();
    } else {
        qCritical() << "ApiProvider render root is not QObject*";
    }
}

/*---------------------------------------------------------------------------*/

void ApiProvider::initSingletonType(
    QQmlEngine *qmlEngine,
    QJSEngine *jsEngine
) {
    qDebug() << "ApiProvider initSingletonType";
    Q_UNUSED(qmlEngine);
    QJSValue instance=jsEngine->newQObject(new ApiProvider(jsEngine));
    QJSValue restProvider=jsEngine->evaluate(
        "function() {"
        "    const createElement=this.createElement"
        "    this.createElement=function(type){"
        "        const args=Array.apply(null, arguments).slice(1);"
        "        return createElement(type, args);"
        "    }"
        "}",
    );
    restProvider.callWithInstance(instance);
    return instance;
}

/*****************************************************************************/
