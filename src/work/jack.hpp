/*	Program PrintJackMidi: A MIDI tool
	Author: Harry van Haaren
	E-mail: harryhaaren@gmail.com
	Copyright (C) 2010 Harry van Haaren

	PrintJackMidi is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	PrintJackMidi is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with PrintJackMidi.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef JACK
#define JACK

#include <iostream>
#include <jack/jack.h>
#include <jack/midiport.h>

class Jack
{
	public:
		Jack();
		~Jack();

		void activate();

		static int staticProcess(jack_nframes_t nframes, void *arg);

	private:
		float bpm;

		int process(jack_nframes_t nframes);

		jack_client_t*	client;
		jack_port_t*		inputPort;
		jack_port_t*		outputPort;
};

#endif


