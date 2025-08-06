// graph_ui.h — Touch‑driven DAG editor (no external colour header)
// -----------------------------------------------------------------------------
// This version removes the dependency on **colourDefines.h**.  It keeps a small
// baked‑in colour palette and exposes setPalette() if you want to override
// the defaults at runtime.
//
// It still expects:
//   • display.h  – your drawing wrapper  ➜ class display
//   • touch.h    – global getPoints() and touchArray[]
//   • point.h    – simple x/y struct
// Nothing else is required.
// -----------------------------------------------------------------------------
#pragma once

#include <Arduino.h>
#include <vector>
#include <array>
#include <cstdint>
#include <algorithm>
#include "display.h"
#include "touch.h"
#include "point.h"

class GraphUI {
public:
    /* show a one‑shot status line (ms = how long to keep it visible) */
    void showAlert(const String& msg, uint32_t ms = 2000);

    void sendGraphNow();
    void resetGraph();

    // ── graph‑checks ─────────────────────────────────────────────
    bool  isValidConfig() const;
    void  drawConfigStatus();

    static constexpr int8_t  maxSliders = 3;      // was 5

    uint16_t paramBlock = 0;       // default selected (input block)
    int paramIndex = -1;
    String editBuf = "";


    void sendGraphCSV(Stream& out) const
    {
        auto csv = exportGraph();          // build the vector of CSV lines
        for (const auto& line : csv)       // each String already ends with a comma
            out.println(line);             // prints "N…;E…;P…;I…" + \r\n
        out.println();                     // blank line acts as a frame delimiter
    }

    std::vector<String> exportGraph() const;
    void drawGraphText();

    // ── node types ────────────────────────────────────────────────────────
    enum class NodeType : uint8_t { Reverb, Cabinet, SuproAmp, Phasor, Chorus, Delay, Input, Output, Count };
   
    static constexpr int paletteBtnCount = static_cast<int>(NodeType::Input);

    struct ParamMeta {
        const char* title;   // on‑screen label
        float       min;     // engineering range
        float       max;
        bool        isInt;   // true → transmit as integer
    };
    
    // ── per‑block parameter tables (6 max per block) ──────────────────────
    static ParamMeta paramMeta(NodeType nt, int idx)
    {
        // prettier with C++17 inline arrays
        using PM = ParamMeta;
        switch (nt) {
            case NodeType::Chorus: {
                constexpr PM table[] = {
                    { "Wetness",     0,   254, true  },
                    { "Depth",    0,   254, true },
                    { "Rate",  0,   254, true },
                    { "", 0, 1, false },
                    { "", 0, 1, false },
                    { "", 0, 1, false },
                };
                return table[idx];
            }
            case NodeType::Delay: {
                constexpr PM table[] = {
                    { "Wetness",     0,   254, true  },
                    { "", 0, 1, false },
                    { "", 0, 1, false },
                    { "", 0, 1, false },
                    { "", 0, 1, false },
                    { "", 0, 1, false },
                };
                return table[idx];
            }
            /* --- other blocks (shorter tables pad with dummies) --------- */
            case NodeType::SuproAmp: {
                constexpr PM table[] = {
                    { "", 0, 1, false },
                    { "", 0, 1, false }, { "",0,1,false },
                    { "",0,1,false },    { "",0,1,false }, { "",0,1,false }
                };
                return table[idx];
            }
            case NodeType::Phasor: {
                constexpr PM table[] = {
                    { "Wetness",     0,   254, true  },   // Byte‑depth (integer 0‑127)
                    { "Depth",    0,   254, true },   // Wet mix %
                    { "Rate",  0,   254, true },   // Mod depth %
                    { "", 0, 1, false },   // Hz
                    { "", 0, 1, false  },   // # all‑pass stages
                    { "", 0, 1, false },   // Output mix %
                };
                return table[idx];
            }
            
           /* case NodeType::Mixer: {
                constexpr PM table[] = {
                    { "Ratio",  0,   254, true },   // Input‑to‑output %
                    { "", 0,1,false }, { "",0,1,false },   // unused slots
                    { "",0,1,false },  { "",0,1,false }, { "",0,1,false }
                };
                return table[idx];
            }*/

            case NodeType::Cabinet: {
                constexpr PM table[] = {
                    { "IR#", 0,  254,  true },   // integer: choose Impulse‑Response 0‑32
                    { "", 0,1,false }, { "",0,1,false },
                    { "",0,1,false }, { "",0,1,false }, { "",0,1,false }
                };
                return table[idx];
            }
            /* … Phasor, Reverb, Mixer – same pattern … */
            default: return { "", 0, 1, false };
        }
    }


