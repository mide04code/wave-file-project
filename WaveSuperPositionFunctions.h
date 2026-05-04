#pragma once
#pragma once
#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

int x_Screen(double x, double x0, double x1, double W) {
	return static_cast<int>((x - x0) * W / (x1 - x0));
}
int y_Screen(double y, double y0, double y1, double H) {
	return static_cast<int>((y - y1) * H / (y0 - y1));
}
double x_World(int px, double x0, double x1, double W) {
	return x0 + px * (x1 - x0) / W;
}
double y_World(int py, double y0, double y1, double H) {
	return y1 - py * (y0 - y1) / H;
}

// Two simple sine waves + superposition (your original)
void drawFunction(Graphics^ g, double wavelength1, double wavelength2, double phase1, double phase2, double amplitude1, double amplitude2, double time, int W, int H) {
	double xMin = 0.0, xMax = 10.0;
	double maxAmp = amplitude1 + amplitude2;
	double yMax = (maxAmp > 5.0) ? maxAmp : 5.0;
	double yMin = -yMax;

	if (W <= 0 || H <= 0) return;

	Pen^ pen1 = gcnew Pen(Color::FromArgb(220, 80, 120), 2); // pink-red
	Pen^ pen2 = gcnew Pen(Color::FromArgb(100, 140, 220), 2); // soft blue
	Pen^ penSum = gcnew Pen(Color::FromArgb(150, 80, 200), 2); // lavender

	double k1 = 2 * M_PI / wavelength1;
	double k2 = 2 * M_PI / wavelength2;
	double omega1 = 2.5, omega2 = 2.5;

	int py1_prev = 0, py2_prev = 0, pySum_prev = 0;

	for (int px = 0; px < W; px++) {
		double x = x_World(px, xMin, xMax, W);
		double wave1 = amplitude1 * sin(k1 * x - omega1 * time + phase1);
		double wave2 = amplitude2 * sin(k2 * x - omega2 * time + phase2);
		double waveSum = wave1 + wave2;

		int py1 = y_Screen(wave1, yMin, yMax, H);
		int py2 = y_Screen(wave2, yMin, yMax, H);
		int pySum = y_Screen(waveSum, yMin, yMax, H);

		if (px > 0) {
			g->DrawLine(pen1, px - 1, py1_prev, px, py1);
			g->DrawLine(pen2, px - 1, py2_prev, px, py2);
			g->DrawLine(penSum, px - 1, pySum_prev, px, pySum);
		}
		py1_prev = py1; py2_prev = py2; pySum_prev = pySum;
	}
}

// Instrument wave using Fourier harmonics (green)
void drawInstrumentWave(Graphics^ g, cli::array<double>^ harmonics, double wavelength, double phase, double amplitude, double time, int W, int H) {//copilot told me to do this
	double xMin = 0.0, xMax = 10.0;
	double yMax = 5.0, yMin = -5.0;
	if (W <= 0 || H <= 0) return;

	Pen^ pen = gcnew Pen(Color::FromArgb(80, 180, 130), 2); // mint green

	int py_prev = 0;
	for (int px = 0; px < W; px++) {
		double x = x_World(px, xMin, xMax, W);
		double y = 0.0;
		for (int n = 1; n <= 5; n++) {
			double k_n = n * 2 * M_PI / wavelength;
			double omega_n = n * 2.5;
			y += amplitude * harmonics[n - 1] * sin(k_n * x - omega_n * time + phase);
		}
		int py = y_Screen(y, yMin, yMax, H);
		if (px > 0) g->DrawLine(pen, px - 1, py_prev, px, py);
		py_prev = py;
	}
}
