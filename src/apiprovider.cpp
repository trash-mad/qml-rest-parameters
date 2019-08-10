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
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

/*---------------------------------------------------------------------------*/

Element::~Element() {
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
    QStringList line;
    for (int i=space;i!=0;i--) {
        line.append(" ");
    }
    line.append(root->getType());
    qInfo() << line.join("");
    space+=2;
    QLinkedList<Element*> child=root->getChild();
    QLinkedList<Element*>::iterator iter;
    for (iter=child.begin();iter!=child.end();iter++) {
        printTree(*iter);
    }
    space-=2;
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

QJSValue ApiProvider::createElementInternal(
    QJSValue type,
    QJSValue child
) {
    qDebug() << "ApiProvider createElementInternal type"<<type.toString();
    QLinkedList<Element*> elems;
    if (child.isArray()) {
        const uint length = child.property("length").toUInt();
        for (uint i=0;i!=length;i++) {
            elems.append(static_cast<Element*>(child.property(i).toQObject()));
        }
    } else if (child.isQObject()) {
        elems.append(static_cast<Element*>(child.toQObject()));
    } else if (!child.isUndefined()) {
        qCritical() << "ApiProvider createElement invalid child";
        return QJSValue();
    }
    return engine->newQObject(new Element(type.toString(),elems));
}

/*---------------------------------------------------------------------------*/

void ApiProvider::render(QJSValue root) {
    qDebug() << "ApiProvider render";
    if (root.isQObject()) {
        Element* elem = qobject_cast<Element*>(root.toQObject());
        Element::printTree(elem);
        elem->setParent(this);
    } else {
        qCritical() << "ApiProvider render root is not QObject*";
    }
}

/*---------------------------------------------------------------------------*/

QJSValue ApiProvider::initSingletonType(
    QQmlEngine *qmlEngine,
    QJSEngine *jsEngine
) {
    qDebug() << "ApiProvider initSingletonType";
    Q_UNUSED(qmlEngine);
    jsEngine->installExtensions(QJSEngine::ConsoleExtension);
    QJSValue instance=jsEngine->newQObject(new ApiProvider(jsEngine));
    QJSValue restProvider=jsEngine->evaluate(
        "(function (instance) {" \
        "   instance.createElement=function(type,...child){" \
        "       return instance.createElementInternal(type,child);" \
        "   }; " \
        "   return instance;" \
        "})"
    );
    if (restProvider.isError()) {
        qCritical() << restProvider.toString();
        return QJSValue();
    } else {
        QJSValue result=restProvider.call({instance});
        if (result.isError()) {
            qCritical() << result.toString();
            return QJSValue();
        } else {
            return instance;
        }
    }
}

/*****************************************************************************/
