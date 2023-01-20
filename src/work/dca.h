
void dca_update();

void init_dcas();

float envelope1_out(float, float);
void trigger_envelope1();
float release_envelope1();
void set_envelope1_attack(float);
void set_envelope1_decay(float);
void set_envelope1_sustain(float);
void set_envelope1_release(float);

float envelope2_out(float, float);
void trigger_envelope2();
float release_envelope2();
void set_envelope2_attack(float);
void set_envelope2_decay(float);
void set_envelope2_sustain(float);
void set_envelope2_release(float);

void toggle_legato();
void set_legato(int);
int get_legato();
