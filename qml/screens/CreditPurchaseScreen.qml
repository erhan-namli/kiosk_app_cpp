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
            spacing: Math.max(12, root.height * 0.025)
            width: Math.min(root.width * 0.9, 600)

            Text {
                text: "Purchase Credits"
                font.pixelSize: Math.min(root.width * 0.045, 32)
                font.weight: Font.SemiBold
                color: "#1F2937"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Select a package to continue"
                font.pixelSize: Math.min(root.width * 0.02, 14)
                color: "#6B7280"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Credit options
            Column {
                spacing: Math.max(12, root.height * 0.025)
                width: parent.width

                // 1 Credit option
                Button {
                    width: parent.width
                    height: Math.min(root.height * 0.3, 140)

                    background: Rectangle {
                        color: parent.pressed ? "#F3F4F6" : (parent.hovered ? "#F9FAFB" : "white")
                        radius: 12
                        border.width: 3
                        border.color: parent.hovered ? "#BFDBFE" : "transparent"
                    }

                    contentItem: Row {
                        spacing: Math.max(12, root.width * 0.02)
                        padding: Math.max(16, root.width * 0.03)

                        Column {
                            spacing: 4
                            width: parent.width - Math.min(root.width * 0.1, 80)

                            Row {
                                spacing: 8
                                Text {
                                    text: "$3"
                                    font.pixelSize: Math.min(root.width * 0.06, 42)
                                    font.weight: Font.Bold
                                    color: "#1F2937"
                                }
                                Text {
                                    text: "USD"
                                    font.pixelSize: Math.min(root.width * 0.025, 18)
                                    color: "#6B7280"
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 4
                                }
                            }

                            Text {
                                text: "1 Credit"
                                font.pixelSize: Math.min(root.width * 0.032, 22)
                                font.weight: Font.SemiBold
                                color: "#1F2937"
                            }

                            Text {
                                text: "Single scan"
                                font.pixelSize: Math.min(root.width * 0.018, 14)
                                color: "#6B7280"
                            }
                        }

                        Rectangle {
                            width: Math.min(root.width * 0.07, 50)
                            height: Math.min(root.width * 0.07, 50)
                            radius: width / 2
                            color: "#2563EB"
                            anchors.verticalCenter: parent.verticalCenter

                            Text {
                                text: "✓"
                                font.pixelSize: Math.min(root.width * 0.035, 24)
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
                    height: Math.min(root.height * 0.35, 160)

                    background: Rectangle {
                        color: parent.pressed ? "#F3F4F6" : (parent.hovered ? "#F9FAFB" : "white")
                        radius: 12
                        border.width: 3
                        border.color: parent.hovered ? "#BFDBFE" : "transparent"

                        // Popular badge
                        Rectangle {
                            width: Math.min(root.width * 0.25, 160)
                            height: Math.min(root.height * 0.06, 28)
                            radius: height / 2
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: -height / 2
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#F97316" }
                                GradientStop { position: 1.0; color: "#EA580C" }
                            }

                            Text {
                                text: "MOST POPULAR"
                                font.pixelSize: Math.min(root.width * 0.016, 12)
                                font.weight: Font.SemiBold
                                color: "white"
                                anchors.centerIn: parent
                            }
                        }
                    }

                    contentItem: Row {
                        spacing: Math.max(12, root.width * 0.02)
                        padding: Math.max(16, root.width * 0.03)

                        Column {
                            spacing: 4
                            width: parent.width - Math.min(root.width * 0.1, 80)

                            Row {
                                spacing: 8
                                Text {
                                    text: "$10"
                                    font.pixelSize: Math.min(root.width * 0.06, 42)
                                    font.weight: Font.Bold
                                    color: "#1F2937"
                                }
                                Text {
                                    text: "USD"
                                    font.pixelSize: Math.min(root.width * 0.025, 18)
                                    color: "#6B7280"
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 4
                                }
                            }

                            Text {
                                text: "4 Credits"
                                font.pixelSize: Math.min(root.width * 0.032, 22)
                                font.weight: Font.SemiBold
                                color: "#1F2937"
                            }

                            Text {
                                text: "Best value!"
                                font.pixelSize: Math.min(root.width * 0.018, 14)
                                color: "#6B7280"
                            }

                            Text {
                                text: "Save $2"
                                font.pixelSize: Math.min(root.width * 0.016, 12)
                                font.weight: Font.SemiBold
                                color: "#16A34A"
                            }
                        }

                        Rectangle {
                            width: Math.min(root.width * 0.07, 50)
                            height: Math.min(root.width * 0.07, 50)
                            radius: width / 2
                            color: "#2563EB"
                            anchors.verticalCenter: parent.verticalCenter

                            Text {
                                text: "✓"
                                font.pixelSize: Math.min(root.width * 0.035, 24)
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
                font.pixelSize: Math.min(root.width * 0.018, 12)
                color: "#6B7280"
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