    // ── public data structures ───────────────────────────────────────────-
    struct Block {
        uint16_t              id;          // unique (>0 & <0xFFFF); Input=0, Output=0xFFFF
        NodeType              type;
        int16_t               x, y;        // *top‑left* corner (keep integer)
        uint16_t              w, h;        // dimensions (80×40)
        std::vector<uint16_t> outs;        // adjacency list (dst block IDs)
        std::vector<String> params;
    };

    static std::vector<String> defaultParams(NodeType nt)
    {
        switch (nt) {
            case NodeType::SuproAmp:    return { "50" };                                                 // Cab#
            case NodeType::Chorus: return { "50", "50", "50" };              // Wetness, Depth, Rate
            case NodeType::Phasor: return { "50", "50", "50" };                       // Wetness, Depth, Rate …
            case NodeType::Delay:  return { "50" };                // Wetness
            case NodeType::Reverb: return {};                                                      // none
          // case NodeType::Mixer:  return { "50" };      
            case NodeType::Cabinet:    return { "0" };                                          // Ratio %
            default:               return {};
        }
    }

    // ── ctor ──────────────────────────────────────────────────────────────
    explicit GraphUI(display& disp) : d(disp) {}

    // ── optional palette setter ───────────────────────────────────────────
    struct SimplePalette {
        uint16_t main      = 0x07E0; // green
        uint16_t background= 0x0000; // black
        uint16_t accent1   = 0x001F; // blue
        uint16_t accent2   = 0xFFE0; // yellow
        uint16_t accent3   = 0xF800; // red
    };
    void setPalette(const SimplePalette& p) { pal = p; }

    // ── lifecycle helpers ─────────────────────────────────────────────────
    void init() {
        paletteH = 40;          // button height (same as original UI)
        blkW     = 80;
        blkH     = 40;

        // create fixed Input and Output blocks
        int16_t yRow = paletteH + 10;
        Block in  {0x0001,      NodeType::Input,    10,                         yRow, blkW, blkH, {}};
        Block out {0xFFFF, NodeType::Output,  static_cast<int16_t>(d.screenSize.x - blkW - 10), yRow, blkW, blkH, {}};
        nextId = 2;
        blocks.push_back(in);
        blocks.push_back(out);
    }

