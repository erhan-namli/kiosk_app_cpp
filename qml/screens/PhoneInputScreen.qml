import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    signal phoneSubmitted(string phone)

    property string phoneNumber: ""

    function formatPhone(phone) {
        if (phone.length <= 3) return phone
        if (phone.length <= 6) return `(${phone.slice(0, 3)}) ${phone.slice(3)}`
        return `(${phone.slice(0, 3)}) ${phone.slice(3, 6)}-${phone.slice(6)}`
    }

    Rectangle {
        anchors.fill: parent
        color: "#F9FAFB"

        Column {
            anchors.centerIn: parent
            spacing: Math.max(8, parent.height * 0.02)
            width: Math.min(parent.width * 0.85, 500)

            Text {
                text: "Enter Your Phone Number"
                font.pixelSize: Math.min(parent.width * 0.05, 32)
                font.weight: Font.SemiBold
                color: "#1F2937"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Phone display
            Rectangle {
                width: parent.width
                height: Math.max(60, parent.height * 0.15)
                color: "white"
                radius: 12

                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        text: "Phone Number"
                        font.pixelSize: Math.min(root.width * 0.018, 14)
                        color: "#6B7280"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: phoneNumber.length === 0 ? "(___) ___-____" : formatPhone(phoneNumber)
                        font.pixelSize: Math.min(root.width * 0.04, 28)
                        font.weight: Font.Medium
                        color: phoneNumber.length === 0 ? "#D1D5DB" : "#1F2937"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            // Number pad
            Grid {
                columns: 3
                spacing: Math.max(8, root.width * 0.015)
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: ["1", "2", "3", "4", "5", "6", "7", "8", "9"]
                    delegate: Button {
                        width: Math.min((root.width * 0.25), 110)
                        height: Math.min((root.height * 0.12), 60)
                        text: modelData
                        font.pixelSize: Math.min(root.width * 0.035, 28)
                        font.weight: Font.Medium

                        background: Rectangle {
                            color: parent.pressed ? "#E5E7EB" : (parent.hovered ? "#F3F4F6" : "white")
                            radius: 12
                        }

                        onClicked: {
                            if (phoneNumber.length < 10) {
                                phoneNumber += modelData
                            }
                        }
                    }
                }
            }

            // Zero button
            Button {
                width: parent.width
                height: Math.min((root.height * 0.12), 60)
                text: "0"
                font.pixelSize: Math.min(root.width * 0.035, 28)
                font.weight: Font.Medium
                anchors.horizontalCenter: parent.horizontalCenter

                background: Rectangle {
                    color: parent.pressed ? "#E5E7EB" : (parent.hovered ? "#F3F4F6" : "white")
                    radius: 12
                }

                onClicked: {
                    if (phoneNumber.length < 10) {
                        phoneNumber += "0"
                    }
                }
            }

            // Action buttons
            Row {
                spacing: Math.max(8, root.width * 0.015)
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    width: Math.min((root.width * 0.4), 200)
                    height: Math.min((root.height * 0.12), 60)
                    text: "Delete"
                    font.pixelSize: Math.min(root.width * 0.025, 18)

                    background: Rectangle {
                        color: parent.pressed ? "#B91C1C" : (parent.hovered ? "#DC2626" : "#EF4444")
                        radius: 12
                    }

                    contentItem: Text {
                        text: parent.text
                        font: parent.font
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        if (phoneNumber.length > 0) {
                            phoneNumber = phoneNumber.slice(0, -1)
                        }
                    }
                }

                Button {
                    width: Math.min((root.width * 0.4), 200)
                    height: Math.min((root.height * 0.12), 60)
                    text: "Continue"
                    font.pixelSize: Math.min(root.width * 0.028, 20)
                    font.weight: Font.SemiBold
                    enabled: phoneNumber.length === 10

                    background: Rectangle {
                        color: parent.enabled ? (parent.pressed ? "#1E40AF" : (parent.hovered ? "#1D4ED8" : "#2563EB")) : "#D1D5DB"
                        radius: 12
                    }

                    contentItem: Text {
                        text: parent.text
                        font: parent.font
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        if (phoneNumber.length === 10) {
                            root.phoneSubmitted(phoneNumber)
                        }
                    }
                }
            }
        }
    }
}
