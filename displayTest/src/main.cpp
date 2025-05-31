#include <HardwareSerial.h>
#include "graph_ui.h"
#include "display.h"
#include "touch.h"

display  screen;
GraphUI  gui(screen);

constexpr int UART1_TX = 17;      // P3‑1
constexpr int UART1_RX = 18;      // P3‑2  (change if 18 is busy)

HardwareSerial UART1(1);          // choose hardware UART1

uint32_t lastTx = 0;

String   inBuf;                       // accumulates chars until '\n'
constexpr size_t MAX_LINE = 64;       // overflow guard

void processLine(const String& line)            // interpret one command
{
    if (line == "POOP") {
        gui.showAlert("Too many effects!", 2500);   // 2.5 s on‑screen
    }
}

void pollSerial()                                  // non‑blocking RX
{
    while (Serial2.available()) {
        char c = Serial2.read();

        if (c == '\n') {                 // end‑of‑line
            inBuf.trim();                // drop stray CR / spaces
            if (inBuf.length()) processLine(inBuf);
            inBuf = "";
        }
        else if (c != '\r') {            // ignore CR in CR‑LF
            if (inBuf.length() < MAX_LINE)
                inBuf += c;              // store printable char
        }
    }
}

void setup()
{
    GraphUI::SimplePalette p;
    p.main       = 0xF81F;
    p.background = 0xFFFF;
    p.accent1    = 0x07E0;
    p.accent2    = 0x0410;
    p.accent3    = 0xF800;
    gui.setPalette(p);          // <- set before init()
    // --- peripherals -------------------------------------------------
    screen.start();
    touch_init();                 // keeps I²C on GPIO19/20
    gui.init();

    // --- set pins *before* begin() -----------------------------------
    //UART1.setPins(UART1_TX, UART1_RX);   // RTS,CTS default to ‑1 (unused)
    Serial2.begin(115200, SERIAL_8N1, UART1_RX, UART1_TX);     // 115 k 8‑N‑1

    Serial2.println("[UART1] ready");
}

void loop()
{

    pollSerial();
    gui.tick();
    gui.draw();

    //if (millis() - lastTx >= 500) {
    //    gui.sendGraphCSV(Serial2);         // stream over P4 heade
    //    lastTx = millis();
    //}
}