    // call every frame -----------------------------------------------------
    void tick() {
        bool touching = getPoints();

        // ---- finger just went down ------------------------------------------
        if (touching && !touchPrev) {
            startX = touchArray[0].x;
            startY = touchArray[0].y;
            
            /* if the tap landed inside the slider window, handle it and stop */
            if (inParamWin(startX, startY)) {
                paramWinTouchDown(startX, startY);
                needRedraw = true;
                touchPrev = touching;
                return;                         // swallow event
            }
            
            // palette tap? -------------------------------------------------
            if (startY < paletteH) {                 // finger landed in palette row
                int btnW = d.screenSize.x / paletteBtnCount;
                int idx  = startX / btnW;
                if (idx >= 0 && idx < paletteBtnCount)
                    spawnType = static_cast<NodeType>(idx);
                dragFromId = 0;                      // cancel any drag‑to‑connect
            } else {
                dragFromId = hitBlock(startX, startY);

                if (dragFromId) {                       // tap landed on a node → select it
                    selected = dragFromId;
                    needRedraw = true; }
                else if (!inDeleteBtn(startX, startY))// tap blank canvas → clear selection
                    selected = 0;                     // (but NOT when tap is on the DEL button)
            }
        }


        // ---- finger moved ------------------------------------------------
        if (touching) {
            curX = touchArray[0].x;
            curY = touchArray[0].y;
        }

        // continuous slider drag
        if (touching && paramBlock == selected &&
            inParamWin(curX, curY)) {
            paramWinTouchDown(curX, curY);    // reuse the same helper
            needRedraw = true;
            touchPrev = touching;             // keep state in sync
            return;                           // swallow the event
        }

        // ---- finger lifted ----------------------------------------------
        if (!touching && touchPrev) {
            
            /* Gesture started in parameter window? → do nothing else. */
            if (inParamWin(startX, startY)) {
                dragFromId = 0;
                touchPrev  = touching;
                return;                       // ← prevents block spawn
            }
            
            // 0.  Was it the delete button?   (do this FIRST)
            if (inDeleteBtn(startX, startY)) {
                if (deletable(selected)) deleteBlock(selected);
                needRedraw = true;
                dragFromId = 0;
                touchPrev  = touching;
                return;                     // ←‑‑ skip connect / spawn logic
            }

            // update button? ---------------------------------------------
            if (inUpdateBtn(startX, startY)) {
                if (isValidConfig()) {          // ← only transmit when it’s legal
                    sendGraphNow();
                } else {
                    needRedraw = true;          // force a repaint so the red
                                                // “Invalid Config” banner is visible
                    /* optional: add a short buzzer‑beep or LED flash here */
                }
                touchPrev = touching;
                return;                         // swallow tap either way
            }


            // reset button? ------------------------------------------------
            if (inResetBtn(startX, startY)) {
                resetGraph();            // helper we define next
                touchPrev = touching;
                return;                  // swallow the event
            }

        
            // 1.  Normal connect logic -----------------------------------
            uint16_t target = hitBlock(curX, curY);
            if (dragFromId) {
                if (target && target != dragFromId) {
                    connectBlocks(dragFromId, target);
                    needRedraw = true; }
            }
            // 2.  Spawn a new node on blank canvas ------------------------
            else if (!target && startY >= paletteH) {
                addBlock(spawnType,
                         static_cast<int16_t>(startX - blkW / 2),
                         static_cast<int16_t>(startY - blkH / 2));
                needRedraw = true;
            }
        
            dragFromId = 0;
        }

    // inside tick(), after handling selection
    if (selected != paramBlock) {
        paramBlock = selected;
        paramIndex = -1;
        editBuf = "";
    }

        touchPrev = touching;
    }
    void drawDeleteButton()
    {
        point centre{ static_cast<int16_t>(d.screenSize.x - delW/2 - delMargin),
                      static_cast<int16_t>(d.screenSize.y - delH/2 - delMargin) };

        uint16_t fill = (touchPrev && inDeleteBtn(curX, curY)) ? pal.accent3 : pal.main;
        d.drawBorderedRoundedRectangle(centre, delW, delH, 6, 2, fill, 0x0000);
        d.printText(centre, 2, "DEL", pal.background);
    }

    // ── reset button ─────────────────────────────────────────────────
    void drawResetBtn()
    {
        /* centre point ------------------------------------------------ */
        point centre{
            static_cast<float>(d.screenSize.x - delMargin
                                            - resetW / 2),
            static_cast<float>(d.screenSize.y - delMargin - delH
                                            - resetMargin - resetH / 2)
        };

        /* press‑state highlight --------------------------------------- */
        uint16_t fill = (touchPrev && inResetBtn(curX, curY))
                        ? pal.accent3        // same “pressed” red as DEL
                        : pal.main;          // normal idle colour

        d.drawBorderedRoundedRectangle(centre,
                                    resetW, resetH,
                                    6, 2, fill, 0x0000);

        /* label colour identical to DEL ------------------------------- */
        d.printText(centre, 2, "RESET", pal.background);
    }

    void drawUpdateBtn()
    {
        /* centre of the button */
        point c{
            static_cast<float>(d.screenSize.x - delMargin - updateW / 2),
            static_cast<float>(d.screenSize.y - delMargin - delH
                            - resetMargin - resetH
                            - updateMargin - updateH / 2)
        };

        /* same “pressed” logic as DEL / RESET */
        uint16_t fill = (touchPrev && inUpdateBtn(curX, curY))
                        ? pal.accent3      // red while finger down
                        : pal.main;        // idle colour

        d.drawBorderedRoundedRectangle(c, updateW, updateH,
                                    6, 2, fill, 0x0000);

        d.printText(c, 2, "UPDATE", pal.background);
    }




