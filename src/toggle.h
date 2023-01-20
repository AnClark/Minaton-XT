/* abGate - LV2 Noise Gate Plugin
 *
 * Copyright 2011 Antanas Bružas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _TOGGLE_H
#define _TOGGLE_H

#include <gtkmm/adjustment.h>
#include <gtkmm/misc.h>
#include <string>
#include <sstream>

using namespace std;
using namespace Gtk;

class toggle : public Misc {
	public:
		toggle(const sigc::slot<void> toggle_slot);
		~toggle();

		void connecting(Adjustment*, const sigc::slot<void> slot);
		void value_changed();
		float get_toggle_value();
		void set_toggle_value(float value);
		void set_toggle(int);
		float get_value();
		string bundle_path;

		// Dispacher used to update GUI from the main (GUI) thread
		Glib::Dispatcher dis_sig;
	
	protected:
		virtual bool on_expose_event(GdkEventExpose*);
		virtual bool on_button_press_event(GdkEventButton*);
		virtual bool on_button_release_event(GdkEventButton*);
	private:
		Glib::RefPtr<Gdk::Pixbuf> pixbuf, on, off;
		Glib::RefPtr<Gdk::Drawable> m_drawable;
		Adjustment *a_tog;
};

#endif
