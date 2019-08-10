import QtQuick 2.7
import QtQuick.Controls 1.0
import QtQuick.Window 2.2

import com.tripolskypetr.quitejs 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Button {
        anchors.centerIn: parent
        text: "Click me and check console output"
        onClicked: {
            var root = Api.createElement("parent",
                Api.createElement("child1"),
                Api.createElement("child2",
                    Api.createElement("child2-1"),
                    Api.createElement("child2-2")
                ),
                Api.createElement("child3"),
                Api.createElement("child4")
            );
            Api.render(root);
        }
    }
}