    // render everything ----------------------------------------------------
    void draw() {
        static uint32_t lastDraw = 0;
        uint32_t now = millis();

        if (!needRedraw && (now - lastDraw) < 25) return;   // skip
        lastDraw   = now;            // reached only when we paint
        needRedraw = false;          // reset for next frame


        d.fill(pal.background);
        drawPalette();
        drawEdges();
        drawBlocks();
        drawDragLine();
        drawDeleteButton();          // <‑‑ add this line
        drawResetBtn(); 
        drawUpdateBtn();
        drawGraphText();          // ←‑‑ add this line
        drawConfigStatus();      // <-- new
        drawParamWin();
        d.update();
    }

    // expose graph ---------------------------------------------------------
    const std::vector<Block>& getBlocks() const { return blocks; }

private:
    String   alertMsg;
    uint32_t alertUntil = 0;

    bool inResetBtn(int16_t x, int16_t y) const;
    bool inUpdateBtn(int16_t x, int16_t y) const;

    void drawParamWin();
    bool inParamWin(int16_t x, int16_t y) const;
    void paramWinTouchDown(int16_t x, int16_t y);

    // ── parameter‑window state ──────────────────────────────────────────────
    bool     paramWinOpen { false };   // is the param window showing?

    // ────────────────── drawing helpers ───────────────────────────────────
    void drawPalette()
    {
        int btnW = d.screenSize.x / paletteBtnCount;

        for (int i = 0; i < paletteBtnCount; ++i) {
            NodeType nt   = static_cast<NodeType>(i);          // Amp … Cab
            point centre{ static_cast<int16_t>(btnW * i + btnW / 2),
                        static_cast<int16_t>(paletteH / 2) };

            uint16_t fill   = (spawnType == nt) ? pal.accent1 : pal.main;
            d.drawBorderedRoundedRectangle(centre,
                                            static_cast<int16_t>(btnW - 4),
                                            static_cast<int16_t>(paletteH - 4),
                                            6, 2, fill, 0x0000);
            d.printText(centre, 2, String(nodeName(nt)), pal.accent2);
        }
    }

    void drawBlock(const Block& b) {
        point centre{ static_cast<int16_t>(b.x + b.w / 2), static_cast<int16_t>(b.y + b.h / 2) };
        uint16_t col = (b.id == selected) ? pal.accent3 : pal.accent1;
        d.drawBorderedRoundedRectangle(centre, b.w, b.h, 8, 2, col, 0x0000);
        d.printText(centre, 2, String(nodeName(b.type)), pal.background);
    }

    void drawBlocks() {
        for (const auto& b : blocks) drawBlock(b);
    }

    void drawEdges() {
        for (const auto& b : blocks) {
            point p1{ static_cast<int16_t>(b.x + b.w), static_cast<int16_t>(b.y + b.h / 2) };
            for (uint16_t dst : b.outs) {
                const Block* to = findBlock(dst);
                if (!to) continue;
                point p2{ to->x, static_cast<int16_t>(to->y + to->h / 2) };
                d.drawThickLine(p1, p2, 4, pal.accent2);
            }
        }
    }

    void drawDragLine() {
        if (touchPrev && dragFromId) {
            const Block* from = findBlock(dragFromId);
            if (!from) return;
            point p1{ static_cast<int16_t>(from->x + from->w), static_cast<int16_t>(from->y + from->h / 2) };
            point p2{ curX, curY };
            d.drawThickLine(p1, p2, 4, pal.accent2);
        }
    }

    // ────────────────── graph operations ──────────────────────────────────
    void addBlock(NodeType nt, int16_t x, int16_t y) {
        Block b{ nextId++, nt, x, y, blkW, blkH, {}, defaultParams(nt) };
        blocks.push_back(b);
    }

    // graph_ui.h  ─────────────────────────────────────────────────────────────
    int incomingCount(uint16_t id) const {
        int n = 0;
        for (const auto& b : blocks)
            if (std::find(b.outs.begin(), b.outs.end(), id) != b.outs.end())
                ++n;
        return n;
    }


    void connectBlocks(uint16_t from, uint16_t to) {
        Block* src = findBlock(from);
        Block* dst = findBlock(to);
        if (!src || !dst) return;
    
        // already wired?
        if (std::find(src->outs.begin(), src->outs.end(), to) != src->outs.end()) return;
    
        // no loops allowed
        if (createsCycle(from, to)) return;
    
        // ── NEW fan‑in guard ────────────────────────────────────────────────
        int limit = 1;
        //if (dst->type == NodeType::Mixer)   limit = 2;
        if (dst->type == NodeType::Input)  return;     // never feed the Input block
    
        if (incomingCount(to) >= limit) return;             // refuse, already full
        // ────────────────────────────────────────────────────────────────────
    
        src->outs.push_back(to);
    }
    

