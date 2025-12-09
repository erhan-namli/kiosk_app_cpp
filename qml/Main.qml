import QtQuick
import QtQuick.Controls
import QtQuick.Window
import "screens"

Window {
    id: root
    width: 1280
    height: 720
    visible: true
    title: "SCAN.RN - Photo Scanner Kiosk"
    color: "#F9FAFB"

    property int inactivityTimeout: 60000 // 60 seconds

    Timer {
        id: inactivityTimer
        interval: root.inactivityTimeout
        running: stackView.currentItem !== logoScreen
        repeat: false
        onTriggered: {
            console.log("Inactivity timeout - resetting to logo")
            appController.resetToIdle()
        }
    }

    // Reset inactivity timer on user interaction
    MouseArea {
        anchors.fill: parent
        z: -1
        onPressed: (mouse) => {
            inactivityTimer.restart()
            mouse.accepted = false
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: logoScreen

        // Smooth transitions between screens
        pushEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 300
            }
        }
        pushExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 300
            }
        }
    }

    // Screen components
    Component {
        id: logoScreen
        LogoScreen {
            onContinueClicked: {
                appController.startNewSession()
                stackView.push(phoneInputScreen)
            }
        }
    }

    Component {
        id: phoneInputScreen
        PhoneInputScreen {
            onPhoneSubmitted: (phone) => {
                appController.setPhoneNumber(phone)
                stackView.push(creditPurchaseScreen)
            }
        }
    }

    Component {
        id: creditPurchaseScreen
        CreditPurchaseScreen {
            onCreditsPurchased: (credits, price) => {
                appController.setPurchaseInfo(credits, price)
                stackView.push(tapToPayScreen)
            }
        }
    }

    Component {
        id: tapToPayScreen
        TapToPayScreen {
            credits: appController.credits
            price: appController.price
            onPaymentCompleted: {
                appController.startScanning()
                stackView.push(scanPromptScreen)
            }
        }
    }

    Component {
        id: scanPromptScreen
        ScanPromptScreen {
            totalScans: appController.credits
            currentScan: appController.currentScan
            onScanRequested: {
                appController.executeScan()
            }
        }
    }

    Component {
        id: confirmationScreen
        ConfirmationScreen {
            phoneNumber: appController.phoneNumber
            onResetRequested: {
                appController.resetToIdle()
                stackView.clear()
                stackView.push(logoScreen)
            }
        }
    }

    // Listen to controller signals to change screens
    Connections {
        target: appController

        function onScanningCompleted() {
            stackView.push(confirmationScreen)
        }
    }
}
