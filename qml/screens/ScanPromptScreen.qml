import QtQuick
import QtQuick.Controls

Item {
    id: root

    property int totalScans: 1
    property int currentScan: 0
    property bool isScanning: false

    signal scanRequested()

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#2563EB" }
            GradientStop { position: 1.0; color: "#1E40AF" }
        }

        Column {
            anchors.centerIn: parent
            spacing: 24
            width: 500

            Text {
                text: isScanning ? "Scanning..." : (currentScan === 0 ? "Ready to Scan" : "Next Scan Ready")
                font.pixelSize: 36
                font.weight: Font.Bold
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Scanning " + (currentScan + 1) + " of " + totalScans
                font.pixelSize: 28
                font.weight: Font.SemiBold
                color: "#E6FFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Scanner alignment guide
            Rectangle {
                width: parent.width
                height: 240
                radius: 16
                color: "#F2FFFFFF" // rgba(255, 255, 255, 0.95)

                Column {
                    anchors.centerIn: parent
                    spacing: 16
                    width: parent.width - 48

                    Text {
                        text: "Align document to LEFT edge"
                        font.pixelSize: 16
                        font.weight: Font.SemiBold
                        color: "#1F2937"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    // Scanner bed visualization
                    Rectangle {
                        width: parent.width
                        height: 120
                        radius: 8
                        color: "#E5E7EB"
                        border.width: 4
                        border.color: "#9CA3AF"
                        anchors.horizontalCenter: parent.horizontalCenter

                        // Left edge indicator
                        Rectangle {
                            width: 8
                            height: parent.height
                            color: "#DC2626"
                            anchors.left: parent.left
                            radius: 4
                        }

                        // Document representation
                        Rectangle {
                            width: parent.width * 0.6
                            height: parent.height - 16
                            color: "white"
                            border.width: 2
                            border.color: "#2563EB"
                            radius: 4
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            anchors.verticalCenter: parent.verticalCenter

                            Column {
                                anchors.centerIn: parent
                                spacing: 4
                                width: parent.width - 20

                                Repeater {
                                    model: 3
                                    Rectangle {
                                        width: parent.width * (index === 0 ? 1.0 : index === 1 ? 0.85 : 0.65)
                                        height: 8
                                        radius: 4
                                        color: "#D1D5DB"
                                    }
                                }
                            }
                        }

                        // Arrow pointing left
                        Text {
                            text: "←"
                            font.pixelSize: 32
                            font.weight: Font.Bold
                            color: "#DC2626"
                            anchors.left: parent.left
                            anchors.leftMargin: -40
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Text {
                        text: "← Insert document flush with left edge"
                        font.pixelSize: 14
                        font.weight: Font.SemiBold
                        color: "#DC2626"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Text {
                text: "Make sure document is flat and fully inserted"
                font.pixelSize: 14
                color: "#CCFFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Button {
                width: parent.width
                height: 80
                text: isScanning ? "Scanning..." : (currentScan === 0 ? "Start Scanning" : "Scan Next")
                font.pixelSize: 28
                font.weight: Font.Bold
                enabled: !isScanning
                anchors.horizontalCenter: parent.horizontalCenter

                background: Rectangle {
                    color: parent.enabled ? (parent.pressed ? "#E5E7EB" : (parent.hovered ? "#F3F4F6" : "white")) : "#D1D5DB"
                    radius: 16
                }

                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "#2563EB"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: root.scanRequested()
            }

            Text {
                text: (totalScans - currentScan) + (totalScans - currentScan === 1 ? " scan remaining" : " scans remaining")
                font.pixelSize: 14
                color: "#CCFFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        // Scanning animation
        Row {
            anchors.centerIn: parent
            spacing: 8
            visible: isScanning

            Repeater {
                model: 3
                Rectangle {
                    width: 12
                    height: 12
                    radius: 6
                    color: "white"

                    SequentialAnimation on opacity {
                        loops: Animation.Infinite
                        NumberAnimation { from: 0; to: 1; duration: 400 }
                        NumberAnimation { from: 1; to: 0; duration: 400 }
                        PauseAnimation { duration: index * 200 }
                    }
                }
            }
        }
    }
}