    bool createsCycle(uint16_t from, uint16_t to) {
        std::vector<uint16_t> stack{ to }, seen;
        while (!stack.empty()) {
            uint16_t cur = stack.back(); stack.pop_back();
            if (cur == from) return true;
            if (std::find(seen.begin(), seen.end(), cur) != seen.end()) continue;
            seen.push_back(cur);
            const Block* b = findBlock(cur);
            if (!b) continue;
            stack.insert(stack.end(), b->outs.begin(), b->outs.end());
        }
        return false;
    }

    // ────────────────── lookup / hit‑test helpers ─────────────────────────
    uint16_t hitBlock(int16_t x, int16_t y) const {
        for (const auto& b : blocks) {
            if (x >= b.x && x <= b.x + b.w && y >= b.y && y <= b.y + b.h)
                return b.id;
        }
        return 0;
    }

    Block* findBlock(uint16_t id) {
        for (auto& b : blocks) if (b.id == id) return &b;
        return nullptr;
    }
    const Block* findBlock(uint16_t id) const {
        for (const auto& b : blocks) if (b.id == id) return &b;
        return nullptr;
    }

    const char* nodeName(NodeType nt) const {
        static constexpr std::array<const char*, static_cast<int>(NodeType::Count)> names{
        "Reverb", "Cab", "Amp", "Phasor", "Chorus", "Delay",
        "Input", "Output"
    };
        return names[static_cast<int>(nt)];
    }

    // ── members ──────────────────────────────────────────────────────────
    display&             d;
    SimplePalette        pal;          // local colours
    std::vector<Block>   blocks;
    uint16_t             nextId{1};
    uint16_t             selected{0};
    NodeType             spawnType{NodeType::SuproAmp};

    // input‑tracking state -------------------------------------------------
    bool     touchPrev{false};
    uint16_t dragFromId{0};
    int16_t  startX{0}, startY{0};
    int16_t  curX{0},   curY{0};

    // geometry ------------------------------------------------------------
    uint16_t blkW{80}, blkH{40};
    uint16_t paletteH{40};

    bool     needRedraw{true};        // NEW – tracks “dirty” state

    // ── delete‑button geometry ──────────────────────────────────────────────
    static constexpr int16_t delW      = 80;
    static constexpr int16_t delH      = 40;
    static constexpr int16_t delMargin = 10;      // gap from screen edge

    // ── reset‑button geometry ───────────────────────────────────────
    static constexpr int resetW      = delW;          // same width as DEL
    static constexpr int resetH      = delH;
    static constexpr int resetMargin = 4;             // gap between the two

    // ── update‑button geometry ─────────────────────────────────────
    static constexpr int updateW      = resetW;     // match RESET width
    static constexpr int updateH      = resetH;
    static constexpr int updateMargin = 4;          // gap above RESET



    bool inDeleteBtn(int16_t x, int16_t y) const {
        int16_t left = d.screenSize.x - delW - delMargin;
        int16_t top  = d.screenSize.y - delH - delMargin;
        return (x >= left && x <= left + delW &&
                y >= top  && y <= top  + delH);
    }

    bool deletable(uint16_t id) const {
        const Block* b = findBlock(id);
        return b && b->type != NodeType::Input && b->type != NodeType::Output;
    }

    void deleteBlock(uint16_t id)
    {
        if (!deletable(id)) return;

        // 1. remove every edge that points to this block
        for (auto& b : blocks)
            b.outs.erase(std::remove(b.outs.begin(), b.outs.end(), id), b.outs.end());

        // 2. erase the block itself
        blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
                     [id](const Block& b){ return b.id == id; }), blocks.end());

        // 3. clear local state
        if (selected == id)  selected   = 0;
        if (dragFromId == id) dragFromId = 0;
    }

    // ──────────────────── EXPORT PATCH ────────────────────
//
// 1.  Add these two  helpers *inside* the GraphUI
//     class declaration:
//
//     std::vector<String> exportGraph() const;
//     void drawGraphText();
//
// 2.  Paste the implementations (below) **after** the class
//     definition – e.g. very end of graph_ui.h.
//

