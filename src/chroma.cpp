/*
 * Chromaprint -- Audio fingerprinting toolkit
 * Copyright (C) 2010  Lukas Lalinsky <lalinsky@gmail.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <limits>
#include <math.h>
#include "fft_frame.h"
#include "utils.h"
#include "chroma.h"

using namespace std;
using namespace Chromaprint;

static const int NUM_BANDS = 12;

inline double FreqToOctave(double freq, double base = 440.0 / 16.0)
{
	return log(freq / base) / log(2.0);
}

Chroma::Chroma(int min_freq, int max_freq, int frame_size, int sample_rate, FeatureVectorConsumer *consumer)
	: m_notes(frame_size),
	  m_features(NUM_BANDS),
	  m_consumer(consumer)
{
	PrepareNotes(min_freq, max_freq, frame_size, sample_rate);
}

Chroma::~Chroma()
{
}

void Chroma::PrepareNotes(int min_freq, int max_freq, int frame_size, int sample_rate)
{
	m_min_index = max(1, FreqToIndex(min_freq, frame_size, sample_rate));
	m_max_index = min(frame_size / 2, FreqToIndex(max_freq, frame_size, sample_rate));
	for (int i = m_min_index; i < m_max_index; i++) {
		double freq = IndexToFreq(i, frame_size, sample_rate);
		double octave = FreqToOctave(freq);
		m_notes[i] = NUM_BANDS * (octave - floor(octave)); 
	}
}

void Chroma::Reset()
{
}

void Chroma::Consume(const FFTFrame &frame)
{
	fill(m_features.begin(), m_features.end(), 0.0);
	for (int i = m_min_index; i < m_max_index; i++) {
		m_features[m_notes[i]] += frame.Energy(i);
	}
	m_consumer->Consume(m_features);
}
