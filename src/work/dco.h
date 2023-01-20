#include <string>
using namespace std;

void init_src();
void dco_on(int);
void dco_off(int);
int get_dco_state(int);
int add_wave(string, string);
void add_dco();
float get_dco_out(int);
void dco_cycle(int);
float get_dco_frequency(int);
void set_dco_frequency(int, float);
float get_dco_volume(int);
void set_dco_volume(int, float);
int get_dco_wave(int);
void set_dco_wave(int, int);
float get_dco_inertia(int);
void set_dco_inertia(int, float);
void set_dco_lfo1_amount(int, float);
void set_dco_lfo2_amount(int, float);

void set_tuning(float);
void toggle_sync();
void set_sync_mode(int value);
int get_sync_mode();

float get_dco_frequency(int);
int get_dco_wave(int);
float get_dco_lfo1_amount(int);
float get_dco_lfo2_amount(int);

void update_dco1_tuning();
void update_dco2_tuning();
void update_dco3_tuning();
