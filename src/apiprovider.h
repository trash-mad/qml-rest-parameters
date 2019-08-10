#ifndef APIPROVIDER_H
#define APIPROVIDER_H

#include <QtDebug>
#include <QObject>
#include <QJSValue>
#include <QJSEngine>
#include <QQmlEngine>
#include <QLinkedList>

#include <string>

/*****************************************************************************/

class Element : public QObject {
  Q_OBJECT
  private:
    QString type;
    QLinkedList<Element*> child;
  public:
    explicit Element(QString type, QLinkedList<Element*> child);
    virtual Element();
  public:
    QLinkedList<Element*> getChild() const;
    QString getType() const;
  public:
    static void printTree(Element* root);
};

/*****************************************************************************/

class ApiProvider : public QObject {
  Q_OBJECT
  private:
    QJSEngine* engine;
  public:
    explicit ApiProvider(QJSEngine* engine);
    virtual ~ApiProvider();
  public slots:
    QJSValue createElement(QJSValue type, QJSValue child);
    void render(QJSValue root);
  public:
    void initSingletonType(QQmlEngine *engine, QJSEngine *scriptEngine);
};

/*****************************************************************************/

#endif // APIPROVIDER_H