// ── helper: NodeType ➜ effect‑code table ───────────────
static inline uint8_t effectCode(NodeType nt)
{
    switch (nt) {
        case NodeType::Reverb: return 0;
        case NodeType::Cabinet:    return 1;
        case NodeType::SuproAmp:    return 2;
        case NodeType::Phasor: return 3;
        case NodeType::Chorus: return 4;
        case NodeType::Delay:  return 4;
        case NodeType::Output: return 5;        // reserved
        
//        case NodeType::Mixer:  return 6;
        
        default:               return 254;      // should never hit
    }
}

// ───────────────────── tiny helpers ──────────────────────
struct IdMap { uint16_t id; uint16_t n; };
struct InMap { uint16_t id; uint16_t label; };

static int lookup(const std::vector<IdMap>& v, uint16_t id)
{
    for (const auto& p : v) if (p.id == id) return p.n;
    return -1;                             // not found
}
static int lookupIn(const std::vector<InMap>& v, uint16_t id)
{
    for (const auto& p : v) if (p.id == id) return p.label;
    return -1;
}

};


// ── CSV exporter (no <unordered_map> needed) ─────────────
std::vector<String> GraphUI::exportGraph() const
{
    std::vector<IdMap> idToN;
    uint16_t nextN   = 2;          // N0/N1 are reserved for outputs
    uint16_t outCnt  = 0;

    // A. number the Output blocks first
    for (const auto& b : blocks)
        if (b.type == NodeType::Output)
            idToN.push_back({ b.id, (outCnt < 2) ? outCnt++ : nextN++ });

    // B. number every other non‑Input block
    for (const auto& b : blocks)
        if (b.type != NodeType::Input && b.type != NodeType::Output)
            idToN.push_back({ b.id, nextN++ });

    // C. label the first two Input blocks 255 / 254
    std::vector<InMap> inMap;
    uint16_t inCnt = 0;
    for (const auto& b : blocks)
        if (b.type == NodeType::Input && inCnt < 2)
            idToN.emplace_back(IdMap{ b.id,
                (outCnt < 2) ? outCnt++        // N0 / N1
                            : nextN++ });     // everything else
    // D. compose one CSV row per *non‑Input* block
    std::vector<String> rows;
    rows.reserve(blocks.size());

    uint8_t implicitInCnt = 0;          // ←‑‑‑ add this single line

    size_t remaining = 0;
    for (const auto& b : blocks)
        if (b.type != NodeType::Input)        // Input blocks are skipped
            ++remaining;

    for (const auto& dst : blocks) {
        if (dst.type == NodeType::Input) continue;
    
        String row = "N";
        row += lookup(idToN, dst.id);
        row += ";E";
        row += effectCode(dst.type);
        row += ";P";
        const Block* pb = &dst;   // current block
        for (size_t i=0;i<pb->params.size();++i){
            if (i==0) row += pb->params[i];    // first param touches the 'P'
            else      row += String(",") + pb->params[i];
        }

        if (dst.type == NodeType::Delay)
            row += ",0,0";        //  → gives “P50,0,0”

        row += ";I";
    
        bool first = true;
        for (const auto& src : blocks)
            for (uint16_t to : src.outs)
                if (to == dst.id) {
                    if (!first) row += ",";
                    first = false;

                    // 1) do we already know this Input (254 / 253)?
                    int lbl = lookupIn(inMap, src.id);

                    if (lbl >= 0) {                        // yes → reuse
                        row += lbl;
                    }
                    else if (src.type == NodeType::Input)        // new Input block
                    {
                        lbl = (implicitInCnt == 0) ? 254 : 253;
                        inMap.push_back(InMap{ src.id, static_cast<uint16_t>(lbl) });  // explicit
                        ++implicitInCnt;
                        row += lbl;
                    }
                    else                                   // ordinary upstream node
                        row += lookup(idToN, src.id);
                }

        /* root‑without‑sources case (optional keep‑alive) ------------------- */
        if (first) {                                 // no upstream edge at all
            int lbl = (implicitInCnt == 0) ? 254 : 253;
            ++implicitInCnt;                         // reserve the label
            row += lbl;                              // use it once
        }

        --remaining;                        // one row has just been built
        if (remaining > 0)
            row += '\n';       
        //row += "\",";                                     // finish the CSV line
        rows.push_back(row);
    }

    if (!rows.empty())
        rows.back() += ";T";          // append “;T” to the final row
    return rows;
}

