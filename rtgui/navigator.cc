/*
 *  This file is part of RawTherapee.
 *
 *  Copyright (c) 2004-2010 Gabor Horvath <hgabor@rawtherapee.com>
 *
 *  RawTherapee is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RawTherapee is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RawTherapee.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iomanip>
#include "navigator.h"
#include "toolpanel.h"
#include "../rtengine/iccmatrices.h"
#include "../rtengine/iccstore.h"
#include "../rtengine/curves.h"
#include "../rtengine/color.h"
#include "../rtengine/rt_math.h"
#include "options.h"

extern Options options;

using namespace rtengine;

Navigator::Navigator () : currentRGBUnit(options.navRGBUnit), currentHSVUnit(options.navHSVUnit)
{

    set_label (M("MAIN_MSG_NAVIGATOR"));
    Gtk::VBox* mbox = Gtk::manage (new Gtk::VBox ());
    previewWindow = Gtk::manage (new PreviewWindow ());
    mbox->pack_start (*previewWindow, Gtk::PACK_SHRINK, 2);
    position = Gtk::manage (new Gtk::Label ());
    mbox->pack_start (*position, Gtk::PACK_SHRINK, 2);

    //labels
    lR = Gtk::manage (new Gtk::Label (M("NAVIGATOR_R")));
    lG = Gtk::manage (new Gtk::Label (M("NAVIGATOR_G")));
    lB = Gtk::manage (new Gtk::Label (M("NAVIGATOR_B")));
    lH = Gtk::manage (new Gtk::Label (M("NAVIGATOR_H")));
    lS = Gtk::manage (new Gtk::Label (M("NAVIGATOR_S")));
    lV = Gtk::manage (new Gtk::Label (M("NAVIGATOR_V")));
    lLAB_A = Gtk::manage (new Gtk::Label (M("NAVIGATOR_LAB_A")));
    lLAB_B = Gtk::manage (new Gtk::Label (M("NAVIGATOR_LAB_B")));
    lLAB_L = Gtk::manage (new Gtk::Label (M("NAVIGATOR_LAB_L")));

    // left-align labels
    lR->set_alignment(Gtk::ALIGN_LEFT);
    lG->set_alignment(Gtk::ALIGN_LEFT);
    lB->set_alignment(Gtk::ALIGN_LEFT);
    lH->set_alignment(Gtk::ALIGN_LEFT);
    lS->set_alignment(Gtk::ALIGN_LEFT);
    lV->set_alignment(Gtk::ALIGN_LEFT);
    lLAB_A->set_alignment(Gtk::ALIGN_LEFT);
    lLAB_B->set_alignment(Gtk::ALIGN_LEFT);
    lLAB_L->set_alignment(Gtk::ALIGN_LEFT);

    //values
    R = Gtk::manage (new Gtk::Label ());
    G = Gtk::manage (new Gtk::Label ());
    B = Gtk::manage (new Gtk::Label ());
    H = Gtk::manage (new Gtk::Label ());
    S = Gtk::manage (new Gtk::Label ());
    V = Gtk::manage (new Gtk::Label ());
    LAB_A = Gtk::manage (new Gtk::Label ());
    LAB_B = Gtk::manage (new Gtk::Label ());
    LAB_L = Gtk::manage (new Gtk::Label ());

    // right-align values
    R->set_alignment(Gtk::ALIGN_RIGHT);
    G->set_alignment(Gtk::ALIGN_RIGHT);
    B->set_alignment(Gtk::ALIGN_RIGHT);
    H->set_alignment(Gtk::ALIGN_RIGHT);
    S->set_alignment(Gtk::ALIGN_RIGHT);
    V->set_alignment(Gtk::ALIGN_RIGHT);
    LAB_A->set_alignment(Gtk::ALIGN_RIGHT);
    LAB_B->set_alignment(Gtk::ALIGN_RIGHT);
    LAB_L->set_alignment(Gtk::ALIGN_RIGHT);

    // set font family and size
    Glib::ustring fontname;

#ifdef WIN32
    fontname = "Droid Sans Mono Slashed"; // font file is provided in the source tree in rtdata/fonts to be installed by the windows installer
#endif

#ifdef __linux__
    fontname = "Monospace";
#endif

#ifdef __APPLE__
    fontname = "Menlo";
#endif

    if (fontname != "") {
        R->modify_font(Pango::FontDescription(fontname));
        G->modify_font(Pango::FontDescription(fontname));
        B->modify_font(Pango::FontDescription(fontname));
        H->modify_font(Pango::FontDescription(fontname));
        S->modify_font(Pango::FontDescription(fontname));
        V->modify_font(Pango::FontDescription(fontname));
        LAB_A->modify_font(Pango::FontDescription(fontname));
        LAB_B->modify_font(Pango::FontDescription(fontname));
        LAB_L->modify_font(Pango::FontDescription(fontname));

        lR->modify_font(Pango::FontDescription(fontname));
        lG->modify_font(Pango::FontDescription(fontname));
        lB->modify_font(Pango::FontDescription(fontname));
        lH->modify_font(Pango::FontDescription(fontname));
        lS->modify_font(Pango::FontDescription(fontname));
        lV->modify_font(Pango::FontDescription(fontname));
        lLAB_A->modify_font(Pango::FontDescription(fontname));
        lLAB_B->modify_font(Pango::FontDescription(fontname));
        lLAB_L->modify_font(Pango::FontDescription(fontname));

        position->modify_font(Pango::FontDescription(fontname));
    }

    // setup the tables
    Gtk::Table* table0 = Gtk::manage (new Gtk::Table (1, 3)); //rows, cols The main table container
    // let's pack tables1,2-3 into table0


    // RGB
    Gtk::EventBox *evBox1 = Gtk::manage (new Gtk::EventBox());
    Gtk::HBox* hbox1 = Gtk::manage (new Gtk::HBox ()); // container
    Gtk::Table* table1 = Gtk::manage (new Gtk::Table (3, 2));

    table1->attach (*lR, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table1->attach (*R,  1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    table1->attach (*lG, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table1->attach (*G,  1, 2, 1, 2, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    table1->attach (*lB, 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table1->attach (*B,  1, 2, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    evBox1->add (*table1);
    evBox1->signal_button_release_event().connect_notify( sigc::mem_fun(*this, &Navigator::cycleUnitsRGB));

    hbox1->pack_start (*evBox1, Gtk::PACK_EXPAND_WIDGET, 4);
    hbox1->pack_start (*Gtk::manage (new  Gtk::VSeparator()), Gtk::PACK_SHRINK, 4);
    table0->attach (*hbox1, 0, 1, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    // HSV
    Gtk::EventBox *evBox2 = Gtk::manage (new Gtk::EventBox());
    Gtk::HBox* hbox2 = Gtk::manage (new Gtk::HBox ()); // container
    Gtk::Table* table2 = Gtk::manage (new Gtk::Table (3, 2));

    table2->attach (*lH, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table2->attach (*H,  1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    table2->attach (*lS, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table2->attach (*S,  1, 2, 1, 2, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    table2->attach (*lV, 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table2->attach (*V,  1, 2, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    evBox2->add (*table2);
    evBox2->signal_button_release_event().connect_notify( sigc::mem_fun(*this, &Navigator::cycleUnitsHSV));

    hbox2->pack_start (*evBox2, Gtk::PACK_EXPAND_WIDGET, 4);
    hbox2->pack_start (*Gtk::manage (new  Gtk::VSeparator()), Gtk::PACK_SHRINK, 4);
    table0->attach (*hbox2, 1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    // LAB
    Gtk::HBox* hbox3 = Gtk::manage (new Gtk::HBox ()); // container
    Gtk::Table* table3 = Gtk::manage (new Gtk::Table (3, 2));

    table3->attach (*lLAB_L, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table3->attach (*LAB_L,  1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    table3->attach (*lLAB_A, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table3->attach (*LAB_A,  1, 2, 1, 2, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    table3->attach (*lLAB_B, 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK, 4, 0);
    table3->attach (*LAB_B,  1, 2, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    hbox3->pack_start (*table3, Gtk::PACK_EXPAND_WIDGET, 4);
    hbox3->pack_start (*Gtk::manage (new  Gtk::HBox()), Gtk::PACK_SHRINK, 2);
    table0->attach (*hbox3, 2, 3, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 0, 0);

    table0->set_homogeneous(true); // all cells will be the same size as the largest cell.

    mbox->pack_start (*table0, Gtk::PACK_EXPAND_WIDGET, 2);
    add (*mbox);

    setInvalid ();
    show_all ();
}

void Navigator::setInvalid (int fullWidth, int fullHeight)
{
    if (fullWidth > 0 && fullHeight > 0) {
        position->set_text (Glib::ustring::compose (M("NAVIGATOR_XY_FULL"), fullWidth, fullHeight));
    } else {
        position->set_text (M("NAVIGATOR_XY_NA"));
    }

    R->set_text (M("NAVIGATOR_NA"));
    G->set_text (M("NAVIGATOR_NA"));
    B->set_text (M("NAVIGATOR_NA"));
    H->set_text (M("NAVIGATOR_NA"));
    S->set_text (M("NAVIGATOR_NA"));
    V->set_text (M("NAVIGATOR_NA"));
    LAB_A->set_text (M("NAVIGATOR_NA"));
    LAB_B->set_text (M("NAVIGATOR_NA"));
    LAB_L->set_text (M("NAVIGATOR_NA"));
}

// if !validPos then x/y contain the full image size
void Navigator::pointerMoved (bool validPos, Glib::ustring profile, Glib::ustring profileW, int x, int y, int r, int g, int b)
{

    if (!validPos) {
        setInvalid (x, y);
    } else {
        position->set_text (Glib::ustring::compose ("x: %1, y: %2", x, y));

        switch (currentRGBUnit) {
        case (Options::NavigatorUnit::R0_1):
            R->set_text (Glib::ustring::format(std::fixed, std::setprecision(4), r / 255.f));
            G->set_text (Glib::ustring::format(std::fixed, std::setprecision(4), g / 255.f));
            B->set_text (Glib::ustring::format(std::fixed, std::setprecision(4), b / 255.f));
            break;
        case (Options::NavigatorUnit::R0_255):
            R->set_text (Glib::ustring::format(std::fixed, std::setprecision(0), r));
            G->set_text (Glib::ustring::format(std::fixed, std::setprecision(0), g));
            B->set_text (Glib::ustring::format(std::fixed, std::setprecision(0), b));
            break;
        case (Options::NavigatorUnit::PERCENT):
        default:
            R->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), r * 100.f / 255.f) + Glib::ustring("%"));
            G->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), g * 100.f / 255.f) + Glib::ustring("%"));
            B->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), b * 100.f / 255.f) + Glib::ustring("%"));
            break;
        }

        float h, s, v;
        Color::rgb2hsv (r * 0xffff / 0xff, g * 0xffff / 0xff, b * 0xffff / 0xff, h, s, v);
        switch (currentHSVUnit) {
        case (Options::NavigatorUnit::R0_1):
            H->set_text (Glib::ustring::format(std::fixed, std::setprecision(4), h));
            S->set_text (Glib::ustring::format(std::fixed, std::setprecision(4), s));
            V->set_text (Glib::ustring::format(std::fixed, std::setprecision(4), v));
            break;
        case (Options::NavigatorUnit::R0_255):
            H->set_text (Glib::ustring::format(std::fixed, std::setprecision(0), h * 255));
            S->set_text (Glib::ustring::format(std::fixed, std::setprecision(0), s * 255));
            V->set_text (Glib::ustring::format(std::fixed, std::setprecision(0), v * 255));
            break;
        case (Options::NavigatorUnit::PERCENT):
        default:
            H->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), h * 360.f) + Glib::ustring("\xc2\xb0"));
            S->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), s * 100.f) + Glib::ustring("%"));
            V->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), v * 100.f) + Glib::ustring("%"));
            break;
        }

        float LAB_a, LAB_b, LAB_l;
        //rgb2lab (r, g, b, LAB_l, LAB_a, LAB_b);
        Color::rgb2lab (profile, profileW, r * 0xffff / 0xff, g * 0xffff / 0xff, b * 0xffff / 0xff, LAB_l, LAB_a, LAB_b, options.rtSettings.HistogramWorking);  // TODO: Really sure this function works?
        LAB_A->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), LAB_a));
        LAB_B->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), LAB_b));
        LAB_L->set_text (Glib::ustring::format(std::fixed, std::setprecision(1), LAB_l));
    }
}

void Navigator::cycleUnitsRGB (GdkEventButton *event) {
    uint16_t v = (uint16_t)currentRGBUnit;
    ++v;
    if (v == (uint16_t)Options::NavigatorUnit::_COUNT) {
        v = 0;
    }
    options.navRGBUnit = currentRGBUnit = (Options::NavigatorUnit)v;

    switch (currentRGBUnit) {
    case Options::NavigatorUnit::R0_1:
        R->set_text ("[0-1]");
        G->set_text ("[0-1]");
        B->set_text ("[0-1]");
        break;
    case Options::NavigatorUnit::R0_255:
        R->set_text ("[0-255]");
        G->set_text ("[0-255]");
        B->set_text ("[0-255]");
        break;
    case Options::NavigatorUnit::PERCENT:
    default:
        R->set_text ("[%]");
        G->set_text ("[%]");
        B->set_text ("[%]");
        break;
    }
}

void Navigator::cycleUnitsHSV (GdkEventButton *event) {
    uint16_t v = (uint16_t)currentHSVUnit;
    ++v;
    if (v == (uint16_t)Options::NavigatorUnit::_COUNT) {
        v = 0;
    }
    options.navHSVUnit = currentHSVUnit = (Options::NavigatorUnit)v;

    switch (currentHSVUnit) {
    case Options::NavigatorUnit::R0_1:
        H->set_text ("[0-1]");
        S->set_text ("[0-1]");
        V->set_text ("[0-1]");
        break;
    case Options::NavigatorUnit::R0_255:
        H->set_text ("[0-255]");
        S->set_text ("[0-255]");
        V->set_text ("[0-255]");
        break;
    case Options::NavigatorUnit::PERCENT:
    default:
        H->set_text ("[\xc2\xb0]");
        S->set_text ("[%]");
        V->set_text ("[%]");
        break;
    }
}
