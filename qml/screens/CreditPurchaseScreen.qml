import QtQuick
import QtQuick.Controls

Item {
    id: root

    signal creditsPurchased(int credits, int price)

    Rectangle {
        anchors.fill: parent
        color: "#F9FAFB"

        Column {
            anchors.centerIn: parent
            spacing: 24
            width: 600

            Text {
                text: "Purchase Credits"
                font.pixelSize: 36
                font.weight: Font.SemiBold
                color: "#1F2937"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Select a package to continue"
                font.pixelSize: 16
                color: "#6B7280"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Credit options
            Column {
                spacing: 24
                width: parent.width

                // 1 Credit option
                Button {
                    width: parent.width
                    height: 180

                    background: Rectangle {
                        color: parent.pressed ? "#F3F4F6" : (parent.hovered ? "#F9FAFB" : "white")
                        radius: 16
                        border.width: 4
                        border.color: parent.hovered ? "#BFDBFE" : "transparent"
                    }

                    contentItem: Row {
                        spacing: 20
                        padding: 32

                        Column {
                            spacing: 8
                            width: parent.width - 100

                            Row {
                                spacing: 12
                                Text {
                                    text: "$3"
                                    font.pixelSize: 54
                                    font.weight: Font.Bold
                                    color: "#1F2937"
                                }
                                Text {
                                    text: "USD"
                                    font.pixelSize: 22
                                    color: "#6B7280"
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 8
                                }
                            }

                            Text {
                                text: "1 Credit"
                                font.pixelSize: 28
                                font.weight: Font.SemiBold
                                color: "#1F2937"
                            }

                            Text {
                                text: "Single scan"
                                font.pixelSize: 16
                                color: "#6B7280"
                            }
                        }

                        Rectangle {
                            width: 60
                            height: 60
                            radius: 30
                            color: "#2563EB"
                            anchors.verticalCenter: parent.verticalCenter

                            Text {
                                text: "✓"
                                font.pixelSize: 32
                                color: "white"
                                anchors.centerIn: parent
                            }
                        }
                    }

                    onClicked: root.creditsPurchased(1, 3)
                }

                // 4 Credits option (Popular)
                Button {
                    width: parent.width
                    height: 200

                    background: Rectangle {
                        color: parent.pressed ? "#F3F4F6" : (parent.hovered ? "#F9FAFB" : "white")
                        radius: 16
                        border.width: 4
                        border.color: parent.hovered ? "#BFDBFE" : "transparent"

                        // Popular badge
                        Rectangle {
                            width: 180
                            height: 32
                            radius: 16
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: -16
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#F97316" }
                                GradientStop { position: 1.0; color: "#EA580C" }
                            }

                            Text {
                                text: "MOST POPULAR"
                                font.pixelSize: 14
                                font.weight: Font.SemiBold
                                color: "white"
                                anchors.centerIn: parent
                            }
                        }
                    }

                    contentItem: Row {
                        spacing: 20
                        padding: 32

                        Column {
                            spacing: 8
                            width: parent.width - 100

                            Row {
                                spacing: 12
                                Text {
                                    text: "$10"
                                    font.pixelSize: 54
                                    font.weight: Font.Bold
                                    color: "#1F2937"
                                }
                                Text {
                                    text: "USD"
                                    font.pixelSize: 22
                                    color: "#6B7280"
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 8
                                }
                            }

                            Text {
                                text: "4 Credits"
                                font.pixelSize: 28
                                font.weight: Font.SemiBold
                                color: "#1F2937"
                            }

                            Text {
                                text: "Best value!"
                                font.pixelSize: 16
                                color: "#6B7280"
                            }

                            Text {
                                text: "Save $2"
                                font.pixelSize: 14
                                font.weight: Font.SemiBold
                                color: "#16A34A"
                            }
                        }

                        Rectangle {
                            width: 60
                            height: 60
                            radius: 30
                            color: "#2563EB"
                            anchors.verticalCenter: parent.verticalCenter

                            Text {
                                text: "✓"
                                font.pixelSize: 32
                                color: "white"
                                anchors.centerIn: parent
                            }
                        }
                    }

                    onClicked: root.creditsPurchased(4, 10)
                }
            }

            Text {
                text: "Tap a package to purchase"
                font.pixelSize: 14
                color: "#6B7280"
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
