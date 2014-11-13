
void setup()
{
}

int i=0;
void loop()
{
  
        // Example messages:
        // Note on, channel 0, middle C (48), normal velocity (64):
        // e = {0x09, 0x90, 48, 64}
        // Note off, channel 2, middle C, fastest velocity (127):
        // e = {0x08, 0x82, 48, 127}
        
     i = (i+1)%127;
     noteOn(2,0,i);
    delay(200);
    noteOff(2,0);
    delay(800);
}

void noteOn(int channel, int pitch, int velocity)
{
  MIDIEvent e = {0x09, 0x82, pitch, velocity};
  MIDIUSB.write(e);
  MIDIUSB.flush();
}

void noteOff(int channel, int pitch)
{
  MIDIEvent e = {0x08, 0x82, pitch,127};
  MIDIUSB.write(e);
  MIDIUSB.flush();
  
}
