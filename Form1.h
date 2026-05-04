#pragma once // stops this file being included more than once - always required at the top of a header.

#include "WaveSuperPositionFunctions.h" // pulls in all the wave maths (drawFunction, drawInstrumentWave etc.)

namespace CppCLRWinFormsProject {

    // bring in the namespaces we use so we don't have to type System::Drawing:: everywhere
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    using namespace System::Media;


    /// <summary>
    /// Main application form - wave superposition simulator
    /// </summary>
    public ref class Form1 : public System::Windows::Forms::Form
    {
    public:

        // ── constructor - runs once when the window is first created ────────────────
        Form1(void)
        {
            // ── harmonic amplitude arrays for each instrument ──────────────────────
            // each array has 5 values = relative strength of harmonics 1 through 5
            // these are approximate real-world timbres, not exact measured values
            harmonicsViolin = gcnew array<double>{ 1.0, 0.5, 0.3, 0.2, 0.1 }; // violin: strong upper harmonics
            harmonicsGuitar = gcnew array<double>{ 1.0, 0.7, 0.4, 0.2, 0.1 }; // guitar: warm with strong 2nd harmonic
            harmonicsCello = gcnew array<double>{ 1.0, 0.6, 0.4, 0.3, 0.2 }; // cello: rich lower harmonics
            harmonicsWaves = gcnew array<double>{ 1.0, 0.0, 0.0, 0.0, 0.0 }; // placeholder for Wave 1+2 mode

            InitializeComponent(); // build all the controls defined in the #pragma region below

            this->DoubleBuffered = true; // prevents flicker during animation repaints

            // ── initial wave parameter values ─────────────────────────────────────
            currentTime = 0.0; // animation clock starts at zero
            selectedInstrument = 0;   // violin selected by default
            masterAmplitude = 1.0; // default wave height
            masterPhase = 0.0; // no phase offset at start
            masterWavelength = 2.0; // default wavelength in metres

            // ── populate the instrument list ──────────────────────────────────────
            listBox1->Items->Add(L"Violin");      // index 0
            listBox1->Items->Add(L"Guitar");      // index 1
            listBox1->Items->Add(L"Cello");       // index 2
            listBox1->Items->Add(L"Wave 1 + 2");  // index 3 - two raw sine waves

            // wire up list events BEFORE setting SelectedIndex so they fire on first selection
            listBox1->SelectedIndexChanged += gcnew EventHandler(this, &Form1::OnInstrumentChanged);



            listBox1->SelectedIndexChanged += gcnew EventHandler(this, &Form1::UpdateInstrumentImage);

            // setting SelectedIndex fires the handlers wired above
            listBox1->SelectedIndex = 0; // start with violin

            // force image update immediately on launch (in case handlers don't fire on first set)
            UpdateInstrumentImage(this, System::EventArgs::Empty);

            // ── amplitude slider (trackBar1) ───────────────────────────────────────
            trackBar1->Minimum = 1;  // lower bound - prevents wave from disappearing entirely
            trackBar1->Maximum = 20; // upper bound
            trackBar1->Value = 5;  // sensible starting height

            // ── phase shift slider (trackBar2) ─────────────────────────────────────
            trackBar2->Minimum = 0;   // no shift
            trackBar2->Maximum = 628; // 2*pi*100; divided by 100 later to get 0–6.28 radians
            trackBar2->Value = 0;   // starts with no phase offset

            // ── wavelength slider (trackBar3) ──────────────────────────────────────
            trackBar3->Minimum = 5;  // shortest allowed wavelength (avoids zero)
            trackBar3->Maximum = 40; // longest allowed wavelength
            trackBar3->Value = 14; // middle default

            // connect all three sliders to the same handler - any change triggers a redraw
            trackBar1->ValueChanged += gcnew EventHandler(this, &Form1::OnSliderChanged);
            trackBar2->ValueChanged += gcnew EventHandler(this, &Form1::OnSliderChanged);
            trackBar3->ValueChanged += gcnew EventHandler(this, &Form1::OnSliderChanged);

            // ── slider labels ──────────────────────────────────────────────────────
            label1->Text = L"amplitude";   // under trackBar1
            label2->Text = L"phase shift"; // under trackBar2
            label3->Text = L"wavelength";  // under trackBar3

            // ── title label ────────────────────────────────────────────────────────
            label4->Text = L"Wave Superposition Simulator";
            label4->Font = gcnew System::Drawing::Font("Segoe UI", 13,
                System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic);
            label4->ForeColor = Color::FromArgb(160, 110, 0); // dark golden text to suit buttercup theme

            // ── star button (top-right) - shows instrument info ────────────────────
            button1->Text = L"★";
            button1->BackColor = Color::FromArgb(255, 200, 0);  // bright gold/amber
            button1->ForeColor = Color::White;
            button1->Font = gcnew System::Drawing::Font("Segoe UI", 14,
                System::Drawing::FontStyle::Bold);
            button1->FlatStyle = FlatStyle::Flat;  // removes the raised 3-D border
            button1->FlatAppearance->BorderSize = 0;                // removes border line
            button1->UseVisualStyleBackColor = false;            // required for custom BackColor to show
            button1->Click += gcnew EventHandler(this, &Form1::starButton_Click);

            // ── help button (bottom-left) - shows usage instructions ───────────────
            button2->Text = L"!";
            button2->BackColor = Color::FromArgb(240, 85, 100); // red/pink accent
            button2->ForeColor = Color::White;
            button2->Font = gcnew System::Drawing::Font("Segoe UI", 14,
                System::Drawing::FontStyle::Bold);
            button2->FlatStyle = FlatStyle::Flat;
            button2->FlatAppearance->BorderSize = 0;
            button2->UseVisualStyleBackColor = false; // required for custom BackColor to show
            button2->Click += gcnew EventHandler(this, &Form1::helpButton_Click);





            button3 = gcnew System::Windows::Forms::Button();
            button3->Text = L"!";
            button3->BackColor = Color::FromArgb(100, 160, 255); // red/pink accent
            button3->ForeColor = Color::White;
            button3->Font = gcnew System::Drawing::Font("Segoe UI", 14, System::Drawing::FontStyle::Bold);
            button3->FlatStyle = FlatStyle::Flat;
            button3->FlatAppearance->BorderSize = 0;
            button3->UseVisualStyleBackColor = false; // required for custom BackColor to show
            button3->Click += gcnew EventHandler(this, &Form1::physicsInfo_Click);

            button3->Anchor = AnchorStyles::Bottom | AnchorStyles::Right;

            button3->Location = System::Drawing::Point(860, 400);


            this->Controls->Add(button3);


            // ── colour scheme: buttercup yellow ────────────────────────────────────
            this->BackColor = Color::FromArgb(255, 245, 180); // warm buttercup yellow for the form
            panel1->BackColor = Color::FromArgb(255, 252, 210); // slightly lighter yellow for the wave panel
            listBox1->BackColor = Color::FromArgb(255, 250, 195); // pale yellow for the instrument list
            listBox1->ForeColor = Color::FromArgb(100, 70, 0);    // dark amber text in the list
            readoutPanel->BackColor = Color::FromArgb(255, 250, 195); // FIX: was missing, left an ugly grey patch

            // ── wire up the wave drawing callback ──────────────────────────────────
            // THIS IS THE CRITICAL LINE - without it panel1 stays blank grey forever
            panel1->Paint += gcnew PaintEventHandler(this, &Form1::panel1_Paint);

            // ── configure pictureBox2 so images scale to fit ───────────────────────
            // FIX: SizeMode was never set; without Zoom images clip or sit in the corner
            pictureBox2->SizeMode = PictureBoxSizeMode::Zoom;

            // ── wire up pictureBox2 click (same info popup as the star button) ──────
            // FIX: pictureBox2 sits on top of pictureBox1 in z-order and ate all clicks,
            // but had no handler - so clicking the image did nothing. Fixed by wiring
            // pictureBox2 directly and removing the now-redundant pictureBox1 click handler.
            pictureBox2->Click += gcnew System::EventHandler(this, &Form1::pictureBox1_Click);

            // ── animation timer setup ──────────────────────────────────────────────
            timer1 = gcnew System::Windows::Forms::Timer();
            timer1->Interval = 30; // fires every 30 ms ≈ 33 fps
            timer1->Tick += gcnew EventHandler(this, &Form1::timer1_Tick);
            timer1->Start();       // start animating immediately on launch

            pictureBox2->BringToFront();

            violinBox->BringToFront();
            guitarBox->BringToFront();
            celloBox->BringToFront();



            UpdateReadout(); // populate the readout labels with correct values straight away
        }

    protected:
        /// <summary>
        /// Clean up any resources being used when the form closes.
        /// </summary>
        ~Form1()
        {
            if (components)
            {
                delete components; // free the designer container and everything it holds
            }
        }

    private:
        // ── designer container (required by Windows Forms internals) ───────────────
        System::ComponentModel::Container^ components;

        // ── animation timer ────────────────────────────────────────────────────────
        System::Windows::Forms::Timer^ timer1;

        // ── controls ───────────────────────────────────────────────────────────────
        System::Windows::Forms::Panel^ panel1;      // main wave drawing area
        System::Windows::Forms::ListBox^ listBox1;    // instrument selector on the left
        System::Windows::Forms::TrackBar^ trackBar1;   // amplitude slider
        System::Windows::Forms::TrackBar^ trackBar2;   // phase shift slider
        System::Windows::Forms::TrackBar^ trackBar3;   // wavelength slider
        System::Windows::Forms::Label^ label1;      // "amplitude" caption
        System::Windows::Forms::Label^ label2;      // "phase shift" caption
        System::Windows::Forms::Label^ label3;      // "wavelength" caption
        System::Windows::Forms::Label^ label4;      // title at the top
        System::Windows::Forms::Label^ listHintLabel; //
        System::Windows::Forms::Button^ button1;     // star info button
        System::Windows::Forms::Button^ button2;
        //help button

        System::Windows::Forms::Button^ button3;


        System::Windows::Forms::PictureBox^ violinImage; //violin Image
        System::Windows::Forms::PictureBox^ guitarImage; //guitar Image
        System::Windows::Forms::PictureBox^ celloImage;  //cello Image
        System::Windows::Forms::PictureBox^ pictureBox2; // larger picture

        System::Windows::Forms::CheckBox^ violinBox;
        System::Windows::Forms::CheckBox^ guitarBox;
        System::Windows::Forms::CheckBox^ celloBox;



        System::Windows::Forms::Panel^ readoutPanel; // box that holds the live number readouts
        System::Windows::Forms::Label^ freqLabel;    // displays frequency in Hz
        System::Windows::Forms::Label^ waveLabel;    // displays wavelength in m
        System::Windows::Forms::Label^ ampLabel;     // displays amplitude

        System::Windows::Forms::PictureBox^ pictureBox1; // invisible hit-target (kept for compatibility)
        // visible instrument image display

        // ── wave state variables ───────────────────────────────────────────────────
        double currentTime;         // elapsed animation time; increments each timer tick
        double masterAmplitude;     // overall wave height, driven by trackBar1
        double masterPhase;         // left/right phase offset in radians, driven by trackBar2
        double masterWavelength;    // spatial stretch of the wave, driven by trackBar3
        int    selectedInstrument;  // index of the currently selected instrument (0-3)

        int polarizationMode = 0;

        // ── harmonic arrays ────────────────────────────────────────────────────────
        // C++/CLI ref classes cannot hold plain 2D arrays, so one array per instrument
        array<double>^ harmonicsViolin; // harmonic amplitudes for violin
        array<double>^ harmonicsGuitar; // harmonic amplitudes for guitar
        array<double>^ harmonicsCello;  // harmonic amplitudes for cello


        array<double>^ harmonicsWaves;  // placeholder for Wave 1+2 mode (unused directly)

        // ── UpdateReadout ──────────────────────────────────────────────────────────
        // Recalculates and displays frequency, wavelength and amplitude in the readout panel.
        // Wave speed is fixed at 10 m/s so frequency = v / lambda.
        void UpdateReadout()
        {
            double waveSpeed = 10.0;                   // fixed propagation speed in m/s
            double frequency = waveSpeed / masterWavelength; // f = v / λ
            double period = 1.0 / frequency;        // T = 1/f (calculated but not displayed)

            // format each value to 2 decimal places and write to the matching label
            freqLabel->Text = L"frequency:   " + System::Math::Round(frequency, 2).ToString() + L" Hz";
            waveLabel->Text = L"wavelength:  " + System::Math::Round(masterWavelength, 2).ToString() + L" m";
            ampLabel->Text = L"amplitude:   " + System::Math::Round(masterAmplitude, 2).ToString() + L" units";

            // force the panel to repaint so the new text is visible immediately
            readoutPanel->Invalidate();
            readoutPanel->Refresh();
        }


        void playInstrumentSound(int index)
        {
            if (index < 0 || index > 2)
                return;

            String^ fileName;

            if (index == 0) fileName = L"violin.wav";
            else if (index == 1) fileName = L"guitar.wav";
            else if (index == 2) fileName = L"cello.wav";

            String^ path = System::IO::Path::Combine(Application::StartupPath, fileName);

            if (!System::IO::File::Exists(path))
            {
                MessageBox::Show(L"Sound file missing : " + path);
                return;
            }

            try
            {
                SoundPlayer^ player = gcnew SoundPlayer(path);
                player->Play();
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(ex->Message);
            }
        }




        // ── timer1_Tick ────────────────────────────────────────────────────────────
        // Called every 30 ms. Advances the animation clock and requests a repaint.
        void timer1_Tick(Object^ sender, EventArgs^ e)
        {
            currentTime += 0.05;    // advance time by a small step each frame
            panel1->Invalidate();    // mark the panel dirty so panel1_Paint fires
        }

        // ── DrawGrid ──────────────────────────────────────────────────────────────
        // Replaces the old notebook-lines background with an evenly spaced grid.
        // Horizontal lines every 40 px, vertical lines every 40 px.
        void DrawGrid(Graphics^ g, int W, int H)
        {
            // faint warm-amber horizontal and vertical lines to match the buttercup theme
            Pen^ gridPen = gcnew Pen(Color::FromArgb(60, 200, 150, 0), 1); // translucent amber

            // horizontal lines across the full panel width
            for (int y = 40; y < H; y += 40)
                g->DrawLine(gridPen, 0, y, W, y);

            // vertical lines across the full panel height
            for (int x = 40; x < W; x += 40)
                g->DrawLine(gridPen, x, 0, x, H);
        }

        // ── panel1_Paint ──────────────────────────────────────────────────────────
        // Called every time panel1 is redrawn (triggered by Invalidate in the timer tick).
        // This is where the actual waveforms are drawn.
        void panel1_Paint(Object^ sender, PaintEventArgs^ e)
        {
            Graphics^ g = e->Graphics; // the GDI+ surface we draw onto
            int W = panel1->Width;     // current pixel width of the wave panel
            int H = panel1->Height;    // current pixel height of the wave panel

            g->Clear(Color::FromArgb(255, 252, 210)); // fill background with pale buttercup yellow

            DrawGrid(g, W, H); // draw the grid lines behind the waves

            // small italic font used for the colour-key text at the top of the panel
            System::Drawing::Font^ legendFont = gcnew System::Drawing::Font(
                "Segoe UI", 8, System::Drawing::FontStyle::Italic);

            if (selectedInstrument == 3)
            {
                // ── Wave 1 + 2 mode: two overlapping sine waves ────────────────────
                g->DrawString(L"pink = wave 1   blue = wave 2   purple = sum",
                    legendFont,
                    gcnew SolidBrush(Color::FromArgb(160, 100, 130)),
                    45, 5); // top-left of panel, inside the grid margin

                // call the two-wave superposition function from WaveSuperPositionFunctions.h
                drawFunction(g,
                    masterWavelength,          // wavelength of the first sine wave
                    masterWavelength * 1.5,    // wavelength of the second sine wave (1.5× the first)
                    masterPhase,               // phase offset of wave 1
                    0.0,                       // wave 2 has no additional phase offset
                    masterAmplitude,           // height of wave 1
                    masterAmplitude * 0.8,     // wave 2 is 80% of wave 1 height
                    currentTime, W, H);        // current animation time and panel dimensions
            }
            else
            {
                // ── Instrument mode: Fourier-series waveform in green ──────────────
                g->DrawString(L"green = instrument wave (harmonics)",
                    legendFont,
                    gcnew SolidBrush(Color::FromArgb(80, 150, 110)),
                    45, 5);

                // choose the harmonic array that matches the selected instrument
                array<double>^ harmonics = gcnew array<double>(5);



                // add violin harmonics
                if (violinBox->Checked)
                {
                    for (int i = 0; i < 5; i++)
                        harmonics[i] += harmonicsViolin[i];
                }

                // add guitar harmonics
                if (guitarBox->Checked)
                {
                    for (int i = 0; i < 5; i++)
                        harmonics[i] += harmonicsGuitar[i];
                }

                //add cello harmonics
                if (celloBox->Checked)
                {
                    for (int i = 0; i < 5; i++)
                        harmonics[i] += harmonicsCello[i];
                }
                /*
                if (selectedInstrument == 0) harmonics = harmonicsViolin;
                else if (selectedInstrument == 1) harmonics = harmonicsGuitar;
                else                              harmonics = harmonicsCello;
                */
                // call the instrument wave drawing function from WaveSuperPositionFunctions.h


                if (polarizationMode == 0)
                {
                    drawInstrumentWave(g,
                        harmonics,        // harmonic amplitude data for this instrument
                        masterWavelength, // base wavelength
                        masterPhase,      // phase offset
                        masterAmplitude,  // overall amplitude scaling
                        currentTime, W, H); // animation time and panel dimensions
                }

                else if (polarizationMode == 1)
                {
                    drawInstrumentWave(g,
                        harmonics,        // harmonic amplitude data for this instrument
                        masterWavelength, // base wavelength
                        masterPhase,      // phase offset
                        masterAmplitude,  // overall amplitude scaling
                        currentTime, W, H); // animation time and panel dimensions
                }

                else if (polarizationMode == 2)
                {
                    //TODO : change this
                    drawInstrumentPolarizedX(g,
                        harmonics,        // harmonic amplitude data for this instrument
                        masterWavelength, // base wavelength
                        masterPhase,      // phase offset
                        masterAmplitude,  // overall amplitude scaling
                        currentTime, W, H); // animation time and panel dimensions
                }


            }
        }

        void drawInstrumentPolarizedX(Graphics^ g, array<double>^ harmonics, double wavelength, double phase,
            double amplitude, double time, int W, int H)
        {
            Pen^ pen = gcnew Pen(Color::Blue, 2);

            float centreY = H / 2.0f;

            for (int x = 0; x < W; x++)
            {
                double sum = 0.0;

                for (int h = 0; harmonics->Length; h++)
                {
                    double freq = (h + 1);

                    sum += harmonics[h] * Math::Sin((x / wavelength) * freq + time + phase);
                }

                sum *= amplitude;

                float px = x + (float)sum;
                float py = centreY;

                g->FillEllipse(Brushes::DarkBlue, (int)px, (int)py, 2, 2);
            }


        }
        // ── OnInstrumentChanged ───────────────────────────────────────────────────
        // Fires when the user clicks a different entry in listBox1.
        void OnInstrumentChanged(Object^ sender, EventArgs^ e)
        {
            if (listBox1->SelectedIndex >= 0)
            {
                // guard against -1 (nothing selected)
                selectedInstrument = listBox1->SelectedIndex; // store the new selection
                playInstrumentSound(selectedInstrument);
            }

            panel1->Invalidate(); // request a redraw with the new instrument's waveform
        }

        // ── OnSliderChanged ───────────────────────────────────────────────────────
        // Fires when any of the three trackbars moves.
        // Converts raw integer slider values into the physical units used by the wave functions.
        void OnSliderChanged(Object^ sender, EventArgs^ e)
        {
            masterAmplitude = trackBar1->Value * 0.2;  // maps 1–20  → amplitude  0.2–4.0
            masterPhase = trackBar2->Value * 0.01; // maps 0–628 → radians    0.0–6.28 (0–2π)
            masterWavelength = trackBar3->Value * 0.15; // maps 5–40  → wavelength 0.75–6.0 m

            // safety clamp: extremely small wavelength causes division-by-zero in UpdateReadout
            if (masterWavelength < 0.3) masterWavelength = 0.3;

            UpdateReadout();       // refresh the numerical readout labels
            panel1->Invalidate();  // redraw the wave with the new parameters
        }

        // ── helpButton_Click ──────────────────────────────────────────────────────
        // Shows a usage guide when the red '!' button is clicked.
        void helpButton_Click(Object^ sender, EventArgs^ e)
        {
            MessageBox::Show(
                L"Wave Superposition Simulator !!!\n\n"
                L"Pick an instrument on the left to see its waveform,\n"
                L"built from harmonics (Fourier Series!)\n\n"
                L"Or pick 'Wave 1 + 2' in the top left to see two sine waves\n"
                L"combining in real time.\n\n"
                L"Sliders at the bottom:\n"
                L"  left   - amplitude (wave height)\n"
                L"  middle - phase shift (left/right)\n"
                L"  right  - wavelength (stretch/squish)\n\n"
                L"Click the star to learn about each instrument!",
                L"How to use this",          // dialog title bar text
                MessageBoxButtons::OK,
                MessageBoxIcon::Information);
        }



        void physicsInfo_Click(Object^ sender, EventArgs^ e)
        {
            MessageBox::Show(
                L"Wave Superposition & Interference !!!\n\n"
                L"Superposition Principle:\n"
                L"When two or more waves meet, the resulting wave is the sum of their displacements at each point\n\n"


                L"Interference\n"
                L"This is what happens when waves overlap.\n\n"
                L"There are two types:\n"

                L"  Constructive Interference(MAX)\n"
                L"Waves are in phase -> they add together\n"
                L"Result : Bigger Amplitude \n\n"


                L"  Destructive Interference(MIN)\n"
                L"Waves are out of phase -> they cancel each other\n"
                L"Result : Smaller Amplitude \n\n",
                L"Wave Physics Explained",          // dialog title bar text
                MessageBoxButtons::OK,
                MessageBoxIcon::Information);
        }


        // ── starButton_Click ──────────────────────────────────────────────────────
        // Shows a description of whichever instrument is currently selected.
        void starButton_Click(Object^ sender, EventArgs^ e)
        {
            // one description string per instrument, same order as listBox1
            array<String^>^ info = gcnew array<String^>{
                L"Violin\n\nBright, intense tone with strong upper harmonics.\n"
                    L"The bow creates continuous vibration, keeping\n"
                    L"higher harmonics strong - that is what gives it\n"
                    L"that piercing, rich sound.",

                    L"Guitar\n\nWarm tone with a strong fundamental and 2nd harmonic.\n"
                    L"When you pluck a string the higher harmonics fade\n"
                    L"faster, giving the guitar its characteristic decay.",

                    L"Cello\n\nDeep, resonant tone with rich lower harmonics.\n"
                    L"Larger body = longer strings = lower frequencies.\n"
                    L"The harmonic series sits lower, giving it a\n"
                    L"warm, full sound.",

                    L"Wave 1 + Wave 2\n\nTwo pure sine waves added together!\n"
                    L"The pink and blue waves combine to make the\n"
                    L"purple wave. No harmonics, just maths :)"
            };

            // matching dialog title for each instrument
            array<String^>^ titles = gcnew array<String^>{
                L"About the Violin",
                    L"About the Guitar",
                    L"About the Cello",
                    L"About Wave Superposition"
            };

            // display the popup for the currently selected instrument
            MessageBox::Show(info[selectedInstrument], titles[selectedInstrument],
                MessageBoxButtons::OK, MessageBoxIcon::Information);
        }

        // ── pictureBox1_Click ─────────────────────────────────────────────────────
        // Reuses the star button info popup when the instrument image is clicked.
        // NOTE: pictureBox2 is the visible image box and is wired to this handler.
        //       pictureBox1 is kept but hidden behind pictureBox2.
        void pictureBox1_Click(Object^ sender, EventArgs^ e)
        {
            starButton_Click(sender, e); // delegate to the existing info popup
        }

        // ── UpdateInstrumentImage ─────────────────────────────────────────────────
        // Loads the correct instrument image file into pictureBox2 when the selection changes.
        // FIX: now clears the image when Wave 1+2 (index 3) is selected, instead of
        //      leaving the previous instrument's image on screen.
        void UpdateInstrumentImage(Object^ sender, EventArgs^ e)
        {
            int index = listBox1->SelectedIndex;
            String^ filename = nullptr;

            if (index == 0) filename = L"violin(10).png";
            else if (index == 1) filename = L"guitar(11).png";
            else if (index == 2) filename = L"cello(10).png";

            if (filename == nullptr)
            {
                if (pictureBox2->Image != nullptr)
                {
                    Image^ old = pictureBox2->Image;
                    pictureBox2->Image = nullptr;
                    delete old;
                }
                return;
            }

            String^ fullPath = System::IO::Path::Combine(
                System::Windows::Forms::Application::StartupPath, filename);

            // TEMPORARY: shows exactly where the code is looking
            MessageBox::Show(L"Looking for: " + fullPath, L"Debug Path");

            if (!System::IO::File::Exists(fullPath))
            {
                MessageBox::Show(L"FILE NOT FOUND: " + fullPath, L"Missing Image");
                return;
            }

            try
            {
                if (pictureBox2->Image != nullptr)
                {
                    Image^ old = pictureBox2->Image;
                    pictureBox2->Image = nullptr;
                    delete old;
                }
                pictureBox2->Image = Image::FromFile(fullPath);
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(L"Error: " + ex->Message, L"Image Load Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
            }
        }
#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify the contents of this method.
        /// </summary>
        ///

        void InitializeComponent(void)
        {
            this->panel1 = (gcnew System::Windows::Forms::Panel());
            this->listBox1 = (gcnew System::Windows::Forms::ListBox());
            this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
            this->trackBar2 = (gcnew System::Windows::Forms::TrackBar());
            this->trackBar3 = (gcnew System::Windows::Forms::TrackBar());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->label3 = (gcnew System::Windows::Forms::Label());
            this->label4 = (gcnew System::Windows::Forms::Label());
            this->listHintLabel = (gcnew System::Windows::Forms::Label());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->button2 = (gcnew System::Windows::Forms::Button());
            this->readoutPanel = (gcnew System::Windows::Forms::Panel());
            this->freqLabel = (gcnew System::Windows::Forms::Label());
            this->waveLabel = (gcnew System::Windows::Forms::Label());
            this->ampLabel = (gcnew System::Windows::Forms::Label());
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
            this->violinBox = (gcnew System::Windows::Forms::CheckBox());
            this->guitarBox = (gcnew System::Windows::Forms::CheckBox());
            this->celloBox = (gcnew System::Windows::Forms::CheckBox());
            this->violinImage = (gcnew System::Windows::Forms::PictureBox());
            this->guitarImage = (gcnew System::Windows::Forms::PictureBox());
            this->celloImage = (gcnew System::Windows::Forms::PictureBox());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar2))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar3))->BeginInit();
            this->readoutPanel->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->violinImage))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->guitarImage))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->celloImage))->BeginInit();
            this->SuspendLayout();
            //
            // panel1
            //
            this->panel1->Location = System::Drawing::Point(131, 37);
            this->panel1->Margin = System::Windows::Forms::Padding(2);
            this->panel1->Name = L"panel1";
            this->panel1->Size = System::Drawing::Size(645, 341);
            this->panel1->TabIndex = 0;
            //
            // listBox1
            //
            this->listBox1->FormattingEnabled = true;
            this->listBox1->Location = System::Drawing::Point(8, 37);
            this->listBox1->Margin = System::Windows::Forms::Padding(2);
            this->listBox1->Name = L"listBox1";
            this->listBox1->Size = System::Drawing::Size(117, 82);
            this->listBox1->TabIndex = 1;
            //
            // trackBar1
            //
            this->trackBar1->Location = System::Drawing::Point(131, 386);
            this->trackBar1->Margin = System::Windows::Forms::Padding(2);
            this->trackBar1->Name = L"trackBar1";
            this->trackBar1->Size = System::Drawing::Size(150, 45);
            this->trackBar1->TabIndex = 2;
            //
            // trackBar2
            //
            this->trackBar2->Location = System::Drawing::Point(311, 386);
            this->trackBar2->Margin = System::Windows::Forms::Padding(2);
            this->trackBar2->Name = L"trackBar2";
            this->trackBar2->Size = System::Drawing::Size(150, 45);
            this->trackBar2->TabIndex = 3;
            //
            // trackBar3
            //
            this->trackBar3->Location = System::Drawing::Point(491, 386);
            this->trackBar3->Margin = System::Windows::Forms::Padding(2);
            this->trackBar3->Name = L"trackBar3";
            this->trackBar3->Size = System::Drawing::Size(150, 45);
            this->trackBar3->TabIndex = 4;
            //
            // label1
            //
            this->label1->Location = System::Drawing::Point(165, 427);
            this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(75, 16);
            this->label1->TabIndex = 5;
            //
            // label2
            //
            this->label2->Location = System::Drawing::Point(345, 427);
            this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->label2->Name = L"label2";
            this->label2->Size = System::Drawing::Size(75, 16);
            this->label2->TabIndex = 6;
            //
            // label3
            //
            this->label3->Location = System::Drawing::Point(525, 427);
            this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->label3->Name = L"label3";
            this->label3->Size = System::Drawing::Size(75, 16);
            this->label3->TabIndex = 7;
            //
            // label4
            //
            this->label4->Location = System::Drawing::Point(8, 6);
            this->label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->label4->Name = L"label4";
            this->label4->Size = System::Drawing::Size(375, 24);
            this->label4->TabIndex = 8;
            //
            // listHintLabel
            //
            this->listHintLabel->Font = (gcnew System::Drawing::Font(L"Segoe UI", 8, System::Drawing::FontStyle::Italic));
            this->listHintLabel->Location = System::Drawing::Point(8, 125);
            this->listHintLabel->Name = L"listHintLabel";
            this->listHintLabel->Size = System::Drawing::Size(117, 42);
            this->listHintLabel->TabIndex = 9;
            this->listHintLabel->Text = L"Click an instrument\nto view its image.";
            //
            // button1
            //
            this->button1->Location = System::Drawing::Point(896, 6);
            this->button1->Margin = System::Windows::Forms::Padding(2);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(30, 32);
            this->button1->TabIndex = 9;
            //
            // button2
            //
            this->button2->Location = System::Drawing::Point(92, 378);
            this->button2->Margin = System::Windows::Forms::Padding(2);
            this->button2->Name = L"button2";
            this->button2->Size = System::Drawing::Size(30, 32);
            this->button2->TabIndex = 10;
            //
            // readoutPanel
            //
            this->readoutPanel->Controls->Add(this->freqLabel);
            this->readoutPanel->Controls->Add(this->waveLabel);
            this->readoutPanel->Controls->Add(this->ampLabel);
            this->readoutPanel->Location = System::Drawing::Point(784, 37);
            this->readoutPanel->Margin = System::Windows::Forms::Padding(2);
            this->readoutPanel->Name = L"readoutPanel";
            this->readoutPanel->Size = System::Drawing::Size(150, 341);
            this->readoutPanel->TabIndex = 11;
            //
            // freqLabel
            //
            this->freqLabel->Location = System::Drawing::Point(8, 24);
            this->freqLabel->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->freqLabel->Name = L"freqLabel";
            this->freqLabel->Size = System::Drawing::Size(135, 24);
            this->freqLabel->TabIndex = 0;
            //
            // waveLabel
            //
            this->waveLabel->Location = System::Drawing::Point(8, 65);
            this->waveLabel->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->waveLabel->Name = L"waveLabel";
            this->waveLabel->Size = System::Drawing::Size(135, 24);
            this->waveLabel->TabIndex = 1;
            //
            // ampLabel
            //
            this->ampLabel->Location = System::Drawing::Point(8, 106);
            this->ampLabel->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
            this->ampLabel->Name = L"ampLabel";
            this->ampLabel->Size = System::Drawing::Size(135, 24);
            this->ampLabel->TabIndex = 2;
            //
            // pictureBox1
            //
            this->pictureBox1->Location = System::Drawing::Point(5, 209);
            this->pictureBox1->Margin = System::Windows::Forms::Padding(2);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(117, 141);
            this->pictureBox1->TabIndex = 0;
            this->pictureBox1->TabStop = false;
            //
            // pictureBox2
            //
            this->pictureBox2->Location = System::Drawing::Point(7, 155);
            this->pictureBox2->Margin = System::Windows::Forms::Padding(2);
            this->pictureBox2->Name = L"pictureBox2";
            this->pictureBox2->Size = System::Drawing::Size(112, 195);
            this->pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
            this->pictureBox2->TabIndex = 12;
            this->pictureBox2->TabStop = false;
            //
            // violinBox
            //
            this->violinBox->Checked = true;
            this->violinBox->CheckState = System::Windows::Forms::CheckState::Checked;
            this->violinBox->Location = System::Drawing::Point(7, 354);
            this->violinBox->Margin = System::Windows::Forms::Padding(2);
            this->violinBox->Name = L"violinBox";
            this->violinBox->Size = System::Drawing::Size(78, 20);
            this->violinBox->TabIndex = 13;
            this->violinBox->Text = L"Violin";
            //
            // guitarBox
            //
            this->guitarBox->Location = System::Drawing::Point(7, 378);
            this->guitarBox->Margin = System::Windows::Forms::Padding(2);
            this->guitarBox->Name = L"guitarBox";
            this->guitarBox->Size = System::Drawing::Size(78, 20);
            this->guitarBox->TabIndex = 14;
            this->guitarBox->Text = L"Guitar";
            //
            // celloBox
            //
            this->celloBox->Location = System::Drawing::Point(7, 402);
            this->celloBox->Margin = System::Windows::Forms::Padding(2);
            this->celloBox->Name = L"celloBox";
            this->celloBox->Size = System::Drawing::Size(78, 20);
            this->celloBox->TabIndex = 15;
            this->celloBox->Text = L"Cello";
            //
            // violinImage
            //
            this->violinImage->Location = System::Drawing::Point(0, 0);
            this->violinImage->Name = L"violinImage";
            this->violinImage->Size = System::Drawing::Size(100, 50);
            this->violinImage->TabIndex = 0;
            this->violinImage->TabStop = false;
            //
            // guitarImage
            //
            this->guitarImage->Location = System::Drawing::Point(0, 0);
            this->guitarImage->Name = L"guitarImage";
            this->guitarImage->Size = System::Drawing::Size(100, 50);
            this->guitarImage->TabIndex = 0;
            this->guitarImage->TabStop = false;
            //
            // celloImage
            //
            this->celloImage->Location = System::Drawing::Point(0, 0);
            this->celloImage->Name = L"celloImage";
            this->celloImage->Size = System::Drawing::Size(100, 50);
            this->celloImage->TabIndex = 0;
            this->celloImage->TabStop = false;
            //
            // Form1
            //
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(938, 455);
            this->Controls->Add(this->panel1);
            this->Controls->Add(this->listBox1);
            this->Controls->Add(this->trackBar1);
            this->Controls->Add(this->trackBar2);
            this->Controls->Add(this->trackBar3);
            this->Controls->Add(this->label1);
            this->Controls->Add(this->label2);
            this->Controls->Add(this->label3);
            this->Controls->Add(this->label4);
            this->Controls->Add(this->listHintLabel);
            this->Controls->Add(this->button1);
            this->Controls->Add(this->button2);
            this->Controls->Add(this->readoutPanel);
            this->Controls->Add(this->pictureBox1);
            this->Controls->Add(this->pictureBox2);
            this->Controls->Add(this->violinBox);
            this->Controls->Add(this->guitarBox);
            this->Controls->Add(this->celloBox);
            this->Margin = System::Windows::Forms::Padding(2);
            this->Name = L"Form1";
            this->Text = L"Wave Superposition Simulator";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar2))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar3))->EndInit();
            this->readoutPanel->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->violinImage))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->guitarImage))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->celloImage))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        } // ── end of InitializeComponent ───────────────────────────────────────────
        void OnInstrumentToggle(Object^ sender, EventArgs^ e)
        {
            panel1->Invalidate();
        }
#pragma endregion
    }; // ── end of Form1 class ───────────────────────────────────────────────────────
}      // ── end of CppCLRWinFormsProject namespace ────────────────────────────────────

