import QtQuick
import QtQuick.Controls

Item {
    id: root

    property string phoneNumber: ""

    signal resetRequested()

    function formatPhone(phone) {
        if (phone.length === 10) {
            return `(${phone.slice(0, 3)}) ${phone.slice(3, 6)}-${phone.slice(6)}`
        }
        return phone
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#10B981" }
            GradientStop { position: 1.0; color: "#059669" }
        }

        Column {
            anchors.centerIn: parent
            spacing: Math.max(15, root.height * 0.03)
            width: Math.min(root.width * 0.85, 600)

            // Success icon
            Text {
                text: "✓"
                font.pixelSize: Math.min(root.width * 0.15, 90)
                font.weight: Font.Bold
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "All Done!"
                font.pixelSize: Math.min(root.width * 0.06, 38)
                font.weight: Font.Bold
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Your scans have been sent via SMS"
                font.pixelSize: Math.min(root.width * 0.028, 20)
                color: "#F2FFFFFF" // rgba(255, 255, 255, 0.95)
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }

            // Phone number display
            Rectangle {
                width: Math.min(root.width * 0.6, 350)
                height: Math.min(root.height * 0.13, 70)
                radius: 12
                color: "#33FFFFFF" // rgba(255, 255, 255, 0.2)
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: formatPhone(phoneNumber)
                    font.pixelSize: Math.min(root.width * 0.04, 28)
                    font.weight: Font.SemiBold
                    color: "white"
                    anchors.centerIn: parent
                }
            }

            Text {
                text: "Check your phone for a text message\nwith your scanned photos"
                font.pixelSize: Math.min(root.width * 0.02, 16)
                color: "#E6FFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }

            Button {
                width: Math.min(root.width * 0.6, 350)
                height: Math.min(root.height * 0.13, 70)
                text: "Done"
                font.pixelSize: Math.min(root.width * 0.04, 28)
                font.weight: Font.Bold
                anchors.horizontalCenter: parent.horizontalCenter

                background: Rectangle {
                    color: parent.pressed ? "#E5E7EB" : (parent.hovered ? "#F3F4F6" : "white")
                    radius: 12
                }

                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "#10B981"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: root.resetRequested()
            }
        }
    }
}
