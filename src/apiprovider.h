#ifndef APIPROVIDER_H
#define APIPROVIDER_H

#include <QtDebug>
#include <QString>
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
    virtual ~Element();
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
    QJSValue createElementInternal(QJSValue type, QJSValue child);
    void render(QJSValue root);
    void test(QJSValue array){qDebug() << array.toString();}
  public:
    static QJSValue initSingletonType(QQmlEngine *qml, QJSEngine *js);
};

/*****************************************************************************/

#endif // APIPROVIDER_H
