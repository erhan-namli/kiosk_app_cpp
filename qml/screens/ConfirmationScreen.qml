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
            spacing: 30
            width: 600

            // Success icon
            Text {
                text: "✓"
                font.pixelSize: 120
                font.weight: Font.Bold
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "All Done!"
                font.pixelSize: 48
                font.weight: Font.Bold
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Your scans have been sent via SMS"
                font.pixelSize: 24
                color: "#F2FFFFFF" // rgba(255, 255, 255, 0.95)
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }

            // Phone number display
            Rectangle {
                width: 400
                height: 80
                radius: 12
                color: "#33FFFFFF" // rgba(255, 255, 255, 0.2)
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: formatPhone(phoneNumber)
                    font.pixelSize: 32
                    font.weight: Font.SemiBold
                    color: "white"
                    anchors.centerIn: parent
                }
            }

            Text {
                text: "Check your phone for a text message\nwith your scanned photos"
                font.pixelSize: 18
                color: "#E6FFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }

            Button {
                width: 400
                height: 80
                text: "Done"
                font.pixelSize: 32
                font.weight: Font.Bold
                anchors.horizontalCenter: parent.horizontalCenter

                background: Rectangle {
                    color: parent.pressed ? "#E5E7EB" : (parent.hovered ? "#F3F4F6" : "white")
                    radius: 16
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
