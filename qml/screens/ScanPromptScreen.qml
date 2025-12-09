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
            spacing: Math.max(12, root.height * 0.025)
            width: Math.min(root.width * 0.85, 500)

            Text {
                text: isScanning ? "Scanning..." : (currentScan === 0 ? "Ready to Scan" : "Next Scan Ready")
                font.pixelSize: Math.min(root.width * 0.045, 32)
                font.weight: Font.Bold
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Scanning " + (currentScan + 1) + " of " + totalScans
                font.pixelSize: Math.min(root.width * 0.032, 24)
                font.weight: Font.SemiBold
                color: "#E6FFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Scanner alignment guide
            Rectangle {
                width: parent.width
                height: Math.min(root.height * 0.4, 200)
                radius: 12
                color: "#F2FFFFFF" // rgba(255, 255, 255, 0.95)

                Column {
                    anchors.centerIn: parent
                    spacing: Math.max(8, root.height * 0.02)
                    width: parent.width - Math.max(24, root.width * 0.05)

                    Text {
                        text: "Align document to LEFT edge"
                        font.pixelSize: Math.min(root.width * 0.02, 14)
                        font.weight: Font.SemiBold
                        color: "#1F2937"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    // Scanner bed visualization
                    Rectangle {
                        width: parent.width
                        height: Math.min(root.height * 0.2, 100)
                        radius: 8
                        color: "#E5E7EB"
                        border.width: 3
                        border.color: "#9CA3AF"
                        anchors.horizontalCenter: parent.horizontalCenter

                        // Left edge indicator
                        Rectangle {
                            width: 6
                            height: parent.height
                            color: "#DC2626"
                            anchors.left: parent.left
                            radius: 3
                        }

                        // Document representation
                        Rectangle {
                            width: parent.width * 0.6
                            height: parent.height - 12
                            color: "white"
                            border.width: 2
                            border.color: "#2563EB"
                            radius: 4
                            anchors.left: parent.left
                            anchors.leftMargin: 6
                            anchors.verticalCenter: parent.verticalCenter

                            Column {
                                anchors.centerIn: parent
                                spacing: 3
                                width: parent.width - 12

                                Repeater {
                                    model: 3
                                    Rectangle {
                                        width: parent.width * (index === 0 ? 1.0 : index === 1 ? 0.85 : 0.65)
                                        height: 6
                                        radius: 3
                                        color: "#D1D5DB"
                                    }
                                }
                            }
                        }

                        // Arrow pointing left
                        Text {
                            text: "←"
                            font.pixelSize: Math.min(root.width * 0.04, 28)
                            font.weight: Font.Bold
                            color: "#DC2626"
                            anchors.left: parent.left
                            anchors.leftMargin: -Math.min(root.width * 0.05, 35)
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Text {
                        text: "← Insert document flush with left edge"
                        font.pixelSize: Math.min(root.width * 0.016, 12)
                        font.weight: Font.SemiBold
                        color: "#DC2626"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Text {
                text: "Make sure document is flat and fully inserted"
                font.pixelSize: Math.min(root.width * 0.016, 12)
                color: "#CCFFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Button {
                width: parent.width
                height: Math.min(root.height * 0.13, 70)
                text: isScanning ? "Scanning..." : (currentScan === 0 ? "Start Scanning" : "Scan Next")
                font.pixelSize: Math.min(root.width * 0.032, 24)
                font.weight: Font.Bold
                enabled: !isScanning
                anchors.horizontalCenter: parent.horizontalCenter

                background: Rectangle {
                    color: parent.enabled ? (parent.pressed ? "#E5E7EB" : (parent.hovered ? "#F3F4F6" : "white")) : "#D1D5DB"
                    radius: 12
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
                font.pixelSize: Math.min(root.width * 0.016, 12)
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
