#include "DFRobotDFPlayerMini.h"

extern DFRobotDFPlayerMini DFPlayer;

void playSound(int track) {
  Serial.print("[AUDIO] Playing track: ");
  Serial.println(track);
  DFPlayer.play(track);
}

void playVictorySound() {
  Serial.println("[AUDIO] Playing Victory!");
  DFPlayer.playMp3Folder(2); // Correspond à end_animation dans l'ancien code
}

void setVolume(int volume) {
  if (volume < 0) volume = 0;
  if (volume > 30) volume = 30;
  DFPlayer.volume(volume);
}
