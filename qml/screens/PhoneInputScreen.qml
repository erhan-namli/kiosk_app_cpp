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
            spacing: 20
            width: 500

            Text {
                text: "Enter Your Phone Number"
                font.pixelSize: 36
                font.weight: Font.SemiBold
                color: "#1F2937"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Phone display
            Rectangle {
                width: parent.width
                height: 120
                color: "white"
                radius: 16

                Column {
                    anchors.centerIn: parent
                    spacing: 8

                    Text {
                        text: "Phone Number"
                        font.pixelSize: 14
                        color: "#6B7280"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: phoneNumber.length === 0 ? "(___) ___-____" : formatPhone(phoneNumber)
                        font.pixelSize: 36
                        font.weight: Font.Medium
                        color: phoneNumber.length === 0 ? "#D1D5DB" : "#1F2937"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            // Number pad
            Grid {
                columns: 3
                spacing: 16
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: ["1", "2", "3", "4", "5", "6", "7", "8", "9"]
                    delegate: Button {
                        width: 150
                        height: 80
                        text: modelData
                        font.pixelSize: 36
                        font.weight: Font.Medium

                        background: Rectangle {
                            color: parent.pressed ? "#E5E7EB" : (parent.hovered ? "#F3F4F6" : "white")
                            radius: 16
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
                height: 80
                text: "0"
                font.pixelSize: 36
                font.weight: Font.Medium
                anchors.horizontalCenter: parent.horizontalCenter

                background: Rectangle {
                    color: parent.pressed ? "#E5E7EB" : (parent.hovered ? "#F3F4F6" : "white")
                    radius: 16
                }

                onClicked: {
                    if (phoneNumber.length < 10) {
                        phoneNumber += "0"
                    }
                }
            }

            // Action buttons
            Row {
                spacing: 16
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    width: 242
                    height: 80
                    text: "Delete"
                    font.pixelSize: 20

                    background: Rectangle {
                        color: parent.pressed ? "#B91C1C" : (parent.hovered ? "#DC2626" : "#EF4444")
                        radius: 16
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
                    width: 242
                    height: 80
                    text: "Continue"
                    font.pixelSize: 24
                    font.weight: Font.SemiBold
                    enabled: phoneNumber.length === 10

                    background: Rectangle {
                        color: parent.enabled ? (parent.pressed ? "#1E40AF" : (parent.hovered ? "#1D4ED8" : "#2563EB")) : "#D1D5DB"
                        radius: 16
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
