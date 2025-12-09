import QtQuick
import QtQuick.Controls

Item {
    id: root

    signal continueClicked()

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#2563EB" }
            GradientStop { position: 1.0; color: "#1E40AF" }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.continueClicked()
        }

        Column {
            anchors.centerIn: parent
            spacing: Math.max(40, root.height * 0.1)

            Text {
                text: "SCAN.RN"
                font.pixelSize: Math.min(root.width * 0.09, 64)
                font.weight: Font.Bold
                font.letterSpacing: Math.min(root.width * 0.01, 8)
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Tap to begin scanning"
                font.pixelSize: Math.min(root.width * 0.032, 24)
                font.weight: Font.Medium
                color: "#E6FFFFFF" // rgba(255, 255, 255, 0.9)
                anchors.horizontalCenter: parent.horizontalCenter

                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.3; duration: 1000; easing.type: Easing.InOutQuad }
                    NumberAnimation { from: 0.3; to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
                }
            }
        }
    }
}