// ── on‑screen preview (call once per frame from draw()) ──
void GraphUI::drawGraphText()
{
    auto csv = exportGraph();
    int16_t x = 4 + 600;
    int16_t y = 300 + paletteH + 4;
    for (size_t i = 0; i < csv.size(); ++i)
    d.printText({ static_cast<float>(x),
        static_cast<float>(y + i * 12) },
      1, csv[i], pal.main);}

// ─ graph_ui.cpp ➍ — window & keypad --------------------------------------
static constexpr int16_t pwX = 10;          // bottom‑left corner
static constexpr int16_t pwY = 480-10;      //   (origin for anchoring)
static constexpr int16_t pwW = 270;
static constexpr int16_t pwH = 170;
static constexpr int16_t keyW = 60;
static constexpr int16_t keyH = 40;
static constexpr int16_t keyPad = 4;


// ─── vertical slider layout ────────────────────────────────────────────
static constexpr int16_t slTop    = pwY - pwH + 38;       // track *top*
static constexpr int16_t slBottom = pwY - 14;             // track *bottom*
static constexpr int16_t slWidth  = 8;                    // thickness of track
static constexpr int16_t slGapX   = 38;                   // centre‑to‑centre gap
static constexpr int16_t slX0     = pwX + 30;             // first slider x‑centre


bool GraphUI::inParamWin(int16_t x, int16_t y) const
{
    int16_t left = pwX;
    int16_t top  = pwY - pwH;
    return (x >= left && x <= left + pwW &&
            y >= top  && y <= top  + pwH);
}

void GraphUI::drawParamWin()
{
    Block* b = findBlock(paramBlock);
    if (!b) return; // sanity check

    // window frame
    point c{static_cast<int16_t>(pwX + pwW / 2),
            static_cast<int16_t>(pwY - pwH / 2)};
    d.drawRoundedRectangle(c, pwW, pwH, 6, pal.background);
    d.drawBorderedRoundedRectangle(c, pwW - 4, pwH - 4, 6, 2, pal.main, 0x0000);

    // title (Effect name)
    d.printText({static_cast<float>(c.x), static_cast<float>(c.y - pwH / 2 + 10)},
                1, String(nodeName(b->type)), pal.accent2);

    // parameter list or "no params" message
    int16_t txtY = c.y - pwH / 2 + 28;
    if (b->params.empty()) {
        d.printText({static_cast<float>(c.x - pwW / 2 + 6), static_cast<float>(txtY)},
                    1, "(No parameters)", pal.accent2);
    } else {
        for (size_t i = 0; i < b->params.size(); ++i, txtY += 12) {
            bool sel = (static_cast<int>(i) == paramIndex);
            String row = String(static_cast<int>(i)) + ": " + b->params[i];
            d.printText({static_cast<float>(c.x - pwW / 2 + 6 + 200), static_cast<float>(txtY)},
                        1, row, sel ? pal.accent3 : pal.accent2);
        }
    }

    for (int i = 0; i < maxSliders; ++i) {
        ParamMeta m = paramMeta(b->type, i);
        int16_t xC  = static_cast<int16_t>(slX0 + i * slGapX);
    
        /* title */
        d.printText({ static_cast<float>(xC),
                      static_cast<float>(slTop - 12) },
                    1, String(m.title), pal.accent2);
    
        /* track */
        d.drawRectangle({ static_cast<float>(xC),
                          static_cast<float>((slTop + slBottom)/2) },
                        slWidth, slBottom - slTop, pal.accent1);
    
            /* knob only if this param exists */
        if (m.title[0] != '\0' && i < b->params.size()) {

            /* 1. read the value that is *actually* stored (int or float) */
            float vReal = b->params[i].toFloat();          // engineering units

            /* 2. convert to normalised 0‑1 for knob position */
            float vNorm = (vReal - m.min) / (m.max - m.min);
            vNorm = constrain(vNorm, 0.f, 1.f);

            /* 3. map to pixel Y */
            int16_t yK = static_cast<int16_t>(slBottom -
                            vNorm * (slBottom - slTop));

            uint16_t fill = (paramIndex == i) ? pal.accent3 : pal.accent2;
            d.drawCircle({ static_cast<float>(xC),
                        static_cast<float>(yK) }, 6, fill);
        }

    }
    
    
}

