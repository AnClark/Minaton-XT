
BUNDLE = minaton.lv2
INSTALL_DIR = ~/.lv2
CFLAGS = -ljack -lsndfile -lsamplerate


$(BUNDLE): manifest.ttl minaton.ttl minaton.so minaton-gui.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp manifest.ttl minaton.ttl minaton.so minaton-gui.so $(BUNDLE)

minaton.so: minaton.cpp
	g++ -shared -Xlinker -zmuldefs -fPIC -DPIC $(CFLAGS) minaton.cpp synth.cpp `pkg-config --cflags --libs lv2-plugin` -o minaton.so 

minaton-gui.so: minaton-gui.cpp
	g++ -shared -fPIC -DPIC minaton-gui.cpp main_window.cpp knob.cpp toggle.cpp `pkg-config --cflags --libs lv2-gui` -o minaton-gui.so

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	cp -R $(BUNDLE) $(INSTALL_DIR)
	cp -r waves $(INSTALL_DIR)/$(BUNDLE)
	cp -r patches $(INSTALL_DIR)/$(BUNDLE)
	cp -r gfx $(INSTALL_DIR)/$(BUNDLE)
clean:
	rm -rf $(BUNDLE) minaton.so
	rm minaton-gui.so

uninstall:
	rm -rf $(INSTALL_DIR)/minaton.lv2








