import QtQuick
import QtQuick.Controls

Item {
    id: root

    property int credits: 1
    property int price: 3

    signal paymentCompleted()

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#9333EA" }
            GradientStop { position: 1.0; color: "#7E22CE" }
        }

        Column {
            anchors.centerIn: parent
            spacing: 30
            width: 500

            // Icon
            Text {
                text: "💳"
                font.pixelSize: 96
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Tap to Pay"
                font.pixelSize: 48
                font.weight: Font.Bold
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Payment info card
            Rectangle {
                width: parent.width
                height: 200
                radius: 16
                color: "#FFFFFF1A" // rgba(255, 255, 255, 0.1)

                Column {
                    anchors.centerIn: parent
                    spacing: 16

                    Text {
                        text: "Payment Amount"
                        font.pixelSize: 18
                        color: "#CCFFFFFF" // rgba(255, 255, 255, 0.8)
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: "$" + price + ".00"
                        font.pixelSize: 64
                        font.weight: Font.Bold
                        color: "white"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: credits + (credits === 1 ? " Credit" : " Credits")
                        font.pixelSize: 18
                        color: "#CCFFFFFF"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Text {
                text: "Hold your card or phone near the reader"
                font.pixelSize: 22
                color: "#E6FFFFFF" // rgba(255, 255, 255, 0.9)
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                width: parent.width
            }

            // Tap button (for demo)
            Button {
                width: parent.width
                height: 100
                text: "Tap Here to Pay"
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
                    color: "#9333EA"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    // Simulate payment processing
                    paymentProcessing.start()
                }
            }

            Text {
                text: "Payment is secure and encrypted"
                font.pixelSize: 14
                color: "#CCFFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Timer {
        id: paymentProcessing
        interval: 1500
        onTriggered: root.paymentCompleted()
    }
}