void GraphUI::paramWinTouchDown(int16_t x, int16_t y)
{
    Block* b = findBlock(selected);
    if (!b) return;

    /* which slider column? */
    int col = (x - (slX0 - slGapX / 2)) / slGapX;
    if (col < 0 || col >= maxSliders) return;

    ParamMeta m = paramMeta(b->type, col);
    if (m.title[0] == '\0') return;           // no such param

    /* Y‑pos → normalised 0‑1 */
    if (y < slTop || y > slBottom) return;
    float vNorm = float(slBottom - y) / float(slBottom - slTop);
    vNorm = constrain(vNorm, 0.f, 1.f);

    /* scale to engineering units */
    float vReal = m.min + vNorm * (m.max - m.min);

    /* store as int or float */
    if (m.isInt)  b->params[col] = String(int(vReal));
    else          b->params[col] = String(vReal, 2);   // 2 decimals

    paramIndex  = col;
    paramBlock  = b->id;
    editBuf     = b->params[col];
    needRedraw  = true;
}


// ────────────────── validity checker ─────────────────────────
bool GraphUI::isValidConfig() const
{
    for (const auto& b : blocks) {
        int in  = incomingCount(b.id);   // helper already exists :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}
        int out = b.outs.size();

        switch (b.type) {
            case NodeType::Input:
                if (out == 0)                        return false;      // must feed something
                break;

            case NodeType::Output:
                if (in == 0)                         return false;      // must be driven
                break;

            /*case NodeType::Mixer:
                if (in != 2 || out == 0)             return false;      // exactly two ins, ≥1 out
                break;
*/
            default:   /* Amp, Chorus, Phasor, Delay, Reverb, Cab */
                if (in == 0 || out == 0)             return false;      // fully connected
        }
    }
    return true;                                      // every test passed
}

// ────────────────── status banner ────────────────────────────
void GraphUI::drawConfigStatus()
{
    bool ok   = isValidConfig();
    String msg = ok ? "Valid Config" : "Invalid Config";

    /* pick a colour: accent1 (greenish) for OK, accent3 (red) for error */
    uint16_t col = ok ? pal.accent1 : pal.accent3;

    /* bottom‑left, just above the DEL button */
    int16_t x = delMargin + 4 + 600;
    int16_t y = d.screenSize.y - delH - delMargin - 14;
    /* ── optional one‑shot alert line ───────────────────────────── */
    if (millis() < alertUntil && alertMsg.length()) {
        uint16_t warnCol = pal.accent3;          // same red as “Invalid”
        d.printText({ static_cast<float>(x), static_cast<float>(y - 12) },
                    1, alertMsg, warnCol);       // 12 px above the banner
    }
    d.printText({ static_cast<float>(x), static_cast<float>(y) },
                1, msg, col);
}

inline bool GraphUI::inResetBtn(int16_t x, int16_t y) const
{
    int16_t left = d.screenSize.x - delMargin - resetW;
    int16_t top  = d.screenSize.y - delMargin - delH - resetMargin - resetH;
    return (x >= left && x < left + resetW &&
            y >= top  && y < top  + resetH);
}

inline bool GraphUI::inUpdateBtn(int16_t x, int16_t y) const
{
    int16_t left = d.screenSize.x - delMargin - updateW;
    int16_t top  = d.screenSize.y - delMargin - delH        // bottom row (DEL)
                                 - resetMargin - resetH      // middle row (RESET)
                                 - updateMargin - updateH;   // ← NEW row (UPDATE)
    return (x >= left && x < left + updateW &&
            y >= top  && y < top  + updateH);
}

void GraphUI::resetGraph()
{
    blocks.clear();
    nextId = 1;          // whatever counter you use for user nodes

    init();              // the same routine that builds Input/Output nodes
    selected   = 0;
    spawnType  = NodeType::SuproAmp;
    paramWinOpen = false;
}

void GraphUI::sendGraphNow()
{
    auto rows = exportGraph();
    for (auto& r : rows)
        Serial2.print(r);             // or whatever Serial you use
        Serial2.print("\n");
    //Serial2.println();                  // blank line terminator (optional)
}

void GraphUI::showAlert(const String& msg, uint32_t ms)
{
    alertMsg   = msg;
    alertUntil = millis() + ms;   // hide after ‘ms’ milliseconds
}
